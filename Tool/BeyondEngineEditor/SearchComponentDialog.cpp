#include "stdafx.h"
#include "SearchComponentDialog.h"
#include "Component\Component\ComponentProject.h"
#include "Component\Component\ComponentProxyManager.h"
#include "Component\Component\ComponentProxy.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"

wxBEGIN_EVENT_TABLE(CSearchComponentDialog, wxDialog)
wxEND_EVENT_TABLE()

CSearchComponentDialog::CSearchComponentDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos /*= wxDefaultPosition */, const wxSize &size /*= wxDefaultSize */, long style /*= wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION */, const wxString &name /*= wxDialogNameStr*/)
    :wxDialog(parent, id, title, pos, size, style, name)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);

    wxBoxSizer* pSearchSizer = new wxBoxSizer(wxHORIZONTAL);
    wxString strSearchChoiceString[] = {wxT("ID"), wxT("名字(已加载)")};
    int uChoiceCount = sizeof(strSearchChoiceString) / sizeof(wxString);
    m_pSearchViewChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, uChoiceCount, strSearchChoiceString, 0);
    pSearchSizer->Add(m_pSearchViewChoice, 0, wxALL | wxALIGN_LEFT, 5);
    m_pSearchViewChoice->SetSelection(0);

    m_pInputText = new wxTextCtrl(this, ID_TextCtrl_Input, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    m_pInputText->SetFocus();
    pSearchSizer->Add(m_pInputText, 0, wxALL | wxALIGN_LEFT, 5);
    pMainSizer->Add(pSearchSizer, 0, wxALL, 0);

    m_pSearchBtn = new wxButton(this, wxID_ANY, wxT("搜索"));
    pMainSizer->Add(m_pSearchBtn, 0, wxALL | wxALIGN_CENTRE, 5);

    m_pComponentListBox = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxSize(230, 300));
    pMainSizer->Add(m_pComponentListBox, 0, wxALL | wxALIGN_CENTRE, 5);

    m_pInputText->Connect(wxEVT_KEY_UP, wxKeyEventHandler(CSearchComponentDialog::OnKeyClicked), NULL, this);
    m_pSearchViewChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CSearchComponentDialog::OnViewChoiceChanged), NULL, this);
    m_pSearchBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CSearchComponentDialog::OnSearchBtnClicked), NULL, this);
    m_pComponentListBox->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxListEventHandler(CSearchComponentDialog::OnFileFilterSelect), NULL, this);
}

CSearchComponentDialog::~CSearchComponentDialog()
{
    m_pInputText->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(CSearchComponentDialog::OnKeyClicked), NULL, this);
    m_pSearchViewChoice->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CSearchComponentDialog::OnViewChoiceChanged), NULL, this);
    m_pSearchBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CSearchComponentDialog::OnSearchBtnClicked), NULL, this);
    m_pComponentListBox->Disconnect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxListEventHandler(CSearchComponentDialog::OnFileFilterSelect), NULL, this);
}

void CSearchComponentDialog::OnViewChoiceChanged(wxCommandEvent& event)
{
    if (m_nCurrentChoice != event.GetSelection())
    {
        m_nCurrentChoice = event.GetSelection();
    }
}

void CSearchComponentDialog::OnSearchBtnClicked(wxCommandEvent& /*event*/)
{
    SearchComponent();
}

void CSearchComponentDialog::OnFileFilterSelect(wxListEvent& /*event*/)
{
    wxString strSelection = m_pComponentListBox->GetStringSelection();
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pMainFrame->JumpToComponent(m_componentIdMap[strSelection]);
}

void CSearchComponentDialog::SearchComponent()
{
    m_pComponentListBox->Clear();
    m_componentIdMap.clear();
    wxString strInput = m_pInputText->GetValue().Lower();
    switch (m_nCurrentChoice)
    {
    case 0:
    {
        int iComponentId = atoi(strInput);
        if (iComponentId != 0xFFFFFFFF)
        {
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
            uint32_t uFileId = pProject->QueryFileId(iComponentId, false);
            if (uFileId != 0xFFFFFFFF)
            {
                CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
                pMainFrame->JumpToComponent(iComponentId);
                wxString strComponentId = wxString::Format(_T("%d"), iComponentId);
                m_componentIdMap[strComponentId] = iComponentId;
                m_pComponentListBox->AppendString(strComponentId);
            }
            else
            {
                TCHAR szInfo[1024];
                _stprintf(szInfo, _T("未能找到ID为%d的组件."), iComponentId);
                MessageBox(BEYONDENGINE_HWND, szInfo, _T("搜索结果"), MB_OK);
            }
        }
    }
    break;
    case 1:
    {
        const std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>* pComponentMap = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
        for (auto iter = pComponentMap->begin(); iter != pComponentMap->end(); ++iter)
        {
            for (auto subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
            {
                CComponentProxy* pComponentProxy = down_cast<CComponentProxy*>(subIter->second);
                const TString& strDisplayName = pComponentProxy->GetUserDefineDisplayName().empty() ? pComponentProxy->GetDisplayName() : pComponentProxy->GetUserDefineDisplayName();
                wxString strName = wxString::FromUTF8(strDisplayName.c_str());
                bool bMatch = strName.Lower().find(strInput.c_str()) != -1;
                if (bMatch)
                {
                    m_componentIdMap[strName] = subIter->first;
                    m_pComponentListBox->AppendString(strName);
                }
            }
        }
    }
    break;
    default:
        break;
    }
}

void CSearchComponentDialog::OnKeyClicked(wxKeyEvent& event)
{
    if (event.GetKeyCode() == 13 || event.GetKeyCode() == 370)
    {
        SearchComponent();
    }
}
