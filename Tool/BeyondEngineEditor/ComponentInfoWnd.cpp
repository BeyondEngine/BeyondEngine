#include "stdafx.h"
#include "ComponentInfoWnd.h"
#include <wx/grid.h>
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProject.h"
#include "Component/Component/ComponentProjectDirectory.h"
#include "Component/Component/componentbase.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"
#include "Resource/Resource.h"
#include "Resource/ResourceManager.h"
#include <algorithm>
#include <wx/srchctrl.h>
#include <set>

BEGIN_EVENT_TABLE(CComponentInfoWnd, wxFrame)
EVT_GRID_COL_SORT(CComponentInfoWnd::OnGridColSort)
EVT_GRID_CELL_LEFT_CLICK(CComponentInfoWnd::OnCellLeftClick)
EVT_GRID_CELL_LEFT_DCLICK(CComponentInfoWnd::OnCellLeftDclick)
EVT_IDLE(CComponentInfoWnd::OnSrchIdle)
END_EVENT_TABLE()

CComponentInfoWnd::CComponentInfoWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
: super(parent, id, title, pos, size, style, name)
, m_nCurrentChoice(-1)
, m_uLastEnumSearchTextUpdateTime(0)
, m_bEnumSearchTextUpdate(false)
, m_bShowUnInvoked(false)
{
    SetMaxClientSize(wxSize(1600, 800));
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);
    m_pGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0));
    m_pGrid->CreateGrid(0, 0);
    pMainSizer->Add(m_pGrid, 1, wxALL, 0);

    wxBoxSizer* pBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxString strViewChoiceString[] = { wxT("查看所有文件"), wxT("查看当前加载文件"), wxT("查看所有组件"), wxT("查看所有加载组件"), wxT("查看当前加载资源文件") };
    int uChoiceCount = sizeof(strViewChoiceString) / sizeof(wxString);
    m_pViewChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, uChoiceCount, strViewChoiceString, 0);
    pBtnSizer->Add(m_pViewChoice, 0, wxALL, 5);
    m_pRefreshBtn = new wxButton(this, wxID_ANY, wxT("刷新"));
    pBtnSizer->Add(m_pRefreshBtn, 0, wxALL, 5);
    m_pSrchCtrl = new wxSearchCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_pSrchCtrl->ShowCancelButton(true);
    pBtnSizer->Add(m_pSrchCtrl, 0, wxALL, 5);
    m_pShowUnInvokedCheckBox = new wxCheckBox(this, wxID_ANY, wxT("显示未使用组件"));
    m_pShowUnInvokedCheckBox->Hide();
    pBtnSizer->Add(m_pShowUnInvokedCheckBox, 0, wxALL, 5);
    pMainSizer->Add(pBtnSizer, 0, wxALL, 0);
    this->Fit();
    m_pViewChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CComponentInfoWnd::OnViewChoiceChanged), NULL, this);
    m_pRefreshBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CComponentInfoWnd::OnRefreshBtnClicked), NULL, this);
    m_pShowUnInvokedCheckBox->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CComponentInfoWnd::OnShowUnInvokedCheckBox), NULL, this);
    m_pSrchCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CComponentInfoWnd::OnSrchUpdate), NULL, this);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CComponentInfoWnd::OnClose), NULL, this);
}

CComponentInfoWnd::~CComponentInfoWnd()
{
    m_pViewChoice->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CComponentInfoWnd::OnViewChoiceChanged), NULL, this);
    m_pRefreshBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CComponentInfoWnd::OnRefreshBtnClicked), NULL, this);
    m_pShowUnInvokedCheckBox->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CComponentInfoWnd::OnShowUnInvokedCheckBox), NULL, this);
    m_pSrchCtrl->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CComponentInfoWnd::OnSrchUpdate), NULL, this);
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CComponentInfoWnd::OnClose), NULL, this);
}

bool CComponentInfoWnd::Show(bool bShow /*= true*/)
{
    super::Show(bShow);
    if (bShow)
    {
        wxCommandEvent event;
        event.SetInt(0);
        m_pViewChoice->SetSelection(0);
        m_nCurrentChoice = 0xFFFFFFFF;
        this->OnViewChoiceChanged(event);
    }
    return true;
}

void CComponentInfoWnd::OnClose(wxCloseEvent& /*event*/)
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_ComponentInfoBtn, false);
    pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    this->Show(false);
}

void CComponentInfoWnd::OnViewChoiceChanged(wxCommandEvent& event)
{
    m_pShowUnInvokedCheckBox->SetValue(false);
    m_pShowUnInvokedCheckBox->Hide();
    m_bShowUnInvoked = false;
    if (m_nCurrentChoice != event.GetSelection())
    {
        ClearGrid();
        m_nCurrentChoice = event.GetSelection();
        switch (m_nCurrentChoice)
        {
        case 0:
        {
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
            const std::vector<TString>* pFileList = pProject->GetFileList();
            uint32_t uFileCount = pFileList->size();
            m_pGrid->AppendRows(uFileCount);
            m_pGrid->AppendCols(2);
            m_pGrid->SetColLabelValue(0, _T("文件ID"));
            m_pGrid->SetColLabelValue(1, _T("文件名称"));
            for (uint32_t i = 0; i < uFileCount; ++i)
            {
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), i), i, 0);
                m_pGrid->SetCellValue(pFileList->at(i), i, 1);
            }
        }
        break;
        case 1:
        {
#ifdef _DEBUG
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
            uint32_t uCurFileId = CComponentProxyManager::GetInstance()->GetCurrLoadFileId();
            if (uCurFileId != 0xFFFFFFFF)
            {
                auto iter = pProject->GetFileToDirectoryMap()->find(uCurFileId);
                BEATS_ASSERT(iter != pProject->GetFileToDirectoryMap()->end());
                CComponentProjectDirectory* pCurFileDirectory = iter->second;
                BEATS_ASSERT(pCurFileDirectory != NULL);
                std::vector<CComponentProjectDirectory*> directories;
                pCurFileDirectory = pCurFileDirectory->GetParent();
                while (pCurFileDirectory != NULL)
                {
                    directories.push_back(pCurFileDirectory);
                    pCurFileDirectory = pCurFileDirectory->GetParent();
                }
                std::vector<uint32_t> shouldLoadFiles;
                while (directories.size() > 0)
                {
                    pCurFileDirectory = directories.back();
                    const std::vector<uint32_t>& filesInDirectory = pCurFileDirectory->GetFileList();
                    for (uint32_t j = 0; j < filesInDirectory.size(); ++j)
                    {
                        shouldLoadFiles.push_back(filesInDirectory[j]);
                    }
                    directories.pop_back();
                }
                shouldLoadFiles.push_back(uCurFileId);
                std::vector<uint32_t> loadedFilesTest = CComponentProxyManager::GetInstance()->GetLoadedFiles();
                BEATS_ASSERT(loadedFilesTest.size() >= shouldLoadFiles.size()); // it's ok to get more files because we may load some bcf manually.
                for (uint32_t i = 0; i < shouldLoadFiles.size(); ++i)
                {
                    auto iter = std::find(loadedFilesTest.begin(), loadedFilesTest.end(), shouldLoadFiles[i]);
                    BEATS_ASSERT(iter != loadedFilesTest.end());
                    loadedFilesTest.erase(iter);
                }
                BEATS_ASSERT(loadedFilesTest.empty(), _T("Exam loaded files failed!"));
            }
#endif
            const std::vector<uint32_t>& loadedFiles = CComponentProxyManager::GetInstance()->GetLoadedFiles();

            uint32_t uFileCount = loadedFiles.size();
            m_pGrid->AppendRows(uFileCount);
            m_pGrid->AppendCols(2);
            m_pGrid->SetColLabelValue(0, _T("文件ID"));
            m_pGrid->SetColLabelValue(1, _T("文件名称"));
            uint32_t uCounter = 0;
            for (auto iter = loadedFiles.begin(); iter != loadedFiles.end(); ++iter)
            {
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), *iter), uCounter, 0);
                const TString& strFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(*iter);
                m_pGrid->SetCellValue(strFileName, uCounter, 1);
                uCounter++;
            }
        }
        break;
        case 2:
        {
            m_pShowUnInvokedCheckBox->Show();
            ShowAllComponent();
        }
        break;
        case 3:
        {
            ShowInstanceComponent();
        }
        break;
        case 4:
        {
            uint32_t uRowCounter = 0;
            CResourceManager::GetInstance()->CleanUp();
            std::map<EResourceType, std::vector< SharePtr<CResource> > > resourceTypeMap;
            const std::map<TString, SharePtr<CResource>>& resourceMap = CResourceManager::GetInstance()->GetResourceMap();
            for (auto iter = resourceMap.begin(); iter != resourceMap.end(); ++iter)
            {
                resourceTypeMap[iter->second->GetType()].push_back(iter->second);
            }
            m_pGrid->AppendCols(4);
            m_pGrid->SetColLabelValue(0, _T("Type"));
            m_pGrid->SetColLabelValue(1, _T("RefCount"));
            m_pGrid->SetColLabelValue(2, _T("Path"));
            m_pGrid->SetColLabelValue(3, _T("Description"));
            for (auto iter = resourceTypeMap.begin(); iter != resourceTypeMap.end(); ++iter)
            {
                for (size_t i = 0; i < iter->second.size(); ++i)
                {
                    m_pGrid->AppendRows();
                    if (i == 0)
                    {
                        m_pGrid->SetCellValue(wxString::Format(wxT("%s x %d"), pszResourceTypeString[iter->first], iter->second.size()), uRowCounter, 0);
                    }
                    m_pGrid->SetCellValue(wxString::Format(wxT("%d"), iter->second[i].RefCount() - 1), uRowCounter, 1);
                    m_pGrid->SetCellValue(iter->second[i]->GetFilePath(), uRowCounter, 2);
#ifdef DEVELOP_VERSION
                    m_pGrid->SetCellValue(iter->second[i]->GetDescription(), uRowCounter, 3);
#endif
                    uRowCounter++;
                }
            }
        }
        break;
        default:
            BEATS_ASSERT(false, _T("Impossible to reach here!"));
            break;
        }
    }
    m_pGrid->AutoSizeColumns();
    this->Fit();
}

void CComponentInfoWnd::OnRefreshBtnClicked(wxCommandEvent& /*event*/)
{
    wxCommandEvent event;
    event.SetInt(m_nCurrentChoice);
    m_nCurrentChoice = 0xFFFFFFFF;
    this->OnViewChoiceChanged(event);
}

void CComponentInfoWnd::OnGridColSort(wxGridEvent& event)
{
    const int col = event.GetCol();
    size_t uColumn = m_pGrid->GetTable()->GetColsCount();
    size_t uRow = m_pGrid->GetTable()->GetRowsCount();
    std::vector<wxString> dataList;
    for (size_t i = 0; i < uRow; i++)
    {
        for (size_t j = 0; j < uColumn; j++)
        {
            dataList.push_back(m_pGrid->GetCellValue(i, j));
        }
    }
    bool bValid = true;
    std::vector<std::pair<int, int>> sortValueList;
    for (size_t i = 0; i < uRow; i++)
    {
        wxString strValue = m_pGrid->GetCellValue(i, col);
        long nSortValue = 0;
        if (strValue.ToLong(&nSortValue))
        {
            sortValueList.push_back(std::pair<int, int>(nSortValue, i));
        }
        else
        {
            bValid = false;
        }
    }
    bool bAscending = m_pGrid->IsSortOrderAscending();
    std::sort(sortValueList.begin(), sortValueList.end(), [=](const std::pair<int, int> v1, const std::pair<int, int> v2)
    {
        return bAscending ? v1.first < v2.first : v1.first > v2.first;
    });
    if (bValid)
    {
        for (size_t i = 0; i < uRow; i++)
        {
            for (size_t j = 0; j < uColumn; j++)
            {
                m_pGrid->SetCellValue(i, j, dataList[sortValueList[i].second * uColumn + j]);
            }
        }
    }
}

void CComponentInfoWnd::OnCellLeftClick(wxGridEvent& event)
{
    m_pGrid->SelectRow(event.GetRow(), false);
    if (m_nCurrentChoice == 3)//show all loaded components
    {
        wxString strValue = m_pGrid->GetCellValue(event.GetRow(), 1);
        unsigned long ulId = 0xFFFFFFFF;
        if (strValue.ToULong(&ulId))
        {
            CComponentBase* pComponent = CComponentProxyManager::GetInstance()->GetComponentInstance(ulId);
            CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
            pMainFrame->GetPropGridManager()->ClearPage(0);
            pMainFrame->GetPropGridManager()->InsertComponentsInPropertyGrid(static_cast<CComponentProxy*>(pComponent));
        }
    }
}

void CComponentInfoWnd::OnCellLeftDclick(wxGridEvent& event)
{
    if (m_nCurrentChoice == 2 || m_nCurrentChoice == 3)
    {
        wxString strId = m_pGrid->GetCellValue(event.GetRow(), 1);
        unsigned long uId = 0xFFFFFFFF;
        if (strId.ToULong(&uId))
        {
            CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
            pMainFrame->JumpToComponent(uId);
        }
    }
}

void CComponentInfoWnd::OnSrchIdle(wxIdleEvent& /*event*/)
{
    if (m_nCurrentChoice == 2 || m_nCurrentChoice == 3)
    {
        if (m_bEnumSearchTextUpdate && GetTickCount() - m_uLastEnumSearchTextUpdateTime > 700)
        {
            ClearGrid();
            m_bEnumSearchTextUpdate = false;
            wxString szText = m_pSrchCtrl->GetValue().Lower();
            if (m_nCurrentChoice == 2)
            {
                ShowAllComponent(szText);
            }
            else if (m_nCurrentChoice == 3)
            {
                ShowInstanceComponent(szText);
            }
            m_pGrid->AutoSizeColumns();
            this->Fit();
        }
    }
}

void CComponentInfoWnd::OnSrchUpdate(wxCommandEvent& /*event*/)
{
    m_uLastEnumSearchTextUpdateTime = GetTickCount();
    m_bEnumSearchTextUpdate = true;
}

void CComponentInfoWnd::ShowInstanceComponent(wxString szText)
{
    uint32_t uRowCounter = 0;
    m_pGrid->AppendCols(4);
    m_pGrid->SetColLabelValue(0, _T("GUID"));
    m_pGrid->SetColLabelValue(1, _T("ID"));
    m_pGrid->SetColLabelValue(2, _T("ClassString"));
    m_pGrid->SetColLabelValue(3, _T("FileId"));
    const std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>* pComponents = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
    for (auto iter = pComponents->begin(); iter != pComponents->end(); ++iter)
    {
        std::map<uint32_t, CComponentBase*>* pComponentsMap = iter->second;
        wxString strClassName = CComponentProxyManager::GetInstance()->GetComponentTemplate(iter->first)->GetClassStr();
        bool bMatch = strClassName.Lower().find(szText.utf8_str()) != -1;
        if (bMatch)
        {
            for (auto subIter = pComponentsMap->begin(); subIter != pComponentsMap->end(); ++subIter)
            {
                m_pGrid->AppendRows();
                if (subIter == pComponentsMap->begin())
                {
                    m_pGrid->SetCellValue(wxString::Format(_T("0x%08x"), iter->first), uRowCounter, 0);
                }
                uint32_t uComponentId = subIter->first;
                CComponentProxy* pComponentProxy = static_cast<CComponentProxy*>(subIter->second);
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), uComponentId), uRowCounter, 1);
                wxString strShowGuidStr;
                strShowGuidStr.append(pComponentProxy->GetClassStr());
                m_pGrid->SetCellValue(strShowGuidStr, uRowCounter, 2);
                const std::map<uint32_t, uint32_t>* pComponentToFileMap = CComponentProxyManager::GetInstance()->GetProject()->GetComponentToFileMap();
                uint32_t uFileId = 0xFFFFFFFF;
                if (pComponentToFileMap->find(uComponentId) != pComponentToFileMap->end())
                {
                    uFileId = pComponentToFileMap->find(uComponentId)->second;
                }
                else
                {
                    BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentsInCurScene().find(uComponentId) != CComponentProxyManager::GetInstance()->GetComponentsInCurScene().end());
                    uFileId = CComponentProxyManager::GetInstance()->GetCurrentViewFileId();
                }
                BEATS_ASSERT(uFileId != 0xFFFFFFFF);
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), uFileId), uRowCounter, 3);
                uRowCounter++;
            }
        }
    }
}

void CComponentInfoWnd::ShowAllComponent(wxString szText)
{
    uint32_t uRowCounter = 0;
    m_pGrid->AppendCols(5);
    m_pGrid->SetColLabelValue(0, _T("GUID"));
    m_pGrid->SetColLabelValue(1, _T("ID"));
    m_pGrid->SetColLabelValue(2, _T("ClassString"));
    m_pGrid->SetColLabelValue(3, _T("Path"));
    m_pGrid->SetColLabelValue(4, _T("FileId"));
    auto& abstractGuidNameMap = CComponentProxyManager::GetInstance()->GetAbstractComponentNameMap();
    if (m_bShowUnInvoked)
    {
        std::set<uint32_t> uninvokeGuidList;
        CComponentProxyManager::GetInstance()->CheckForUnInvokedGuid(uninvokeGuidList);
        for (auto guid : uninvokeGuidList)
        {
            wxString strClassName;
            if (abstractGuidNameMap.find(guid) != abstractGuidNameMap.end())
            {
                strClassName = abstractGuidNameMap.find(guid)->second;
            }
            else
            {
                strClassName = CComponentProxyManager::GetInstance()->GetComponentTemplate(guid)->GetClassStr();
            }
            bool bMatch = strClassName.Lower().find(szText.utf8_str()) != -1;
            if (bMatch)
            {
                m_pGrid->AppendRows();
                m_pGrid->SetCellValue(wxString::Format(_T("0x%08x"), guid), uRowCounter, 0);
                m_pGrid->SetCellValue(strClassName, uRowCounter, 2);
                m_pGrid->SetCellValue(_T("没有使用"), uRowCounter, 3);
                wxGridCellAttr* attr = new wxGridCellAttr;
                attr->SetBackgroundColour(*wxRED);
                m_pGrid->SetRowAttr(uRowCounter, attr);
                uRowCounter++;
            }
        }
    }
    //used
    const std::map<uint32_t, std::vector<uint32_t> >* pComponents = CComponentProxyManager::GetInstance()->GetProject()->GetTypeToComponentMap();
    std::map<uint32_t, std::set<uint32_t> > typeRefInComponentMap = *CComponentProxyManager::GetInstance()->GetProject()->GetTypeRefInComponentMap();
    const std::map<uint32_t, uint32_t>* pComponentToFileMap = CComponentProxyManager::GetInstance()->GetProject()->GetComponentToFileMap();
    for (auto iter = pComponents->begin(); iter != pComponents->end(); ++iter)
    {
        wxString strClassName = CComponentProxyManager::GetInstance()->GetComponentTemplate(iter->first)->GetClassStr();
        bool bMatch = strClassName.Lower().find(szText.utf8_str()) != -1;
        if (bMatch)
        {
            uint32_t uGuid = iter->first;
            for (auto subIter : iter->second)
            {
                m_pGrid->AppendRows();
                if (subIter == iter->second[0])
                {
                    m_pGrid->SetCellValue(wxString::Format(_T("0x%08x"), uGuid), uRowCounter, 0);
                }
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), subIter), uRowCounter, 1);
                m_pGrid->SetCellValue(strClassName, uRowCounter, 2);
                uint32_t uFileId = 0xFFFFFFFF;
                if (pComponentToFileMap->find(subIter) != pComponentToFileMap->end())
                {
                    uFileId = pComponentToFileMap->find(subIter)->second;
                }
                else
                {
                    BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentsInCurScene().find(subIter) != CComponentProxyManager::GetInstance()->GetComponentsInCurScene().end());
                    uFileId = CComponentProxyManager::GetInstance()->GetCurrentViewFileId();
                }
                BEATS_ASSERT(uFileId != 0xFFFFFFFF);
                const TString& strFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(uFileId);
                m_pGrid->SetCellValue(strFileName, uRowCounter, 3);
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), uFileId), uRowCounter, 4);
                uRowCounter++;
            }
            auto typeRefIter = typeRefInComponentMap.find(uGuid);
            if (typeRefIter != typeRefInComponentMap.end())
            {
                for (auto subIter = typeRefIter->second.begin(); subIter != typeRefIter->second.end(); ++subIter)
                {
                    uint32_t uComponentId = *subIter;
                    m_pGrid->AppendRows();
                    auto attr = m_pGrid->GetOrCreateCellAttr(uRowCounter, 0);
                    attr->SetBackgroundColour(*wxYELLOW);
                    m_pGrid->SetRowAttr(uRowCounter, attr);
                    m_pGrid->SetCellValue(wxString::Format(_T("%d"), uComponentId), uRowCounter, 1);
                    m_pGrid->SetCellValue(strClassName, uRowCounter, 2);
                    uint32_t uFileId = 0xFFFFFFFF;
                    if (pComponentToFileMap->find(uComponentId) != pComponentToFileMap->end())
                    {
                        uFileId = pComponentToFileMap->find(uComponentId)->second;
                    }
                    else
                    {
                        BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentsInCurScene().find(uComponentId) != CComponentProxyManager::GetInstance()->GetComponentsInCurScene().end());
                        uFileId = CComponentProxyManager::GetInstance()->GetCurrentViewFileId();
                    }
                    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
                    const TString& strFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(uFileId);
                    m_pGrid->SetCellValue(strFileName, uRowCounter, 3);
                    m_pGrid->SetCellValue(wxString::Format(_T("%d"), uFileId), uRowCounter, 4);
                    uRowCounter++;
                }
                typeRefInComponentMap.erase(typeRefIter);
            }
        }
    }
    for (auto iter = typeRefInComponentMap.begin(); iter != typeRefInComponentMap.end(); ++iter)
    {
        uint32_t uGuid = iter->first;
        wxString strClassName;
        auto abstractIter = abstractGuidNameMap.find(uGuid);
        if (abstractIter != abstractGuidNameMap.end())
        {
            strClassName = abstractIter->second;
        }
        else
        {
            strClassName = CComponentProxyManager::GetInstance()->GetComponentTemplate(uGuid)->GetClassStr();
        }
        bool bMatch = strClassName.Lower().find(szText.utf8_str()) != -1;
        if (bMatch)
        {
            for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
            {
                uint32_t uComponentId = *subIter;
                m_pGrid->AppendRows();
                auto attr = m_pGrid->GetOrCreateCellAttr(uRowCounter, 0);
                attr->SetBackgroundColour(*wxYELLOW);
                m_pGrid->SetRowAttr(uRowCounter, attr);
                if (subIter == iter->second.begin())
                {
                    m_pGrid->SetCellValue(wxString::Format(_T("0x%08x"), uGuid), uRowCounter, 0);
                }
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), uComponentId), uRowCounter, 1);
                m_pGrid->SetCellValue(strClassName, uRowCounter, 2);
                uint32_t uFileId = 0xFFFFFFFF;
                if (pComponentToFileMap->find(uComponentId) != pComponentToFileMap->end())
                {
                    uFileId = pComponentToFileMap->find(uComponentId)->second;
                }
                else
                {
                    BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentsInCurScene().find(uComponentId) != CComponentProxyManager::GetInstance()->GetComponentsInCurScene().end());
                    uFileId = CComponentProxyManager::GetInstance()->GetCurrentViewFileId();
                }
                BEATS_ASSERT(uFileId != 0xFFFFFFFF);
                const TString& strFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(uFileId);
                m_pGrid->SetCellValue(strFileName, uRowCounter, 3);
                m_pGrid->SetCellValue(wxString::Format(_T("%d"), uFileId), uRowCounter, 4);
                uRowCounter++;
            }
        }
    }
}

void CComponentInfoWnd::ClearGrid()
{
    if (m_pGrid->GetRows() > 0)
    {
        m_pGrid->DeleteRows(0, m_pGrid->GetRows());
    }
    if (m_pGrid->GetCols() > 0)
    {
        m_pGrid->DeleteCols(0, m_pGrid->GetCols());
    }
}

void CComponentInfoWnd::OnShowUnInvokedCheckBox(wxCommandEvent& event)
{
    m_bShowUnInvoked = event.IsChecked() ? true : false;
    ClearGrid();
    ShowAllComponent();
    m_pGrid->AutoSizeColumns();
    this->Fit();
}
