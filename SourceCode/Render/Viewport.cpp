#include "stdafx.h"
#include "Viewport.h"
#include "Renderer.h"
#include "Render/RenderTarget.h"
#ifdef EDITOR_MODE
#include "BeyondEngineEditor/EditorConfig.h"
#endif

CViewport::CViewport(int nLeft, int nTop, int nWidth, int nHeight,
                     GLbitfield clearFlag, CColor clearColor,
                     float fClearDepth, int nClearStencil)
                     : m_nLeft(nLeft)
                     , m_nTop(nTop)
                     , m_nWidth(nWidth)
                     , m_nHeight(nHeight)
                     , m_clearFlags(clearFlag)
                     , m_fClearDepth(fClearDepth)
                     , m_nClearStencil(nClearStencil)
#ifdef EDITOR_MODE
                     , m_clearColor(CEditorConfig::GetInstance()->GetSceneGridParam()->BGColor)
#else
                     , m_clearColor(clearColor)
#endif
{
#ifdef EDITOR_MODE
    BEYONDENGINE_UNUSED_PARAM(clearColor);
#endif
}

CViewport::CViewport(CRenderTarget *pRenderTarget, GLbitfield clearFlag, 
                     CColor clearColor, float fClearDepth, int nClearStencil)
                     : m_nLeft(0)
                     , m_nTop(0)
                     , m_nWidth(pRenderTarget->GetDeviceWidth())
                     , m_nHeight(pRenderTarget->GetDeviceHeight())
                     , m_clearFlags(clearFlag)
                     , m_fClearDepth(fClearDepth)
                     , m_nClearStencil(nClearStencil)
#ifdef EDITOR_MODE
                     , m_clearColor(CEditorConfig::GetInstance()->GetSceneGridParam()->BGColor)
#else
                     , m_clearColor(clearColor)
#endif
{
#ifdef EDITOR_MODE
    BEYONDENGINE_UNUSED_PARAM(clearColor);
#endif
}

CViewport::~CViewport()
{
}

void CViewport::SetLeft(int nLeft)
{
    m_nLeft = nLeft;
}

void CViewport::SetTop(int nTop)
{
    m_nTop = nTop;
}

void CViewport::SetWidth(int nWidth)
{
    m_nWidth = nWidth;
}

void CViewport::SetHeight(int nHeight)
{
    m_nHeight = nHeight;
}

int CViewport::Left() const
{
    return m_nLeft;
}

int CViewport::Top() const
{
    return m_nTop;
}

int CViewport::Width() const
{
    return m_nWidth;
}

int CViewport::Height() const
{
    return m_nHeight;
}

void CViewport::SetClearFlag(GLbitfield clearFlag)
{
    m_clearFlags = clearFlag;
}

void CViewport::SetClearColor(CColor color)
{
    m_clearColor = color;
}

void CViewport::SetClearDepth(float fClearDepth)
{
    m_fClearDepth = fClearDepth;
}

void CViewport::SetClearStencil(int nClearStencil)
{
    m_nClearStencil = nClearStencil;
}

void CViewport::Apply() const
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->Viewport(m_nLeft, m_nTop, m_nWidth, m_nHeight);
    if (m_clearFlags & GL_DEPTH_BUFFER_BIT)
    {
        //if the material set the write mask false,his clear is invalid,
        // so before call clear depth, you must set the write mask true
        pRenderer->DepthMask(true);
        pRenderer->ClearDepth(m_fClearDepth);
    }
    if(m_clearFlags & GL_COLOR_BUFFER_BIT)
    {
        pRenderer->ClearColor(
            (float)m_clearColor.r / 0xFF,
            (float)m_clearColor.g / 0xFF,
            (float)m_clearColor.b / 0xFF,
            (float)m_clearColor.a / 0xFF);
    }
    if(m_clearFlags & GL_STENCIL_BUFFER_BIT)
    {
        pRenderer->ClearStencil(m_nClearStencil);
    }

    // Should disable scissor test due the clear region is
    // relied on scissor box bounds.
    pRenderer->ScissorTest(false);
    // Clear buffers
    pRenderer->ClearBuffer(m_clearFlags);
}

CColor CViewport::GetClearColor() const
{
    return m_clearColor;
}
