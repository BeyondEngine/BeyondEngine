#include "stdafx.h"
#include "CustomResolutionDialog.h"
#include "EditorConfig.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "SelectResolutionDialog.h"
#include "wx/generic/numdlgg.h"

CCustomResolutionDialog::CCustomResolutionDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_pResolutionListBox(NULL)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pBottomSizer = new wxBoxSizer(wxHORIZONTAL);

    pSizer->Add(pTopSizer, 1, wxGROW | wxALL, 5);
    pSizer->Add(pBottomSizer, 0, wxALIGN_CENTER | wxALL, 5);

    m_pResolutionListBox = new wxListBox(this, wxID_ANY);
    pTopSizer->Add(m_pResolutionListBox, 1, wxGROW | wxALL, 5);

    wxButton* pButtonAdd  = new wxButton(this, wxID_ANY, _T("Add"));
    pBottomSizer->Add(pButtonAdd, 0, wxALL, 5);
    wxButton* pButtonDelete = new wxButton(this, wxID_ANY, _T("Delete"));
    pBottomSizer->Add(pButtonDelete, 0, wxALL, 5);

    InitResolutonListBox();
    pButtonAdd->Connect(wxEVT_BUTTON, wxCommandEventHandler(CCustomResolutionDialog::OnButtonAdd), NULL, this);
    pButtonDelete->Connect(wxEVT_BUTTON, wxCommandEventHandler(CCustomResolutionDialog::OnButtonDelete), NULL, this);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CCustomResolutionDialog::OnClose), NULL, this);

}

CCustomResolutionDialog::~CCustomResolutionDialog()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CCustomResolutionDialog::OnClose), NULL, this);
}

void CCustomResolutionDialog::InitResolutonListBox()
{
    for (auto iter : CEditorConfig::GetInstance()->GetResolution())
    {
        m_pResolutionListBox->Append(wxString::Format(_T("%d"), uint32_t(iter.X())) + _T("x") + wxString::Format(_T("%d"), uint32_t(iter.Y())));
    }
}

void CCustomResolutionDialog::OnButtonAdd(wxCommandEvent& /*event*/)
{
    uint32_t uWidth = wxGetNumberFromUser(_T("Input simulate resolution width"), _T("Width:"), _T("Simulate resolution"), 0, 400, 10000);
    if (uWidth != 0xFFFFFFFF)
    {
        uint32_t uHeight = wxGetNumberFromUser(_T("Input simulate resolution height"), _T("Height:"), _T("Simulate resolution"), 0, 300, 10000);
        if (uHeight != 0xFFFFFFFF)
        {
            bool bExists = false;
            for (auto iter : CEditorConfig::GetInstance()->GetResolution())
            {
                if ((uint32_t)iter.X() == uWidth && (uint32_t)iter.Y() == uHeight)
                {
                    bExists = true;
                    break;
                }
            }
            if (!bExists)
            {
                m_pResolutionListBox->Append(wxString::Format(_T("%dx%d"), uWidth, uHeight));
            }
        }
    }
}

void CCustomResolutionDialog::OnButtonDelete(wxCommandEvent& /*event*/)
{
    wxArrayInt selections;
    int n = m_pResolutionListBox->GetSelections(selections);
    if (n == 1)
    {
        m_pResolutionListBox->Delete(selections[0]);
    }
}

void CCustomResolutionDialog::OnClose(wxCloseEvent& /*event*/)
{
    std::vector<CVec2> resolutionVector;
    for (auto i = 0U; i < m_pResolutionListBox->GetCount(); ++i)
    {
        TString strResolution = m_pResolutionListBox->GetString(i);
        std::vector<std::string> strArray;
        CStringHelper::GetInstance()->SplitString(strResolution.c_str(), _T("x"), strArray);
        if (!strArray[0].empty() && !strArray[1].empty())
        {
            resolutionVector.push_back(CVec2(atoi(strArray[0].c_str()), atoi(strArray[1].c_str())));
        }
    }
    CEditorConfig::GetInstance()->SetResolution(resolutionVector);
    EndModal(0);
}
