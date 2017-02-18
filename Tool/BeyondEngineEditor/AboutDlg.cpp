#include "stdafx.h"
#include "AboutDlg.h"
#include "EditorConfig.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Resource/ResourceManager.h"

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
    TString strReadmePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    strReadmePath.append(_T("/../ReadMe.txt"));
    int nRet = 0;
    if (CFilePathTool::GetInstance()->Exists(strReadmePath.c_str()))
    {
        CSerializer serializer(strReadmePath.c_str());
        serializer << 0;
        m_pTextCtrl->SetLabelText((char*)serializer.GetReadPtr());
        m_pShowAfterLaunchCheckBox->SetValue(CEditorConfig::GetInstance()->IsShowAboutDlgAfterLaunch());
        nRet = super::ShowModal();
    }
    return nRet;
}

void CAboutDlg::OnShowAfterLaunchCheckBoxClick(wxCommandEvent& event)
{
    CEditorConfig::GetInstance()->SetShowAboutDlgAfterlaunch(event.GetInt() != 0);
}