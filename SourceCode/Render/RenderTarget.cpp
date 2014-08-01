#include "stdafx.h"
#include "RenderTarget.h"
#include "Render/Viewport.h"
#include "Render/RenderManager.h"
#include "Event/BaseEvent.h"
#include "GUI/WindowManager.h"
#include "RenderGroupManager.h"
#include "Scene/SceneManager.h"

CRenderTarget::CRenderTarget(size_t nWidth, size_t nHeight, bool bAutoCreateViewport, float fScaleFactor)
    : m_uWidth(nWidth)
    , m_uHeight(nHeight)
    , m_bAutoCreateViewport(bAutoCreateViewport)
    , m_fScaleFactor(fScaleFactor)
{
    if(m_bAutoCreateViewport)
    {
        m_pViewport = new CViewport(this);
    }
}

CRenderTarget::~CRenderTarget()
{
    BEATS_SAFE_DELETE(m_pViewport);
}

void CRenderTarget::SetSize(size_t uWidth, size_t uHeight)
{
    if( m_uWidth != uWidth ||
        m_uHeight != uHeight )
    {
        m_uWidth = uWidth;
        m_uHeight = uHeight;

        if(m_pViewport != NULL)
        {
            m_pViewport->SetLeft(0);
            m_pViewport->SetTop(0);
            m_pViewport->SetWidth(m_uWidth);
            m_pViewport->SetHeight(m_uHeight);
        }
    }
}

size_t CRenderTarget::GetWidth() const
{
    return m_uWidth;
}

size_t CRenderTarget::GetHeight() const
{
    return m_uHeight;
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
    pViewport->SetRenderTarget(this);
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
    m_pViewport->Apply();
    CSceneManager::GetInstance()->RenderScene();
    CWindowManager::GetInstance()->Render();
    CRenderManager::GetInstance()->Render();

}