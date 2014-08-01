#include "stdafx.h"
#include "EditLanguageDialog.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include <wx/srchctrl.h>
#include "Resource/ResourcePathManager.h"
#include "Utf8String.h"

BEGIN_EVENT_TABLE(CEditLanguageDialog, CEditDialogBase)
    EVT_BUTTON(ID_BUTTON_ADD, CEditLanguageDialog::OnButtonAdd)
    EVT_BUTTON(ID_BUTTON_EXPORT, CEditLanguageDialog::OnButtonExport)
    EVT_BUTTON(ID_BUTTON_CLEAR, CEditLanguageDialog::OnButtonClear)
    EVT_SEARCHCTRL_SEARCH_BTN(ID_SERACH_LANGUAGE, CEditLanguageDialog::OnSearchLanguage)
    EVT_TEXT_ENTER(ID_SERACH_LANGUAGE,CEditLanguageDialog::OnSearchEnterLanguange)
    EVT_SEARCHCTRL_CANCEL_BTN(ID_SERACH_LANGUAGE, CEditLanguageDialog::OnCancleSrch)
    EVT_TEXT(ID_SERACH_LANGUAGE, CEditLanguageDialog::OnSrchUpdate)
    EVT_TEXT(ID_TEXT_ENUM, CEditLanguageDialog::OnEnumTextUpdate)
    EVT_IDLE(CEditLanguageDialog::OnSrchIdle)
    EVT_DATAVIEW_SELECTION_CHANGED(ID_DATAVIEW_LANGUAGE, CEditLanguageDialog::OnSelectDataView)
    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ID_DATAVIEW_LANGUAGE, CEditLanguageDialog::OnContextMenu)
    EVT_MENU(wxID_ANY, CEditLanguageDialog::OnMenuEvent)
END_EVENT_TABLE()

CEditLanguageDialog::CEditLanguageDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_selectRow(-1)
    , m_uLastEnumSearchTextUpdateTime(0)
    , m_bEnumSearchTextUpdate(false)
    , m_strChangeEnum(_T(""))
    , m_tmpBeforeStr(_T(""))
    , m_bAddSaveSuc(false)
    , m_bIsChange(false)
    , m_bISLanguageSwitch(false)
    , m_bIsSave(false)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pBottomSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pLeftChildeSizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pLeftChildeSizer2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pLeftChildeSizer3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pLeftChildeSizer4 = new wxBoxSizer(wxHORIZONTAL);

    m_pDataViewListLanguage = new wxDataViewListCtrl(this, ID_DATAVIEW_LANGUAGE);
    m_pButtonClear = new wxButton(this, ID_BUTTON_CLEAR, _T("Clear"), wxDefaultPosition,wxSize(60,30));
    m_pButtonAdd = new wxButton(this, ID_BUTTON_ADD, _T("Add"), wxDefaultPosition,wxSize(60,30));
    m_pButtonExport = new wxButton(this, ID_BUTTON_EXPORT, _T("Export"), wxDefaultPosition,wxSize(60,30));
    m_pTextEnum = new wxTextCtrl(this, ID_TEXT_ENUM, _T("eL_"));
    m_pTextChinese = new wxTextCtrl(this, ID_TEXT_CHINESE);
    m_pTextEnglish = new wxTextCtrl(this, ID_TEXT_ENGLISH);
    m_pStaticTextEnum = new wxStaticText(this, wxID_ANY, _T("  Enum "));
    m_pStaticTextChinese = new wxStaticText(this, wxID_ANY, _T("Chinese"));
    m_pStaticTextEnglish = new wxStaticText(this, wxID_ANY, _T(" English"));
    m_pSrchCtrl = new wxSearchCtrl(this, ID_SERACH_LANGUAGE, _T(""),wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_pSrchText = new wxStaticText(this, wxID_ANY, _T("枚举搜索："));

    pSizer->Add(pTopSizer, 0.5, wxGROW|wxALL, 5);
    pSizer->Add(pBottomSizer, 1, wxGROW|wxALL, 5);

    pTopSizer->Add(pLeftChildeSizer1, 1, wxGROW|wxALL, 20);
    pTopSizer->Add(pLeftChildeSizer2, 1, wxGROW|wxALL, 20);
    pTopSizer->Add(pLeftChildeSizer3, 1, wxGROW|wxALL, 20);
    pTopSizer->Add(pLeftChildeSizer4, 1, wxGROW|wxALL, 10);

    pLeftChildeSizer1->Add(m_pStaticTextEnum, 0, wxGROW|wxALL, 5);
    pLeftChildeSizer1->Add(m_pTextEnum, 1, wxGROW|wxALL, 5);
    pLeftChildeSizer2->Add(m_pStaticTextChinese, 0, wxGROW|wxALL, 5);
    pLeftChildeSizer2->Add(m_pTextChinese, 1, wxGROW|wxALL, 5);
    pLeftChildeSizer3->Add(m_pStaticTextEnglish, 0, wxGROW|wxALL, 5);
    pLeftChildeSizer3->Add(m_pTextEnglish, 1, wxGROW|wxALL, 5);
    pLeftChildeSizer4->AddStretchSpacer(1);
    pLeftChildeSizer4->Add(m_pButtonClear, 0, wxGROW|wxALL, 10);
    pLeftChildeSizer4->Add(m_pButtonAdd, 0, wxGROW|wxALL, 10);
    pLeftChildeSizer4->Add(m_pButtonExport, 0, wxGROW|wxALL, 10);
    pLeftChildeSizer4->AddStretchSpacer(1);

    pBottomSizer->Add(m_pDataViewListLanguage, 1, wxGROW|wxALL, 5);
    pBottomSizer->Add(m_pSrchText, 0, wxGROW|wxALL, 5);
    pBottomSizer->Add(m_pSrchCtrl, 0, wxGROW|wxALL, 5);

    m_pSrchCtrl->ShowCancelButton(true);
    InitLanguageMap();
    InitDataViewListCtrl();
    SetTextCtrlEnum();
}

CEditLanguageDialog::~CEditLanguageDialog()
{

}

void CEditLanguageDialog::InitLanguageMap()
{
    TString filePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Language);
    filePath.append(_T("/Language.bin"));
    int count = 0;
    CSerializer tmp(filePath.c_str());
    tmp >> count;
    for (int i = 0; i < count; i++)
    {
        TString tmpstrEnum;
        tmp >> tmpstrEnum;
        std::string tmpstrChinese;
        tmp >> tmpstrChinese;
        std::string tmpstrEnglish;
        tmp >> tmpstrEnglish;
        std::vector<TString> mLanguageVector;
        mLanguageVector.push_back(Utf8ToTString(tmpstrChinese));
        mLanguageVector.push_back(Utf8ToTString(tmpstrEnglish));
        m_languageMap.emplace(tmpstrEnum, std::move(mLanguageVector));
    }
}

void CEditLanguageDialog::InitDataViewListCtrl()
{
    m_pDataViewListLanguage->AppendTextColumn("枚举", wxDATAVIEW_CELL_INERT, 130);
    m_pDataViewListLanguage->AppendTextColumn("中文", wxDATAVIEW_CELL_INERT, 130);
    m_pDataViewListLanguage->AppendTextColumn("英文", wxDATAVIEW_CELL_INERT, 130);
    AppendDataViewListCtrl();
}

void CEditLanguageDialog::OnButtonAdd( wxCommandEvent& /*event*/ )
{
    if (m_pButtonAdd->GetLabel() == _T("Add") ||
        m_pButtonAdd->GetLabel() == L10N_T(eL_Add))
    {
        AddSaveButtonPrompt();
        if (m_bAddSaveSuc)
        {
            AddLanguage();
            AppendDataViewListCtrl();
            ClearTextCtrl();
            m_pSrchCtrl->SetValue(_T(""));
        }
        m_bAddSaveSuc = false;
    }
    else if (m_pButtonAdd->GetLabel() == _T("Save") ||
        m_pButtonAdd->GetLabel() == L10N_T(eL_Save))
    {
        AddSaveButtonPrompt();
        if (m_bAddSaveSuc)
        {
            m_pButtonAdd->SetLabel(m_bISLanguageSwitch ? L10N_T(eL_Add) : _T("Add"));
            DleLanguage();
            AddLanguage();
            AppendDataViewListCtrl();
            ClearTextCtrl();
            m_pSrchCtrl->SetValue(_T(""));
        }
        m_bAddSaveSuc = false;
        m_bIsChange = false;
        m_bIsSave = false;
    }
}

void CEditLanguageDialog::OnButtonExport( wxCommandEvent& /*event*/ )
{
    // 1. Export Bin File.
    bool bExportBin = false;
    int count = m_languageMap.size();
    CSerializer tmp;
    tmp << count;
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        tmp << iter->first;
        tmp << TStringToUtf8(iter->second[0]);
        tmp << TStringToUtf8(iter->second[1]);
    }
    TString FilePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Language) + _T("/Language.bin");
    bExportBin = tmp.Deserialize(FilePath.c_str());

    //2. Export enum file.
    bool bExporthead;
    std::string content;
    content.append("#ifndef BEYONDENGINEEDITOR_LANGUAGEENUM_H__INCLUDE\n#define BEYONDENGINEEDITOR_LANGUAGEENUM_H__INCLUDE\n").append("\nenum EText\n{\n");
    char szBuffer[128];
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        CStringHelper::GetInstance()->ConvertToCHAR(iter->first.c_str(), szBuffer, 128);
        content.append("    ").append(szBuffer).append(",\n");
    }
    content.append("\n    eL_Count,\n").append("    eL_Force32Bit = 0xFFFFFFFF\n};\n");
    content.append("\n#endif");
    tmp.Reset();
    tmp << content;
    FilePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_SourceCode) + _T("/Language.h");
    bExporthead = tmp.Deserialize(FilePath.c_str(), _T("wt+"));

    //Export hint.
    TString strMessage;
    if (bExportBin && bExporthead)
    {
        strMessage = _T("Export Bin File and enum file are succeed.");
    }
    else
    {
        strMessage = _T("Export Bin File or enum file are failed.");
    }
    wxMessageDialog* pMessageDlg = new wxMessageDialog(this, strMessage, _T("Edit Language"), wxOK|wxCENTRE|wxICON_NONE);
    pMessageDlg->ShowModal();
}

void CEditLanguageDialog::OnSearchLanguage( wxCommandEvent& event )
{
    TString strEnum = event.GetString();
    SearchEnum(strEnum);
}

void CEditLanguageDialog::OnSearchEnterLanguange( wxCommandEvent& event )
{
    TString strEnum = event.GetString();
    SearchEnum(strEnum);
}

void CEditLanguageDialog::AppendDataViewListCtrl()
{
    m_pDataViewListLanguage->DeleteAllItems();
    wxVector<wxVariant> data;
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        data.clear();
        data.push_back( iter->first );
        data.push_back( iter->second[0] );
        data.push_back( iter->second[1] );

        m_pDataViewListLanguage->AppendItem( data );
    }
}

void CEditLanguageDialog::OnButtonClear( wxCommandEvent& /*event*/ )
{
    m_bIsSave = false;
    ClearTextCtrl();
    m_pButtonAdd->SetLabel(m_bISLanguageSwitch ? L10N_T(eL_Add) : _T("Add"));
}

void CEditLanguageDialog::OnSelectDataView( wxDataViewEvent& /*event*/ )
{
    m_selectRow = m_pDataViewListLanguage->GetSelectedRow();
    if (m_selectRow == -1)
    {
        return;
    }
    wxString strEnum = m_pDataViewListLanguage->GetTextValue(m_selectRow, 0);
    m_strChangeEnum = strEnum;
}

void CEditLanguageDialog::ClearTextCtrl()
{
    m_pTextEnum->SetValue(_T("eL_"));
    m_pTextChinese->Clear();
    m_pTextEnglish->Clear();
}

void CEditLanguageDialog::OnCancleSrch( wxCommandEvent& /*event*/ )
{
    AppendDataViewListCtrl();
}

void CEditLanguageDialog::SearchEnum( TString str )
{
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        if (iter->first == str)
        {
            m_pDataViewListLanguage->DeleteAllItems();
            wxVector<wxVariant> data;
            data.push_back( iter->first );
            data.push_back( iter->second[0] );
            data.push_back( iter->second[1] );
            m_pDataViewListLanguage->AppendItem( data );
        }
    }
}

void CEditLanguageDialog::OnSrchUpdate( wxCommandEvent& /*event*/ )
{
    m_uLastEnumSearchTextUpdateTime = GetTickCount();
    m_bEnumSearchTextUpdate = true;
}

void CEditLanguageDialog::OnEnumTextUpdate( wxCommandEvent& /*event*/ )
{
    wxString tmpNowStr;
    tmpNowStr = m_pTextEnum->GetValue();
    size_t uStrLen = _tcslen(_T("eL_"));
    if (tmpNowStr.Left(uStrLen) != wxT("eL_"))
    {
         wxString tmpStr = _T("eL_");
         if (tmpNowStr.size() < uStrLen)
         {
             m_pTextEnum->SetValue(tmpStr);
         }
         else if (tmpNowStr.AfterFirst(wxUniChar('_')) != wxT(""))
         {
             m_pTextEnum->SetValue(tmpStr + tmpNowStr.AfterFirst(wxUniChar('_')));
         }
         else
         {
             m_pTextEnum->SetValue(m_tmpBeforeStr);
         }
         m_pTextEnum->SetInsertionPointEnd();
    }
    m_tmpBeforeStr = m_pTextEnum->GetValue();
}

void CEditLanguageDialog::OnSrchIdle( wxIdleEvent& /*event*/ )
{
    if (m_bEnumSearchTextUpdate && GetTickCount() - m_uLastEnumSearchTextUpdateTime > 700)
    {
        m_bEnumSearchTextUpdate = false;
        wxString szText = m_pSrchCtrl->GetValue();
        m_pDataViewListLanguage->DeleteAllItems();
        for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); ++iter)
        {
            wxString lableText = iter->first;
            bool bMatch = lableText.Find(szText) != -1;
            if (bMatch)
            {
                wxVector<wxVariant> data;
                data.push_back( iter->first );
                data.push_back( iter->second[0] );
                data.push_back( iter->second[1] );
                m_pDataViewListLanguage->AppendItem( data );
            }
        }
    }
}

void CEditLanguageDialog::OnContextMenu( wxDataViewEvent& /*event*/ )
{
    wxMenu menu;
    menu.Append( ID_POPMENU_CHANGE, m_bISLanguageSwitch ? L10N_T(eL_Change) : _T("Change"));
    menu.Append( ID_POPMENU_DLE, m_bISLanguageSwitch ? L10N_T(eL_Delete) : _T("Delete"));
    m_pDataViewListLanguage->PopupMenu(&menu);
}

void CEditLanguageDialog::OnMenuEvent( wxCommandEvent& event )
{
    int id = event.GetId();
    if (id == ID_POPMENU_CHANGE)
    {
        if (m_selectRow != -1)
        {
            m_pButtonAdd->SetLabel(m_bISLanguageSwitch ? L10N_T(eL_Save) : _T("Save"));
            ChangeLanguage();
            m_bIsChange = true;
            m_bIsSave = true;
        }
    }
    else if (id == ID_POPMENU_DLE)
    {
        DleLanguage();
        AppendDataViewListCtrl();
    }
}

void CEditLanguageDialog::ChangeLanguage()
{
    TString strDataviewEnum = m_pDataViewListLanguage->GetTextValue(m_selectRow, 0);
    if (m_languageMap.find(strDataviewEnum) != m_languageMap.end())
    {
         m_pTextEnum->SetValue(m_languageMap.find(strDataviewEnum)->first);
         m_pTextChinese->SetValue(m_languageMap.find(strDataviewEnum)->second[0]);
         m_pTextEnglish->SetValue(m_languageMap.find(strDataviewEnum)->second[1]);
    }
}

void CEditLanguageDialog::AddLanguage()
{
    TString strInputEnum = m_pTextEnum->GetValue();
    TString strInputChinese = m_pTextChinese->GetValue();
    TString strInputEnglish = m_pTextEnglish->GetValue();
    if (strInputEnum == _T("eL_") || strInputChinese == _T("") || strInputChinese == _T("") )
    {
        return;
    }
    std::vector<TString> mLanguageVector;
    mLanguageVector.push_back(strInputChinese);
    mLanguageVector.push_back(strInputEnglish);
    m_languageMap.insert(std::map<TString, std::vector<TString>>::value_type(strInputEnum,mLanguageVector));
}

void CEditLanguageDialog::DleLanguage()
{
    std::map<TString, std::vector<TString>>::iterator iter;
    iter = m_languageMap.find(m_strChangeEnum);
    if (iter != m_languageMap.end())
    {
        m_languageMap.erase(iter);
    }
    m_strChangeEnum = _T("");
}

void CEditLanguageDialog::AddSaveButtonPrompt()
{
    TString strMessage;
    TString strInputEnum = m_pTextEnum->GetValue();
    if (strInputEnum.size() == 3)
    {
        strMessage = _T("Enum not only as 'eL_'!");
    }
    else if (m_languageMap.find(strInputEnum) != m_languageMap.end() && m_bIsChange == false)
    {
        strMessage = _T("Enum has been exist!");
    }
    else if (m_pTextChinese->GetValue() == _T(""))
    {
        strMessage = _T("Chinese is empty!");
    }
    else if(m_pTextEnglish->GetValue() == _T(""))
    {
        strMessage = _T("English is empty!");
    }
    else
    {
        strMessage = _T("Added and saved successfully!");
        m_bAddSaveSuc = true;
    }
    wxMessageDialog* pMessageDlg = new wxMessageDialog(this, strMessage, _T("Edit Language"), wxOK|wxCENTRE|wxICON_NONE);
    pMessageDlg->ShowModal();
}

void CEditLanguageDialog::SetTextCtrlEnum()
{
    wxArrayString includeList; 
    TCHAR szBuffer;
    for (int i = 65; i <= 90; ++i)
    {
        szBuffer = i;
        includeList.Add(szBuffer);
    }
    for (int i = 97; i <= 122; ++i)
    {
        szBuffer = i;
        includeList.Add(szBuffer);
    }
    for (int i = 48; i <= 57; ++i)
    {
        szBuffer = i;
        includeList.Add(szBuffer);
    }
    includeList.Add(_T("_"));
    wxTextValidator textValidator(wxFILTER_INCLUDE_CHAR_LIST);
    textValidator.SetIncludes(includeList);
    m_pTextEnum->SetValidator(textValidator);
}

void CEditLanguageDialog::LanguageSwitch()
{
    m_bISLanguageSwitch = true;

    m_pStaticTextEnum->SetLabel(L10N_T(eL_Enum));
    m_pStaticTextChinese->SetLabel(L10N_T(eL_Chinese));
    m_pStaticTextEnglish->SetLabel(L10N_T(eL_English));
    m_pButtonClear->SetLabel(L10N_T(eL_Clear));
    m_pButtonExport->SetLabel(L10N_T(eL_Export));
    m_pSrchText->SetLabel(L10N_T(eL_SearchEnum));
    m_pButtonAdd->SetLabel(L10N_T(m_bIsSave ? eL_Save: eL_Add));
}
