#include "stdafx.h"
#include "AboutDlg.h"
#include "EditorConfig.h"

CAboutDlg::CAboutDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pBackgroundPanel = new wxPanel(this);
    m_pTextCtrl = new wxTextCtrl(m_pBackgroundPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH2);
    pMainSizer->Add(m_pTextCtrl, 1, wxEXPAND, 1);
    m_pShowAfterLaunchCheckBox = new wxCheckBox(m_pBackgroundPanel, wxID_ANY, _T("Æô¶¯Ê±ÏÔÊ¾"));
    pMainSizer->Add(m_pShowAfterLaunchCheckBox, 0, wxALL, 10);
    m_pBackgroundPanel->SetSizer(pMainSizer);

    m_pShowAfterLaunchCheckBox->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(CAboutDlg::OnShowAfterLaunchCheckBoxClick), NULL, this);
}

CAboutDlg::~CAboutDlg()
{

}

int CAboutDlg::ShowModal()
{
    m_pShowAfterLaunchCheckBox->SetValue(CEditorConfig::GetInstance()->IsShowAboutDlgAfterLaunch());
    return super::ShowModal();
}

void CAboutDlg::OnShowAfterLaunchCheckBoxClick(wxCommandEvent& event)
{
    CEditorConfig::GetInstance()->SetShowAboutDlgAfterlaunch(event.GetInt() != 0);
}