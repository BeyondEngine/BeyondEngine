#include "stdafx.h"
#include "BatchEditWnd.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"

CBatchEditWnd::CBatchEditWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name/* = wxDialogNameStr*/)
    : super(parent, id, title, pos, size, style, name)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);
    wxBoxSizer* pChangeStyleSizer = new wxBoxSizer(wxHORIZONTAL);
    pChangeStyleSizer->Add(new wxStaticText(this, wxID_ANY, _T("修改范围")), 0, wxLEFT | wxRIGHT, 5);
    wxString strChangeStyleChoiceString[] = { wxT("当前文件"), wxT("已加载文件")};
    int uChoiceCount = sizeof( strChangeStyleChoiceString ) / sizeof( wxString );
    m_pOperatingRange = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, uChoiceCount, strChangeStyleChoiceString, 0 );
    pChangeStyleSizer->Add(m_pOperatingRange, 0,  wxLEFT | wxRIGHT, 5);
    pMainSizer->Add(pChangeStyleSizer, 0, wxALL, 5);

    wxBoxSizer* pGuidSizer = new wxBoxSizer(wxHORIZONTAL);
    pGuidSizer->Add(new wxStaticText(this, wxID_ANY, _T("GUID(16进制)")), 0,  wxLEFT | wxRIGHT, 5);
    m_pGuidTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxNumericPropertyValidator(wxNumericPropertyValidator::Unsigned, 16));
    pGuidSizer->Add(m_pGuidTextCtrl, 0, wxLEFT | wxRIGHT, 5);
    pMainSizer->Add(pGuidSizer, 0, wxALL, 5);

    wxBoxSizer* pPropertyNameSizer = new wxBoxSizer(wxHORIZONTAL);
    pPropertyNameSizer->Add(new wxStaticText(this, wxID_ANY, _T("属性代码名称")), 0,  wxLEFT | wxRIGHT, 5);
    m_pPropertyNameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    pPropertyNameSizer->Add(m_pPropertyNameCtrl, 0,  wxLEFT | wxRIGHT, 5);
    pMainSizer->Add(pPropertyNameSizer, 0, wxALL, 5);

    wxBoxSizer* pNewValueSizer = new wxBoxSizer(wxHORIZONTAL);
    pNewValueSizer->Add(new wxStaticText(this, wxID_ANY, _T("设定值")), 0,  wxLEFT | wxRIGHT, 5);
    m_pNewValueCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    pNewValueSizer->Add(m_pNewValueCtrl, 0, wxLEFT | wxRIGHT, 5);
    pMainSizer->Add(pNewValueSizer, 0, wxALL, 5);

    m_pApplyToDerivedCheckBox = new wxCheckBox(this, wxID_ANY, _T("应用到子类"));
    pMainSizer->Add(m_pApplyToDerivedCheckBox, 0, wxALL, 5);

    m_pApplyBtn = new wxButton (this, wxID_ANY, _T("确定"));
    pMainSizer->Add(m_pApplyBtn, 0, wxALL, 5);

    this->Fit();
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CBatchEditWnd::OnClose), NULL, this);
    this->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CBatchEditWnd::OnApplyBtnClicked), NULL, this);
}

CBatchEditWnd::~CBatchEditWnd()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CBatchEditWnd::OnClose), NULL, this);
    this->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CBatchEditWnd::OnApplyBtnClicked), NULL, this);
}

bool CBatchEditWnd::Show(bool bShow /*= true*/)
{
    super::Show(bShow);
    if (bShow)
    {
        m_pOperatingRange->SetSelection(0);
        m_pApplyToDerivedCheckBox->SetValue(true);
    }
    return true;
}

void CBatchEditWnd::OnClose(wxCloseEvent& /*event*/)
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_BatchEditBtn, false);
    pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    this->Show(false);
}

void CBatchEditWnd::OnApplyBtnClicked(wxCommandEvent& /*event*/)
{
    wxString strPropertyName = m_pPropertyNameCtrl->GetValue();
    if (strPropertyName.IsEmpty())
    {
        wxMessageBox(_T("property name 不能为空！"));
    }
    else
    {
        int nRet = wxMessageBox(wxString::Format(_T("新的值为 %s, 确定吗？"), m_pNewValueCtrl->GetValue()), _T("即将修改变量值"), wxYES_NO, this);
        if (nRet == wxYES)
        {
            unsigned long uGuidRef = 0;
            m_pGuidTextCtrl->GetValue().ToULong(&uGuidRef, 16);
            std::set<CComponentProxy*> operatingComponentProxy;
            std::set<size_t> derivedClassSet;
            bool bApplyToDerivedClass = m_pApplyToDerivedCheckBox->GetValue();
            if (bApplyToDerivedClass)
            {
                std::vector<size_t> derivedClassGuid;
                CComponentProxyManager::GetInstance()->QueryDerivedClass(uGuidRef, derivedClassGuid, true);
                for (size_t i = 0; i < derivedClassGuid.size(); ++i)
                {
                    BEATS_ASSERT(derivedClassSet.find(derivedClassGuid[i]) == derivedClassSet.end());
                    derivedClassSet.insert(derivedClassGuid[i]);
                }
            }
            if (m_pOperatingRange->GetSelection() == 0)
            {
                const std::map<size_t, CComponentProxy*>& curSceneComponentsMap = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
                for (auto iter = curSceneComponentsMap.begin(); iter != curSceneComponentsMap.end(); ++iter)
                {
                    size_t uGuid = iter->second->GetGuid();
                    if (uGuid == uGuidRef || (bApplyToDerivedClass && derivedClassSet.find(uGuid) != derivedClassSet.end()))
                    {
                        BEATS_ASSERT(operatingComponentProxy.find(iter->second) == operatingComponentProxy.end());
                        operatingComponentProxy.insert(iter->second);
                    }
                }
            }
            else
            {
                const std::map<size_t, std::map<size_t, CComponentBase*>*>* pComponentInstanceMap = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
                for (auto iter = pComponentInstanceMap->begin(); iter != pComponentInstanceMap->end(); ++iter)
                {
                    size_t uGuid = iter->first;
                    if (uGuid == uGuidRef || (bApplyToDerivedClass && derivedClassSet.find(uGuid) != derivedClassSet.end()))
                    {
                        for (auto subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
                        {
                            CComponentProxy* pProxy = dynamic_cast<CComponentProxy*>(subIter->second);
                            BEATS_ASSERT(pProxy != NULL);
                            BEATS_ASSERT(operatingComponentProxy.find(pProxy) == operatingComponentProxy.end());
                            operatingComponentProxy.insert(pProxy);
                        }
                    }
                }
            }
            for (auto iter = operatingComponentProxy.begin(); iter != operatingComponentProxy.end(); ++iter)
            {
                CPropertyDescriptionBase* pProperty = (*iter)->GetPropertyDescription(strPropertyName);
                pProperty->GetValueByTChar(m_pNewValueCtrl->GetValue(), pProperty->GetValue(eVT_CurrentValue));
                // Force update the host component, because we have set to the current value by GetValueByTChar.
                pProperty->SetValueWithType(pProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
            }
            wxMessageBox(_T("修改完成！"));
        }
    }
}
