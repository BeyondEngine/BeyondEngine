#include "stdafx.h"
#include "RandomPropertyDialog.h"
#include "wx/valnum.h"
#include "RandomValue.h"

CRandomPropertyDialog::CRandomPropertyDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
: CEditDialogBase(parent, id, title, pos, size, style, name)
, m_pTypeChoices(nullptr)
, m_pConstantTextCtrl(nullptr)
, m_pRandomMinTextCtrl(nullptr)
, m_pRandomMaxTextCtrl(nullptr)
, m_pRandomValue(nullptr)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    m_pTypeChoices = new wxChoice(this, wxID_ANY);
    m_pTypeChoices->AppendString(wxT("常量"));
    m_pTypeChoices->AppendString(wxT("曲线"));
    m_pTypeChoices->AppendString(wxT("随机常量"));
    m_pTypeChoices->AppendString(wxT("随机曲线"));

    m_pConstantTextCtrl = new wxTextCtrl(this, wxID_ANY);
    m_pConstantTextCtrl->SetValidator(wxFloatingPointValidator<float>());
    m_pRandomMinTextCtrl = new wxTextCtrl(this, wxID_ANY);
    m_pRandomMinTextCtrl->SetValidator(wxFloatingPointValidator<float>());
    m_pRandomMaxTextCtrl = new wxTextCtrl(this, wxID_ANY);
    m_pRandomMaxTextCtrl->SetValidator(wxFloatingPointValidator<float>());

    pSizer->Add(m_pTypeChoices, 0, wxGROW | wxALL, 0);
    pSizer->Add(m_pConstantTextCtrl, 0, wxALL, 2);
    pSizer->Add(m_pRandomMinTextCtrl, 0, wxALL, 2);
    pSizer->Add(m_pRandomMaxTextCtrl, 0,  wxALL, 2);

    SetSizer(pSizer);
    Fit();

    m_pTypeChoices->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CRandomPropertyDialog::OnTypeChoiceChanged), NULL, this);
}

CRandomPropertyDialog::~CRandomPropertyDialog()
{
    m_pTypeChoices->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CRandomPropertyDialog::OnTypeChoiceChanged), NULL, this);
}

void CRandomPropertyDialog::SetRandomValue(SRandomValue* pValue)
{
    m_pRandomValue = pValue;
    if (m_pRandomValue != nullptr)
    {
        switch (m_pRandomValue->GetType())
        {
        case ENewRandomValueType::eRVT_Constant:
            m_pConstantTextCtrl->SetValue(wxString::Format("%g", m_pRandomValue->GetFactor()));
            break;
        case ENewRandomValueType::eRVT_RandomConstant:
            m_pRandomMinTextCtrl->SetValue(wxString::Format("%g", m_pRandomValue->GetFactor() * m_pRandomValue->GetMinCurve().m_keyList.begin()->second.m_fValue));
            m_pRandomMaxTextCtrl->SetValue(wxString::Format("%g", m_pRandomValue->GetFactor() * m_pRandomValue->GetMaxCurve().m_keyList.begin()->second.m_fValue ));
            break;
        default:
            break;
        }
        m_pTypeChoices->Select((int)m_pRandomValue->GetType());
    }
}

int CRandomPropertyDialog::ShowModal()
{
    BEATS_ASSERT(m_pRandomValue != nullptr, "Must set a random value before show dialog.");
    UpdateDialog(m_pRandomValue->GetType());
    super::ShowModal();
    ENewRandomValueType type = (ENewRandomValueType)m_pTypeChoices->GetSelection();
    m_pRandomValue->SetType(type);
    switch (type)
    {
    case ENewRandomValueType::eRVT_Constant:
        m_pRandomValue->SetFactor(_tstof(m_pConstantTextCtrl->GetValue()));
        break;
    case ENewRandomValueType::eRVT_Curve:
        break;
    case ENewRandomValueType::eRVT_RandomConstant:
    {
        m_pRandomValue->SetFactor(1.0f);
        m_pRandomValue->GetMinCurve().m_keyList[0].m_fValue = _tstof(m_pRandomMinTextCtrl->GetValue());
        m_pRandomValue->GetMaxCurve().m_keyList[0].m_fValue = _tstof(m_pRandomMaxTextCtrl->GetValue());
    }
        break;
    case ENewRandomValueType::eRVT_RandomCurve:
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
    return 0;
}

void CRandomPropertyDialog::OnTypeChoiceChanged(wxCommandEvent& event)
{
    UpdateDialog((ENewRandomValueType)event.GetSelection());
}

void CRandomPropertyDialog::UpdateDialog(ENewRandomValueType type)
{
    m_pConstantTextCtrl->Show(type == ENewRandomValueType::eRVT_Constant);
    m_pRandomMinTextCtrl->Show(type == ENewRandomValueType::eRVT_RandomConstant);
    m_pRandomMaxTextCtrl->Show(type == ENewRandomValueType::eRVT_RandomConstant);
    Layout();
}