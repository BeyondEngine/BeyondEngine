#include "stdafx.h"
#include "GameViewAgent.h"
#include "EditorMainFrame.h"
#include "wx/generic/splitter.h"
#include "Event/TouchDelegate.h"
#include "Framework/Application.h"
#include "Render/RenderManager.h"
#include "Scene/SceneManager.h"
#include "WxGLRenderWindow.h"

CGameViewAgent* CGameViewAgent::m_pInstance = nullptr;

CGameViewAgent::CGameViewAgent()
{

}

CGameViewAgent::~CGameViewAgent()
{

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
}

void CGameViewAgent::OutView()
{
    wxAuiManager& manager = m_pMainFrame->m_Manager;
    m_pMainFrame->m_nCursorIconID = wxCURSOR_ARROW;
    m_pMainFrame->m_pSplitter->SplitHorizontally((wxWindow*)m_pMainFrame->m_pViewScreen, (wxWindow*)m_pMainFrame->m_pComponentRenderWindow, m_pMainFrame->m_nSashPosition);
    manager.GetPane(m_pMainFrame->m_pRightPanel).Show();
    manager.GetPane(m_pMainFrame->m_pLeftPanel).Show();
    manager.GetPane(m_pMainFrame->m_pResourcePanel).Show();
    manager.GetPane(m_pMainFrame->m_pToolPanel).Show();
    manager.Update();
}

void CGameViewAgent::ProcessKeyboardEvent(wxKeyEvent& event)
{
    super::ProcessKeyboardEvent(event);
    event.Skip();
}
