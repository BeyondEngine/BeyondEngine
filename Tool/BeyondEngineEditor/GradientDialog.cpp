#include "stdafx.h"
#include "GradientDialog.h"
#include "GradientCtrl.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"

BEGIN_EVENT_TABLE(CGradientDialog, CEditDialogBase)

END_EVENT_TABLE()

    CGradientDialog::CGradientDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    m_pGradientCtrl = new CGradientCtrl(this, wxID_ANY);
    pSizer->Add(m_pGradientCtrl, 1, wxGROW|wxALL, 0);
    SetSizer(pSizer);
    InitCtrls();
}

CGradientDialog::~CGradientDialog()
{
}


void CGradientDialog::InitCtrls()
{

}

int CGradientDialog::ShowModal()
{
    m_allCursorPosList.clear();
    int nRet = INVALID_DATA;
    nRet= wxDialog::ShowModal();

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

    auto maskList = m_pGradientCtrl->GetMaskList();
    for (auto itr : maskList)
    {
        float fPos = itr->GetPosPercent();
        if (CheckPosIsOnly(fPos))
        {
            m_allCursorPosList.push_back(fPos);
        }
    }
    return wxID_OK;
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
