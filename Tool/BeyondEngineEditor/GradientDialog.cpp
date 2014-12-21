#include "stdafx.h"
#include "GradientDialog.h"
#include "GradientCtrl.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"

CGradientDialog::CGradientDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
: CEditDialogBase(parent, id, title, pos, size, style, name)
, m_pGradientCtrl(NULL)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pCtrlSizer = new wxBoxSizer(wxVERTICAL);
    m_pGradientCtrl = new CGradientCtrl(this, wxID_ANY);
    pCtrlSizer->Add(m_pGradientCtrl, 1, wxGROW | wxALL, 0);
    pSizer->Add(pCtrlSizer, 0, wxGROW | wxALL, 0);

    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* pYesBtn = new wxButton(this, wxID_ANY, _T("确定"));
    wxButton* pCancelBtn = new wxButton(this, wxID_ANY, _T("取消"));
    pButtonSizer->AddStretchSpacer(1);
    pButtonSizer->Add(pYesBtn, 0, wxALL, 5);
    pButtonSizer->Add(pCancelBtn, 0, wxALL, 5);
    pSizer->Add(pButtonSizer, 0, wxGROW | wxRIGHT, 0);

    SetSizer(pSizer);
    Fit();
    pYesBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CGradientDialog::OnYesBtnClicked), NULL, this);
    pCancelBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CGradientDialog::OnCancelBtnClicked), NULL, this);
}

CGradientDialog::~CGradientDialog()
{
}

int CGradientDialog::ShowModal()
{
    m_allCursorPosList.clear();
    int nRet = INVALID_DATA;
    nRet= wxDialog::ShowModal();
    if (nRet == wxYES)
    {
        m_allCursorPosList.push_back(0.0f);
        m_allCursorPosList.push_back(1.0f);

        auto colorList = m_pGradientCtrl->GetColorList();
        for (auto itr : colorList)
        {
            float fPos = itr->GetPosPercent();
            if (CheckPosIsOnly(fPos))
            {
                m_allCursorPosList.push_back(fPos);
            }
        }

        auto alphaList = m_pGradientCtrl->GetAlphaList();
        for (auto itr : alphaList)
        {
            float fPos = itr->GetPosPercent();
            if (CheckPosIsOnly(fPos))
            {
                m_allCursorPosList.push_back(fPos);
            }
        }
    }

    return nRet;
}

bool CGradientDialog::CheckPosIsOnly(float fPos)
{
    bool bRet = true;
    for (auto itr : m_allCursorPosList)
    {
        if (itr == fPos)
        {
            bRet = false;
            break;
        }
    }
    return bRet;
}

int CGradientDialog::GetPosCount()
{
    return m_allCursorPosList.size();
}

float CGradientDialog::GetPosByIndex(int nIndex)
{
    BEATS_ASSERT(nIndex < (int)m_allCursorPosList.size());
    return m_allCursorPosList[nIndex];
}

wxColor CGradientDialog::GetColorByIndex(int nIndex)
{
    BEATS_ASSERT(nIndex < (int)m_allCursorPosList.size());
    return m_pGradientCtrl->GetColorByPos(m_allCursorPosList[nIndex]);
}

CGradientCtrl* CGradientDialog::GetGradientCtrl() const
{
    return m_pGradientCtrl;
}

void CGradientDialog::Reset()
{
    m_allCursorPosList.clear();
    if (m_pGradientCtrl != NULL)
    {
        m_pGradientCtrl->Reset();
    }
}

void CGradientDialog::OnYesBtnClicked(wxCommandEvent& /*event*/)
{
    EndModal(wxYES);
}

void CGradientDialog::OnCancelBtnClicked(wxCommandEvent& /*event*/)
{
    EndModal(wxCANCEL);
}
