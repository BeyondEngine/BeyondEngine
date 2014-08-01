#include "stdafx.h"
#include "ViewAgentBase.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "BeyondEngineEditorGLWindow.h"
#include "BeyondEngineEditorComponentWindow.h"

CViewAgentBase::CViewAgentBase()
{
    m_pMainFrame = ((CEngineEditor*)wxApp::GetInstance())->GetMainFrame();
}

CViewAgentBase::~CViewAgentBase()
{

}

void CViewAgentBase::ProcessMouseEvent( wxMouseEvent& event )
{
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        wxWindow* pWindow = (wxWindow*)m_pMainFrame->m_pViewScreen;
        if (!pWindow->HasCapture())
        {
            pWindow->CaptureMouse();
        }
    }
    else if(event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        wxWindow* pWindow = (wxWindow*)m_pMainFrame->m_pViewScreen;
        if (pWindow->HasCapture())
        {
            pWindow->ReleaseMouse();
        }
    }
}

void CViewAgentBase::InView()
{

}

void CViewAgentBase::OutView()
{

}

void CViewAgentBase::CreateTools()
{

}

void CViewAgentBase::SelectComponent( CComponentProxy* /*pComponentProxy*/ )
{

}

void CViewAgentBase::OnOpenComponentFile()
{

}

void CViewAgentBase::OnTreeCtrlSelect(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnActivateTreeItem(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnCloseComponentFile()
{

}

void CViewAgentBase::OnUpdateComponentInstance()
{

}

void CViewAgentBase::OnPropertyChanged(wxPropertyGridEvent& /*event*/)
{

}

void CViewAgentBase::OnTreeCtrlRightClick(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnEditTreeItemLabel(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnEditDataViewItem(wxDataViewEvent& /*event*/)
{

}

void CViewAgentBase::OnCommandEvent(wxCommandEvent& /*event*/)
{

}

void CViewAgentBase::Update()
{

}
