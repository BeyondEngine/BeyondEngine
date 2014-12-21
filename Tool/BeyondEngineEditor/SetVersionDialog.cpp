#include "stdafx.h"
#include "SetVersionDialog.h"
#include "EditorConfig.h"

CSetVersionDialog::CSetVersionDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pAppVerSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pResVerSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pTypeSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pButtonVerSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* pAppInfoText = new wxStaticText(this, wxID_ANY, _T("AppVersion"));
    wxStaticText* pResInfoText = new wxStaticText(this, wxID_ANY, _T("ResVersion"));
    wxStaticText* pConvertTypeInfoText = new wxStaticText(this, wxID_ANY, _T("1: ETC 2: PVR"));
    m_pAppVerTextCtrl = new wxTextCtrl(this, wxID_ANY);
    m_pResVerTextCtrl = new wxTextCtrl(this, wxID_ANY);
    m_pConverPNGTypeTextCtrl = new wxTextCtrl(this, wxID_ANY);
    wxButton* pOkButton = new wxButton(this, wxID_ANY, _T("Ok"));
    wxButton* pCancelButton = new wxButton(this, wxID_ANY, _T("Cancel"));
    pAppVerSizer->Add(pAppInfoText, 1, wxALIGN_CENTRE | wxALL, 0);
    pAppVerSizer->Add(m_pAppVerTextCtrl, 1, wxALIGN_CENTRE | wxALL, 0);
    pResVerSizer->Add(pResInfoText, 1, wxALIGN_CENTRE | wxALL, 0);
    pResVerSizer->Add(m_pResVerTextCtrl, 1, wxALIGN_CENTRE | wxALL, 0);
    pTypeSizer->Add(pConvertTypeInfoText, 1, wxALIGN_CENTRE | wxALL, 0);
    pTypeSizer->Add(m_pConverPNGTypeTextCtrl, 1, wxALIGN_CENTRE | wxALL, 0);
    pButtonVerSizer->Add(pOkButton, 0, wxALIGN_CENTRE | wxALL, 0);
    pButtonVerSizer->Add(pCancelButton, 0, wxALIGN_CENTRE | wxALL, 0);
    pSizer->Add(pAppVerSizer, 1, wxALIGN_CENTRE | wxALL, 0);
    pSizer->Add(pResVerSizer, 1, wxALIGN_CENTRE | wxALL, 0);
    pSizer->Add(pTypeSizer, 1, wxALIGN_CENTRE | wxALL, 0);
    pSizer->Add(pButtonVerSizer, 1, wxALIGN_CENTRE | wxALL, 0);
    SetSizer(pSizer);
    pOkButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CSetVersionDialog::OnOk), NULL, this);
    pCancelButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CSetVersionDialog::OnCancel), NULL, this);
}

CSetVersionDialog::~CSetVersionDialog()
{

}

void CSetVersionDialog::OnOk(wxCommandEvent& /*event*/)
{
    EndModal(wxID_OK);
}

void CSetVersionDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

wxString CSetVersionDialog::GetAppVersion()
{
    return m_pAppVerTextCtrl->GetValue();
}

wxString CSetVersionDialog::GetResVersion()
{
    return m_pResVerTextCtrl->GetValue();
}

void CSetVersionDialog::SetAppVersion(wxString strAppVersion)
{
    m_pAppVerTextCtrl->SetValue(strAppVersion);
}

void CSetVersionDialog::SetResVersion(wxString strResVersion)
{
    m_pResVerTextCtrl->SetValue(strResVersion);
}

void CSetVersionDialog::SetConvertTypeText(wxString strType)
{
    m_pConverPNGTypeTextCtrl->SetValue(strType);
}

wxString CSetVersionDialog::GetConvertTypeText()
{
    return m_pConverPNGTypeTextCtrl->GetValue();
}
