#include "stdafx.h"
#include "GameViewAgent.h"
#include "EditorMainFrame.h"
#include "wx/generic/splitter.h"
#include "Event/TouchDelegate.h"
#include "Framework/Application.h"
#include "Render/RenderManager.h"
#include "Render/RenderWindow.h"
#include "Scene/SceneManager.h"
#include "WxGLRenderWindow.h"
#include "Scene/Scene.h"

CGameViewAgent* CGameViewAgent::m_pInstance = nullptr;

CGameViewAgent::CGameViewAgent()
    : m_bStartTouch(false)
{

}

CGameViewAgent::~CGameViewAgent()
{

}

void CGameViewAgent::ProcessMouseEvent( wxMouseEvent& event )
{
    wxPoint MousePos = event.GetPosition();
    unsigned int uTapCount = 1;
    int id = 0;
    float fX = (float)MousePos.x;
    float fY = (float)MousePos.y;
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pRenderTarget != NULL);
    wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
    BEATS_ASSERT(pSceneRenderWnd != NULL);
    float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetWidth() : pSceneRenderWnd->GetScaleFactor();
    fX /= fScaleFactor;
    fY /= fScaleFactor;
    if ( event.ButtonDClick( wxMOUSE_BTN_LEFT ) )
    {
        uTapCount = 2;
        CTouchDelegate::GetInstance()->OnTouchBegan( 1, &id, &fX, &fY, &uTapCount );
        CTouchDelegate::GetInstance()->OnTapped( id, fX, fY, uTapCount);
    }
    else
    {
        if(event.LeftIsDown())
        {
            if (event.Dragging() && m_bStartTouch)
            {
                CTouchDelegate::GetInstance()->OnTouchMoved( 1, &id, &fX, &fY );
            }
            else
            {
                CTouchDelegate::GetInstance()->OnTouchBegan( 1, &id, &fX, &fY, &uTapCount );
                m_bStartTouch = true;
            }
        }
        else if(event.LeftUp())
        {
            CTouchDelegate::GetInstance()->OnTouchEnded( 1, &id, &fX, &fY, &uTapCount );
            m_bStartTouch = false;
        }
    }
    if (event.GetWheelRotation() != 0 )
    {
        CTouchDelegate::GetInstance()->OnPinched( EGestureState::eGS_BEGAN, 1.0f);
        CTouchDelegate::GetInstance()->OnPinched( EGestureState::eGS_CHANGED, event.GetWheelRotation() > 0 ? 1.5f : 0.5f);
    }
}

void CGameViewAgent::InView()
{
    wxAuiManager& manager = m_pMainFrame->m_Manager;
    m_pMainFrame->m_nSashPosition = m_pMainFrame->m_pSplitter->GetSashPosition();
    m_pMainFrame->m_pSplitter->Unsplit();
    manager.GetPane(m_pMainFrame->m_pRightPanel).Hide();
    manager.GetPane(m_pMainFrame->m_pLeftPanel).Hide();
    manager.GetPane(m_pMainFrame->m_pResourcePanel).Hide();
    manager.GetPane(m_pMainFrame->m_pToolPanel).Hide();
    manager.Update();
    CApplication::GetInstance()->Start();
    CScene* pCurScene = CSceneManager::GetInstance()->GetCurrentScene();
    if (pCurScene)
    {
        pCurScene->RestoreCamera();
    }
}

void CGameViewAgent::OutView()
{
    CApplication::GetInstance()->Stop();
    wxAuiManager& manager = m_pMainFrame->m_Manager;
    m_pMainFrame->m_nCursorIconID = wxCURSOR_ARROW;
    m_pMainFrame->m_pSplitter->SplitHorizontally((wxWindow*)m_pMainFrame->m_pViewScreen, (wxWindow*)m_pMainFrame->m_pComponentRenderWindow, m_pMainFrame->m_nSashPosition);
    manager.GetPane(m_pMainFrame->m_pRightPanel).Show();
    manager.GetPane(m_pMainFrame->m_pLeftPanel).Show();
    manager.GetPane(m_pMainFrame->m_pResourcePanel).Show();
    manager.GetPane(m_pMainFrame->m_pToolPanel).Show();
    manager.Update();
}
