#include "stdafx.h"
#include "RenderTarget.h"
#include "Render/Viewport.h"
#include "Render/RenderManager.h"
#include "Event/BaseEvent.h"
#include "Scene/SceneManager.h"
#include "Renderer.h"
#include "GUI/Font/FreetypeFontFace.h"
#include "Texture.h"
#include "CoordinateRenderObject.h"
#include "ParticleSystem/ParticleManager.h"

CRenderTarget::CRenderTarget(uint32_t uDeviceWidth, uint32_t uDeviceHeight)
    : m_uLogicWidth(0)
    , m_uLogicHeight(0)
    , m_uDeviceWidth(0)
    , m_uDeviceHeight(0)
    , m_uPostProcessFBO(0)
    , m_uPostProcessTexture(0)
    , m_uPostProcessDepthBuffer(0)
    , m_bUsePostProcess(true)
    , m_pViewport(new CViewport(this))
    , m_fScaleFactor(1.0f)
{
    SetDeviceSize(uDeviceWidth, uDeviceHeight);
}

CRenderTarget::~CRenderTarget()
{
    BEATS_ASSERT(m_pViewport == nullptr);
}

void CRenderTarget::Uninitialize()
{
    BEATS_SAFE_DELETE(m_pViewport);
    if (m_bUsePostProcess)
    {
        DeleteFBO();
    }
}

void CRenderTarget::SetDeviceSize(uint32_t uDeviceWidth, uint32_t uDeviceHeight, bool bUpdateLogicPos, bool bUpdateWindowManager)
{
    if (uDeviceWidth != m_uDeviceWidth ||
        uDeviceHeight != m_uDeviceHeight)
    {
        // To avoid call here when opengl is not ready, we will create fbo when rendered, and when size changed for the existing fbo.
        if (m_bUsePostProcess && m_uPostProcessFBO != 0)
        {
            RecreateFBO(uDeviceWidth, uDeviceHeight);
        }
        m_uDeviceWidth = uDeviceWidth;
        m_uDeviceHeight = uDeviceHeight;

        if (bUpdateLogicPos)
        {
            uint32_t nDesignWidth = CRenderManager::GetDesignWidth();
            uint32_t nDesignHeight = CRenderManager::GetDesignHeight();
            float fDeviceRatio = (float)m_uDeviceWidth / m_uDeviceHeight;
            float fDesignRation = (float)nDesignWidth / nDesignHeight;
            if (fDeviceRatio < fDesignRation)
            {
                nDesignHeight = (uint32_t)(nDesignWidth / fDeviceRatio);
            }
            else if (fDeviceRatio > fDesignRation)
            {
                nDesignWidth = (uint32_t)(nDesignHeight * fDeviceRatio);
            }
            SetLogicSize(nDesignWidth, nDesignHeight);
            SetScaleFactor((float)m_uDeviceWidth / nDesignWidth);
        }
        if (m_pViewport != NULL)
        {
            m_pViewport->SetLeft(0);
            m_pViewport->SetTop(0);
            m_pViewport->SetWidth(m_uDeviceWidth);
            m_pViewport->SetHeight(m_uDeviceHeight);
        }
        BEYONDENGINE_UNUSED_PARAM(bUpdateWindowManager);
    }
}

void CRenderTarget::DeleteFBO()
{
    if (m_uPostProcessFBO != 0)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        pRenderer->DeleteFramebuffer(1, &m_uPostProcessFBO);
        m_uPostProcessFBO = 0;
        pRenderer->DeleteTexture(1, &m_uPostProcessTexture);
        m_uPostProcessTexture = 0;
        pRenderer->DeleteRenderbuffer(1, &m_uPostProcessDepthBuffer);
        m_uPostProcessDepthBuffer = 0;
    }
}

void CRenderTarget::SetLogicSize(uint32_t uLogicWidth, uint32_t uLogicHeight)
{
    if( m_uLogicWidth != uLogicWidth ||
        m_uLogicHeight != uLogicHeight )
    {
        m_uLogicWidth = uLogicWidth;
        m_uLogicHeight = uLogicHeight;
    }
}

uint32_t CRenderTarget::GetDeviceWidth() const
{
    return m_uDeviceWidth;
}

uint32_t CRenderTarget::GetDeviceHeight() const
{
    return m_uDeviceHeight;
}

uint32_t CRenderTarget::GetLogicWidth() const
{
    return m_uLogicWidth;
}

uint32_t CRenderTarget::GetLogicHeight() const
{
    return m_uLogicHeight;
}

void CRenderTarget::SetScaleFactor(float fScaleFactor)
{
    m_fScaleFactor = fScaleFactor;
}

float CRenderTarget::GetScaleFactor() const
{
    return m_fScaleFactor;
}

void CRenderTarget::SetViewport(CViewport *pViewport)
{
    m_pViewport = pViewport;
}

CViewport *CRenderTarget::GetViewport() const
{
    return m_pViewport;
}

void CRenderTarget::SetToCurrent()
{

}

void CRenderTarget::Render()
{
    if (m_bRenderSwitch)
    {
        GLint nOriginFrameBuffer = 0;
        if (m_bUsePostProcess)
        {
            CRenderer::GetInstance()->GetIntegerV(GL_FRAMEBUFFER_BINDING, &nOriginFrameBuffer);
            if (m_uPostProcessFBO == 0)
            {
                RecreateFBO(m_uDeviceWidth, m_uDeviceHeight);
            }
            CRenderer::GetInstance()->BindFramebuffer(GL_FRAMEBUFFER, m_uPostProcessFBO);
        }
        BEYONDENGINE_PERFORMDETECT_START(ePNT_ApplyViewPort);
        m_pViewport->Apply();
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ApplyViewPort);
        if (BEATS_FLOAT_GREATER(CRenderManager::GetInstance()->GetGlobalColorFactor(), 0))
        {
            BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderScene);
            CSceneManager::GetInstance()->RenderScene();
            BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderScene);
            CParticleManager::GetInstance()->Render();
        }
#ifdef DEVELOP_VERSION
        CCoordinateRenderObject* pCoordinateRenderObject = CRenderManager::GetInstance()->GetCoordinateRenderObject();
        if (pCoordinateRenderObject->GetRenderNode() != nullptr)
        {
            pCoordinateRenderObject->Render();
        }
#endif
        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderManagerRender);
        CRenderManager::GetInstance()->Render();
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderManagerRender);
        if (m_bUsePostProcess)
        {
            CRenderer::GetInstance()->BindFramebuffer(GL_FRAMEBUFFER, nOriginFrameBuffer);
            CRenderManager::GetInstance()->RenderTextureToFullScreen(m_uPostProcessTexture);
        }
#ifdef DEVELOP_VERSION
        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderLogManager);
        CLogManager::GetInstance()->RenderLog();
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderLogManager);
#endif
    }
    else
    {
        CRenderManager::GetInstance()->Reset();
    }
}

void CRenderTarget::RecreateFBO(uint32_t uWidth, uint32_t uHeight)
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    GLint nOriginFrameBuffer = 0;
    pRenderer->GetIntegerV(GL_FRAMEBUFFER_BINDING, &nOriginFrameBuffer);
    if (m_uPostProcessFBO != 0)
    {
        DeleteFBO();
    }
    //FBO stuff
    pRenderer->GenFramebuffers(1, &m_uPostProcessFBO);
    pRenderer->BindFramebuffer(GL_FRAMEBUFFER, m_uPostProcessFBO);
    pRenderer->GenTextures(1, &m_uPostProcessTexture);
    pRenderer->BindTexture(GL_TEXTURE_2D, m_uPostProcessTexture);
    pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    pRenderer->TextureImage2D(GL_TEXTURE_2D, 0, GL_RGBA, uWidth, uHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    pRenderer->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_uPostProcessTexture, 0);

    pRenderer->GenRenderbuffers(1, &m_uPostProcessDepthBuffer);
    pRenderer->BindRenderbuffer(GL_RENDERBUFFER, m_uPostProcessDepthBuffer);
    pRenderer->RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, uWidth, uHeight);
    pRenderer->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uPostProcessDepthBuffer);
    BEATS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, _T("FBO initialize failed!"));
    pRenderer->BindTexture(GL_TEXTURE_2D, 0);
    pRenderer->BindFramebuffer(GL_FRAMEBUFFER, nOriginFrameBuffer);
}

void CRenderTarget::SetUsePostProcess(bool bUsePostProcess)
{
    m_bUsePostProcess = bUsePostProcess;
}

bool CRenderTarget::IsUsePostProcess() const
{
    return m_bUsePostProcess;
}

uint32_t CRenderTarget::GetFBOTexture() const
{
    return m_uPostProcessTexture;
}

void CRenderTarget::SetFBOTexture(uint32_t uId)
{
    m_uPostProcessTexture = uId;
}

void CRenderTarget::SetRenderSwitcher(bool bRenderSwitcher)
{
    m_bRenderSwitch = bRenderSwitcher;
}

bool CRenderTarget::GetRenderSwitcher() const
{
    return m_bRenderSwitch;
}