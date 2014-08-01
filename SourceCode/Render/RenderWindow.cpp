#include "stdafx.h"
#include "RenderWindow.h"
#include "Event/BaseEvent.h"
#include "Event/EventDispatcher.h"
#include "CompWrapper.h"
#include "RenderManager.h"
#include "GUI/WindowManager.h"

CRenderWindow::CRenderWindow(size_t nDeviceWidth, size_t nDeviceHeight, bool bAutoCreateViewport)
    : CRenderTarget(nDeviceWidth, nDeviceHeight, bAutoCreateViewport)
    , m_pEventDispatcher(nullptr)
    , m_nDeviceWidth(nDeviceWidth)
    , m_nDeviceHeight(nDeviceHeight)
{
    m_pEventDispatcher = new CCompWrapper<CEventDispatcher, CRenderWindow>(this);
    SetDeviceResolution(nDeviceWidth, nDeviceHeight);
}

CRenderWindow::~CRenderWindow()
{
    BEATS_SAFE_DELETE(m_pEventDispatcher);
}

CEventDispatcher *CRenderWindow::EventDispatcher() const
{
    return m_pEventDispatcher;
}

void CRenderWindow::SetDeviceResolution(size_t nDeviceWidth, size_t nDeviceHeight)
{
    m_nDeviceWidth = nDeviceWidth;
    m_nDeviceHeight = nDeviceHeight;

    AdjustResolution();

    CWindowManager::GetInstance()->OnSizeChange();
}

void CRenderWindow::AdjustResolution()
{
    float fDeviceRatio = (float)m_nDeviceWidth /  m_nDeviceHeight;
    size_t nWidth = CRenderManager::GetDesignWidth();
    size_t nHeight = CRenderManager::GetDesignHeight();
    if(fDeviceRatio < CRenderManager::GetDesignRatio())
    {
        nHeight = (size_t)(nWidth / fDeviceRatio);
    }
    else if(fDeviceRatio > CRenderManager::GetDesignRatio())
    {
        nWidth = (size_t)(nHeight * fDeviceRatio);
    }
    SetSize(nWidth, nHeight);
    SetScaleFactor((float)m_nDeviceWidth / nWidth);
}

size_t CRenderWindow::GetDeviceWidth() const
{
    return m_nDeviceWidth;
}

size_t CRenderWindow::GetDeviceHeight() const
{
    return m_nDeviceHeight;
}
