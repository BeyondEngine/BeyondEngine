#include "stdafx.h"
#include "SplineDialog.h"
#include "SplineGLWindow.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"


BEGIN_EVENT_TABLE(CSplineDialog, CEditDialogBase)
END_EVENT_TABLE()

CSplineDialog::CSplineDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_pSplineGLWindow(NULL)
{
    InitCtrls();
}

CSplineDialog::~CSplineDialog()
{
}


void CSplineDialog::InitCtrls()
{
    CEngineEditor* pEditer = static_cast<CEngineEditor*>(wxApp::GetInstance());
    wxGLContext* pContext = pEditer->GetGLContext();
    m_pSplineGLWindow = new CSplineGLWindow(this, pContext);
    pEditer->GetMainFrame()->RegisterUpdateWindow(m_pSplineGLWindow);
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);
    pSizer->Add(m_pSplineGLWindow, 1, wxGROW|wxALL, 0);
}

int CSplineDialog::ShowModal()
{
    return wxDialog::ShowModal();
}