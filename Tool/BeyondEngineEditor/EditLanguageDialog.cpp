#include "stdafx.h"
#include "EditLanguageDialog.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include <wx/srchctrl.h>
#include "Resource/ResourceManager.h"

#include "Utility/BeatsUtility/FilePathTool.h"
#include "wx/grid.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"
#include "RapidXML/rapidxml_print.hpp"
#include "Utility/BeatsUtility/md5.h"

BEGIN_EVENT_TABLE(CEditLanguageDialog, CEditDialogBase)
    EVT_BUTTON(ID_BUTTON_ADD, CEditLanguageDialog::OnButtonAdd)
    EVT_BUTTON(ID_BUTTON_EXPORT, CEditLanguageDialog::OnButtonExport)
    EVT_BUTTON(ID_BUTTON_IMPORT, CEditLanguageDialog::OnButtonImport)
    EVT_BUTTON(ID_BUTTON_SAVE, CEditLanguageDialog::OnButtonSave)
    EVT_SEARCHCTRL_SEARCH_BTN(ID_SERACH_LANGUAGE, CEditLanguageDialog::OnSearchLanguage)
    EVT_TEXT_ENTER(ID_SERACH_LANGUAGE,CEditLanguageDialog::OnSearchEnterLanguange)
    EVT_SEARCHCTRL_CANCEL_BTN(ID_SERACH_LANGUAGE, CEditLanguageDialog::OnCancleSrch)
    EVT_TEXT(ID_SERACH_LANGUAGE, CEditLanguageDialog::OnSrchUpdate)
    EVT_IDLE(CEditLanguageDialog::OnSrchIdle)
    EVT_GRID_CELL_CHANGED(CEditLanguageDialog::OnGridDataChanged)
    EVT_GRID_LABEL_RIGHT_CLICK(CEditLanguageDialog::OnRightClickOnGridLabel)
END_EVENT_TABLE()

CEditLanguageDialog::CEditLanguageDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_uLastEnumSearchTextUpdateTime(0)
    , m_bEnumSearchTextUpdate(false)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pBottomSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pLanguageGrid = new wxGrid(this, ID_LANGUAGE_GRID);
    m_pLanguageGrid->SetDefaultCellOverflow(false);
    m_pButtonSave = new wxButton(this, ID_BUTTON_SAVE, _T("Save"), wxDefaultPosition,wxSize(60,30));
    m_pButtonAdd = new wxButton(this, ID_BUTTON_ADD, _T("Add"), wxDefaultPosition,wxSize(60,30));
    m_pButtonExport = new wxButton(this, ID_BUTTON_EXPORT, _T("Export"), wxDefaultPosition, wxSize(60, 30));
    m_pButtonImport = new wxButton(this, ID_BUTTON_IMPORT, _T("导入"), wxDefaultPosition, wxSize(60, 30));
    m_pSrchCtrl = new wxSearchCtrl(this, ID_SERACH_LANGUAGE, _T(""),wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    pSizer->Add(pTopSizer, 1, wxGROW|wxALL, 5);
    pSizer->Add(pBottomSizer, 0, wxGROW|wxALL, 5);
    pTopSizer->Add(m_pLanguageGrid, 1, wxGROW | wxALL, 5);

    pBottomSizer->Add(m_pSrchCtrl, 1, wxGROW | wxALL, 5);
    pBottomSizer->Add(m_pButtonSave, 0, wxALL, 5);
    pBottomSizer->Add(m_pButtonAdd, 0, wxALL, 5);
    pBottomSizer->Add(m_pButtonExport, 0, wxALL, 5);
    pBottomSizer->Add(m_pButtonImport, 0, wxALL, 5);

    m_pSrchCtrl->ShowCancelButton(true);
    m_pLanguageGrid->CreateGrid(0, eLT_Count + 1);
    InitLanguageGrid();
}

CEditLanguageDialog::~CEditLanguageDialog()
{

}

void CEditLanguageDialog::InitLanguageGrid()
{
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    std::map<TString, TString>& languageTagMap = CLanguageManager::GetInstance()->GetLanguageTagMap();
    for (int i = eLT_Chinese; i < eLT_Count; ++i)
    {
        m_pLanguageGrid->SetColLabelValue(i, pszLanguageTypeString[i]);
    }
    m_pLanguageGrid->SetColLabelValue(eLT_Count, "Tag");
    m_pLanguageGrid->InsertRows(0, languageMap.size(), false);
    uint32_t uRowCounter = 0;
    for (auto iter = languageMap.begin(); iter != languageMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            m_pLanguageGrid->SetCellValue(uRowCounter, subIter->first, wxString::FromUTF8(subIter->second.c_str()));
        }
        m_pLanguageGrid->SetRowLabelValue(uRowCounter, wxString::FromUTF8(iter->first.c_str()));
        if (languageTagMap.find(iter->first) != languageTagMap.end())
        {
            m_pLanguageGrid->SetCellValue(uRowCounter, eLT_Count, wxString::FromUTF8(languageTagMap[iter->first].c_str()));
        }
        ++uRowCounter;
    }
    m_pLanguageGrid->SetRowLabelSize(wxGRID_AUTOSIZE);
}

void CEditLanguageDialog::OnButtonAdd( wxCommandEvent& /*event*/ )
{
    wxString strEnum = wxGetTextFromUser(_T("Input enum string"), _T("Enum string"), _T("eLTT_"));
    AddNewLanguageText((TString)strEnum);
}

void ReadServerErrCodeFile(std::map<uint32_t, std::map<ELanguageType, TString> >& languageServerErrCodeMap)
{
    TString filePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Language);
    filePath.append(_T("\\")).append("networkError.xml");
    if (CFilePathTool::GetInstance()->Exists(filePath.c_str()))
    {
        rapidxml::file<> fdoc(filePath.c_str());
        rapidxml::xml_document<> errCodeXML;
        try
        {
            errCodeXML.parse<rapidxml::parse_default>(fdoc.data());
        }
        catch (rapidxml::parse_error err)
        {
            BEATS_ASSERT(false, _T("Load config file %s faled!/n%s/n"), "errno.xml", err.what());
        }
        rapidxml::xml_node<>* pRootElement = errCodeXML.first_node("config");
        if (pRootElement)
        {
            for (auto element = pRootElement->first_node(); element; element = element->next_sibling())
            {
                std::map<ELanguageType, TString> curMap;
                curMap[eLT_Chinese] = element->first_attribute("lang_zhCN")->value();
                curMap[eLT_English] = element->first_attribute("lang_enUS")->value();
                languageServerErrCodeMap[_tstoi(element->first_attribute("code")->value())] = curMap;
            }
        }
    }
}

void ExportLanguage()
{
    std::map<uint32_t, std::map<ELanguageType, TString> > languageServerErrCodeMap;
    ReadServerErrCodeFile(languageServerErrCodeMap);
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    // 1. Export Bin File.
    CSerializer fileData;
    for (int i = 0; i < eLT_Count; ++i)
    {
        fileData << languageMap.size();
        for (auto iter = languageMap.begin(); iter != languageMap.end(); ++iter)
        {
            auto subIter = iter->second.find((ELanguageType)i);
            if (subIter != iter->second.end())
            {
                fileData << subIter->second;
            }
            else
            {
                fileData << iter->first;
            }
        }
        bool bRet = false;
        for (auto iter : languageServerErrCodeMap)
        {
            auto subIter = iter.second.find((ELanguageType)i);
            if (subIter != iter.second.end())
            {
                if (!bRet)
                {
                    fileData << languageServerErrCodeMap.size();
                    bRet = true;
                }
                fileData << iter.first;
                fileData << iter.second[(ELanguageType)i];
            }
        }
        TString strFilePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Language);
        strFilePath.append(_T("/")).append(pszLanguageTypeString[i]).append(_T(".bin"));
        fileData.Deserialize(strFilePath.c_str(), _T("wb+"));
        fileData.Reset();
    }
    //2. Export enum file.
    std::string content;
    content.append("#ifndef BEYONDENGINEEDITOR_LANGUAGEENUM_H__INCLUDE\n#define BEYONDENGINEEDITOR_LANGUAGEENUM_H__INCLUDE\n").append("\nenum ELanguageTextType\n{\n");
    for (auto iter = languageMap.begin(); iter != languageMap.end(); iter++)
    {
        content.append("    ").append(iter->first.c_str()).append(",\n");
    }
    content.append("\n    eL_Count,\n").append("    eL_Force32Bit = 0xFFFFFFFF\n};\n");
    content.append("#define LUA_LANGUAGE_MAP(LM)\\\n");
    int32_t nCounter = 0;
    for (auto iter = languageMap.begin(); iter != languageMap.end(); ++iter)
    {
        TCHAR szValueBuffer[32];
        _stprintf(szValueBuffer, "%d", nCounter++);
        content.append("    ").append("LM(").append(iter->first.c_str()).append(",").append(szValueBuffer).append(")\\\n");
    }
    content.append("\n#endif");
    fileData.Reset();
    fileData << content;
    fileData.SetWritePos(fileData.GetWritePos() - 1);// back scape the 0 in the string end.
    bool bFileTheSame = false;
    const TString strHeaderFilePath = CResourceManager::GetInstance()->GetResourcePath(eRT_SourceCode) + _T("/Language/Language.h");
    if (CFilePathTool::GetInstance()->Exists(strHeaderFilePath.c_str()))
    {
        CSerializer tmpData(strHeaderFilePath.c_str(), "rb");
        if (tmpData.GetWritePos() == fileData.GetWritePos())
        {
            CMD5 tmpMD5(tmpData.GetBuffer(), tmpData.GetWritePos());
            CMD5 currentMD5(fileData.GetBuffer(), fileData.GetWritePos());
            bFileTheSame = tmpMD5 == currentMD5;
        }
    }
    if (!bFileTheSame)
    {
        fileData.Deserialize(strHeaderFilePath.c_str(), _T("wb+"));
    }

    //3. Export txt file.
    for (int i = 0; i < eLT_Count; ++i)
    {
        bool bHasData = false;
        fileData.Reset();
        for (auto iter = languageMap.begin(); iter != languageMap.end(); ++iter)
        {
            auto subIter = iter->second.find((ELanguageType)i);
            if (subIter != iter->second.end())
            {
                TString strData = (TString)(wxString::FromUTF8(subIter->second.c_str()));
                if (!bHasData && !strData.empty())
                {
                    bHasData = true;
                }
                fileData << strData;
                fileData.SetWritePos(fileData.GetWritePos() - 1);
            }
            fileData << "\n";
            fileData.SetWritePos(fileData.GetWritePos() - 1);
        }
        if (bHasData)
        {
            TString strFilePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
            strFilePath.append(_T("/")).append(pszLanguageTypeString[i]).append(_T(".txt"));
            fileData.Deserialize(strFilePath.c_str(), _T("wt+"));
        }
    }
    fileData.Reset();
    const std::map<TString, TString>& languageTagMap = CLanguageManager::GetInstance()->GetLanguageTagMap();
    for (auto iter = languageTagMap.begin(); iter != languageTagMap.end(); ++iter)
    {
        TString strData = (TString)(wxString::FromUTF8(iter->second.c_str()));
        fileData << strData;
        fileData.SetWritePos(fileData.GetWritePos() - 1);
        fileData << "\n";
        fileData.SetWritePos(fileData.GetWritePos() - 1);
    }
    TString strFilePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    strFilePath.append(_T("/")).append("Tag").append(_T(".txt"));
    fileData.Deserialize(strFilePath.c_str(), _T("wt+"));

    fileData.Reset();
    for (auto iter = languageMap.begin(); iter != languageMap.end(); ++iter)
    {
        TString strData = (TString)(wxString::FromUTF8(iter->first.c_str()));
        fileData << strData;
        fileData.SetWritePos(fileData.GetWritePos() - 1);
        fileData << "\n";
        fileData.SetWritePos(fileData.GetWritePos() - 1);
    }
    strFilePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    strFilePath.append(_T("/")).append(_T("Enum.txt"));
    fileData.Deserialize(strFilePath.c_str(), _T("wt+"));
}

void CEditLanguageDialog::OnButtonExport( wxCommandEvent& /*event*/ )
{
    ExportLanguage();
    wxMessageBox(_T("Export Bin File and enum file are succeed."), _T("Edit Language"), wxOK | wxCENTRE | wxICON_NONE, this);
}

void CEditLanguageDialog::OnSearchLanguage( wxCommandEvent& event )
{
    TString strEnum = event.GetString().Lower();
    SearchEnum(strEnum);
}

void CEditLanguageDialog::OnSearchEnterLanguange( wxCommandEvent& event )
{
    TString strEnum = event.GetString().Lower();
    SearchEnum(strEnum);
}

void CEditLanguageDialog::OnButtonSave( wxCommandEvent& /*event*/ )
{
    CLanguageManager::GetInstance()->SaveLanguageListToFile();

    down_cast<CEditorMainFrame*>(GetParent())->LanguageSwitch(CLanguageManager::GetInstance()->GetCurrentLanguage(), true);
    wxMessageBox(_T("保存成功"));
}

void CEditLanguageDialog::OnCancleSrch( wxCommandEvent& /*event*/ )
{
}

void CEditLanguageDialog::SearchEnum( TString str )
{
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    for (auto iter = languageMap.begin(); iter != languageMap.end(); iter++)
    {
        if (iter->first == str)
        {
            wxVector<wxVariant> data;
            data.push_back( iter->first );
            for (int i = 0; i < eLT_Count; ++i)
            {
                auto subIter = iter->second.find((ELanguageType)i);
                if (subIter != iter->second.end())
                {
                    data.push_back(wxString::FromUTF8(subIter->second.c_str()));
                }
            }
        }
    }
}

void CEditLanguageDialog::OnSrchUpdate( wxCommandEvent& /*event*/ )
{
    m_uLastEnumSearchTextUpdateTime = GetTickCount();
    m_bEnumSearchTextUpdate = true;
}

void CEditLanguageDialog::OnSrchIdle( wxIdleEvent& /*event*/ )
{
    if (m_bEnumSearchTextUpdate && GetTickCount() - m_uLastEnumSearchTextUpdateTime > 700)
    {
        m_bEnumSearchTextUpdate = false;
        wxString szText = m_pSrchCtrl->GetValue().Lower();
        std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
        int row = 0;
        for (auto iter = languageMap.begin(); iter != languageMap.end(); iter++)
        {
            wxString strEnum = iter->first;
            bool bMatch = strEnum.Lower().find(szText.utf8_str()) != -1;
            if (bMatch)
            {
                m_pLanguageGrid->ShowRow(row);
            }
            else
            {
                bool bMatchSub = false;
                for (auto subIter = iter->second.begin(); subIter != iter->second.end(); subIter++)
                {
                    wxString strEnum = subIter->second;
                    bMatchSub = strEnum.Lower().find(szText.utf8_str()) != -1;
                    if (bMatchSub)
                    {
                        break;
                    }
                }
                if (bMatchSub)
                {
                    m_pLanguageGrid->ShowRow(row);
                }
                else
                {
                    m_pLanguageGrid->HideRow(row);
                }
            }
            row++;
        }
    }
}

void CEditLanguageDialog::OnGridDataChanged(wxGridEvent& event)
{
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    std::map<TString, TString>& languageTagMap = CLanguageManager::GetInstance()->GetLanguageTagMap();
    int nRow = event.GetRow();
    int nCol = event.GetCol();
    wxString strValue = m_pLanguageGrid->GetCellValue(nRow, nCol);
    TString strEnumStr = m_pLanguageGrid->GetRowLabelValue(nRow);
    auto languageIter = languageMap.find(strEnumStr);
    BEATS_ASSERT(languageIter != languageMap.end());
    if (nCol == eLT_Count)
    {
        if (strValue.empty())
        {
            languageTagMap.erase(strEnumStr);
        }
        else
        {
            languageTagMap[strEnumStr] = strValue.ToUTF8();
        }
    }
    else
    {
        languageIter->second[(ELanguageType)nCol] = strValue.ToUTF8();
    }
}

void CEditLanguageDialog::OnRightClickOnGridLabel(wxGridEvent& event)
{
    int nRow = event.GetRow();
    int nCol = event.GetCol();
    if (nCol == -1)
    {
        if (nRow == -1)
        {
        }
        else
        {
            wxMenu menu;
            menu.Append(0, _T("ChangeName"));
            menu.Append(1, _T("Delete"));
            int nRet = GetPopupMenuSelectionFromUser(menu, wxDefaultPosition);
            TString strOldLabel = m_pLanguageGrid->GetRowLabelValue(nRow);
            switch (nRet)
            {
            case 0:
            {
                  TString strNewName = wxGetTextFromUser(_T("Input the new name please："), wxGetTextFromUserPromptStr, strOldLabel);
                  if (!strNewName.empty() && strOldLabel != strNewName)
                  {
                      bool bAddSuccess = ExamLanguageText(strNewName, true);
                      if (bAddSuccess)
                      {
                          m_pLanguageGrid->SetRowLabelValue(nRow, strNewName);
                          std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
                          languageMap[strNewName] = languageMap[strOldLabel];
                          languageMap.erase(strOldLabel);
                          wxMessageBox(_T("更名成功"));
                      }
                  }
            }
                break;
            case 1:
                RemoveLanguageText(strOldLabel, nRow);
                break;
            case wxID_NONE:
                break;
            default:
                BEATS_ASSERT(false, _T("Never reach here!"));
                break;
            }

        }
    }
}

bool CEditLanguageDialog::AddNewLanguageText(const TString& strEnum)
{
    bool bRet = ExamLanguageText(strEnum, true);
    if (bRet)
    {
        std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
        m_pLanguageGrid->AppendRows(1);
        m_pLanguageGrid->SetRowLabelValue(m_pLanguageGrid->GetRows() - 1, strEnum);
        languageMap[strEnum] = std::map<ELanguageType, TString>();
        m_pLanguageGrid->SetRowLabelSize(wxGRID_AUTOSIZE);
        m_pLanguageGrid->Refresh();
    }
    return bRet;
}

bool CEditLanguageDialog::RemoveLanguageText(const TString& strEnum, int nRow)
{
    bool bRet = ExamLanguageText(strEnum, false);
    BEATS_ASSERT(bRet);
    if (bRet)
    {
        BEATS_ASSERT(m_pLanguageGrid->GetRowLabelValue(nRow) == strEnum);
        std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
        auto iter = languageMap.find(strEnum);
        if (iter != languageMap.end())
        {
            m_pLanguageGrid->DeleteRows(nRow);
            iter = languageMap.erase(iter);
            for (int nCurRow = nRow; nCurRow < m_pLanguageGrid->GetRows(); ++nCurRow)
            {
                m_pLanguageGrid->SetRowLabelValue(nCurRow, iter->first);
                ++iter;
            }
        }
    }
    return bRet;
}

bool CEditLanguageDialog::ExamLanguageText(const TString& strEnum, bool bCheckExist)
{
    bool bRet = false;
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    if (strEnum.empty())
    {
        wxMessageBox(_T("enum string Can't be empty!"));
    }
    else if (strEnum.find("eLTT_") != 0)
    {
        wxMessageBox(_T("enum string should begin with eLTT_"));
    }
    else if (!((wxString)strEnum).IsAscii())
    {
        wxMessageBox(_T("enum string should be all ascii!"));
    }
    else if (strEnum.find(_T(' '), 0) != 0xFFFFFFFF)
    {
        wxMessageBox(_T("enum string can't contain space!"));
    }
    else if (bCheckExist && languageMap.find(strEnum) != languageMap.end())
    {
        wxMessageBox(wxString::Format(_T("enum string %s already exists!"), strEnum.c_str()));
    }
    else
    {
        bRet = true;
    }
    return bRet;
}

void CEditLanguageDialog::LanguageSwitch()
{
    m_pButtonSave->SetLabel(L10N_T(eLTT_Editor_Common_Save));
    m_pButtonAdd->SetLabel(L10N_T(eLTT_Editor_Common_Add));
    m_pButtonExport->SetLabel(L10N_T(eLTT_Editor_Common_Export));
}

std::vector<wxString> SplitString(wxString& data, wxString& str)
{
    std::vector<wxString> vec = {};
    int index = 0;
    while (true)
    {
        int start = index;
        index = data.find(str, index);
        if (index != wxNOT_FOUND)
        {
            vec.push_back(data.SubString(start, index - 1));
        }
        else
        {
            if (!data.EndsWith(str))
            {
                vec.push_back(data.SubString(start, data.Length()));
            }
            break;
        }
        index += str.Length();
    }
    return vec;
}

void CEditLanguageDialog::OnButtonImport(wxCommandEvent& /*event*/)
{
    TString szBinaryPath;
    CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, szBinaryPath, _T("选择要导入的文件"), _T("excel file(*.csv)\0*.csv\0\0"), NULL);
    if (szBinaryPath.length() > 0)
    {
        wxString data;
        wxFile file;
        if (file.Open(szBinaryPath, wxFile::read))
        {
            if (file.ReadAll(&data))
            {
                wxString rowFlag = _T("\r\n");
                wxString strFlag = _T(",");
                std::vector<wxString> rows = SplitString(data, rowFlag);
                if (rows.size() != 0)
                {
                    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
                    languageMap.clear();
                    for (size_t i = 0; i < rows.size(); i++)
                    {
                        std::vector<wxString> rowStrs = SplitString(rows[i], strFlag);
                        std::vector<wxString> languageStrs;
                        languageStrs.assign(++rowStrs.begin(), rowStrs.end());

                        for (size_t i = 0; i < eLT_Count; i++)
                        {
                            if (!languageStrs[i].empty())
                            {
                                languageMap[rowStrs[0].ToStdString()][(ELanguageType)i] = languageStrs[i].ToUTF8();
                            }
                        }
                    }
                }
                m_pLanguageGrid->ClearGrid();
                InitLanguageGrid();
                wxMessageBox(wxT("导入成功!"));
            }
        }
    }
}
