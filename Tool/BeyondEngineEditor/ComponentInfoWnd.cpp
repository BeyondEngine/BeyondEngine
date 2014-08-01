#include "stdafx.h"
#include "ComponentInfoWnd.h"
#include <wx/grid.h>
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProjectDirectory.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/componentbase.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"

CComponentInfoWnd::CComponentInfoWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_nCurrentChoice(-1)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);
    m_pGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(550, 300));
    m_pGrid->CreateGrid( 0, 0);
    pMainSizer->Add(m_pGrid, 1, wxALL, 0);

    wxBoxSizer* pBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxString strViewChoiceString[] = { wxT("查看所有文件"), wxT("查看当前加载文件"), wxT("查看所有加载组件")};
    int uChoiceCount = sizeof( strViewChoiceString ) / sizeof( wxString );
    m_pViewChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, uChoiceCount, strViewChoiceString, 0 );
    pBtnSizer->Add(m_pViewChoice, 0, wxALL, 0);
    m_pRefreshBtn = new wxButton(this, wxID_ANY, wxT("刷新"));
    pBtnSizer->Add(m_pRefreshBtn, 0, wxALL, 0);
    pMainSizer->Add(pBtnSizer, 0, wxALL, 0);
    this->Fit();
    m_pViewChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CComponentInfoWnd::OnViewChoiceChanged ), NULL, this );
    m_pRefreshBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CComponentInfoWnd::OnRefreshBtnClicked ), NULL, this );
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CComponentInfoWnd::OnClose), NULL, this);
}

CComponentInfoWnd::~CComponentInfoWnd()
{
    m_pViewChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CComponentInfoWnd::OnViewChoiceChanged ), NULL, this );
    m_pRefreshBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CComponentInfoWnd::OnRefreshBtnClicked ), NULL, this );
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

void CComponentInfoWnd::OnViewChoiceChanged( wxCommandEvent& event )
{
    if (m_nCurrentChoice != event.GetSelection())
    {
        if (m_pGrid->GetRows() > 0)
        {
            m_pGrid->DeleteRows(0, m_pGrid->GetRows());
        }
        if (m_pGrid->GetCols() > 0)
        {
            m_pGrid->DeleteCols(0, m_pGrid->GetCols());
        }
        m_nCurrentChoice = event.GetSelection();
        switch (m_nCurrentChoice)
        {
        case 0:
            {
                CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
                const std::vector<TString>* pFileList = pProject->GetFileList();
                size_t uFileCount = pFileList->size();
                m_pGrid->AppendRows(uFileCount);
                m_pGrid->AppendCols(2);
                m_pGrid->SetColLabelValue(0, _T("文件ID"));
                m_pGrid->SetColLabelValue(1, _T("文件名称"));
                for (size_t i = 0; i < uFileCount; ++i)
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
                const TString& strCurWorkingFile = CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath();
                size_t uCurFileId = pProject->GetComponentFileId(strCurWorkingFile);
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
                std::vector<size_t> shouldLoadFiles;
                while (directories.size() > 0)
                {
                    pCurFileDirectory = directories.back();
                    const std::vector<size_t>& filesInDirectory = pCurFileDirectory->GetFileList();
                    for (size_t j = 0; j < filesInDirectory.size(); ++j)
                    {
                        shouldLoadFiles.push_back(filesInDirectory[j]);
                    }
                    directories.pop_back();
                }
                shouldLoadFiles.push_back(uCurFileId);
                std::set<size_t> loadedFilesTest = CComponentProxyManager::GetInstance()->GetLoadedFiles();
                for (size_t i = 0; i < shouldLoadFiles.size(); ++i)
                {
                    BEATS_ASSERT(loadedFilesTest.find(shouldLoadFiles[i]) != loadedFilesTest.end());
                    loadedFilesTest.erase(shouldLoadFiles[i]);
                }
                BEATS_ASSERT(loadedFilesTest.empty(), _T("Exam loaded files failed!"));
#endif
                const std::set<size_t>& loadedFiles = CComponentProxyManager::GetInstance()->GetLoadedFiles();

                size_t uFileCount = loadedFiles.size();
                m_pGrid->AppendRows(uFileCount);
                m_pGrid->AppendCols(2);
                m_pGrid->SetColLabelValue(0, _T("文件ID"));
                m_pGrid->SetColLabelValue(1, _T("文件名称"));
                size_t uCounter = 0;
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
                size_t uCounter = 0;
                m_pGrid->AppendCols(4);
                m_pGrid->SetColLabelValue(0, _T("GUID"));
                m_pGrid->SetColLabelValue(1, _T("ID"));
                m_pGrid->SetColLabelValue(2, _T("ClassString"));
                m_pGrid->SetColLabelValue(3, _T("FileId"));
                const std::map<size_t, std::map<size_t, CComponentBase*>*>* pComponents = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
                for (auto iter = pComponents->begin(); iter != pComponents->end(); ++iter)
                {
                    std::map<size_t, CComponentBase*>* pComponentsMap = iter->second;
                    for (auto subIter = pComponentsMap->begin(); subIter != pComponentsMap->end(); ++subIter)
                    {
                        m_pGrid->AppendRows();
                        if (subIter == pComponentsMap->begin())
                        {
                            m_pGrid->SetCellValue(wxString::Format(_T("0x%x"), iter->first), uCounter, 0);
                        }
                        size_t uComponentId = subIter->first;
                        CComponentProxy* pComponentProxy = static_cast<CComponentProxy*>(subIter->second);
                        m_pGrid->SetCellValue(wxString::Format(_T("%d"), uComponentId), uCounter, 1);
                        wxString strShowGuidStr;
                        if (pComponentProxy->GetId() != pComponentProxy->GetProxyId())
                        {
                            strShowGuidStr = _T("Reference: ");
                            m_pGrid->SetCellTextColour(*wxGREEN, uCounter, 2);
                        }
                        strShowGuidStr.append(pComponentProxy->GetClassStr());
                        m_pGrid->SetCellValue(strShowGuidStr, uCounter, 2);
                        std::map<size_t, size_t>* pComponentToFileMap = CComponentProxyManager::GetInstance()->GetProject()->GetComponentToFileMap();
                        size_t uFileId = 0xFFFFFFFF;
                        if (pComponentToFileMap->find(uComponentId) != pComponentToFileMap->end())
                        {
                            uFileId = pComponentToFileMap->find(uComponentId)->second;
                        }
                        else
                        {
                            BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentsInCurScene().find(uComponentId) != CComponentProxyManager::GetInstance()->GetComponentsInCurScene().end());
                            uFileId = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(CComponentProxyManager::GetInstance()->GetCurrentViewFilePath());
                        }
                        BEATS_ASSERT(uFileId != 0xFFFFFFFF);
                        m_pGrid->SetCellValue(wxString::Format(_T("%d"), uFileId), uCounter, 3);
                        uCounter++;
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
}

void CComponentInfoWnd::OnRefreshBtnClicked(wxCommandEvent& /*event*/)
{
    wxCommandEvent event;
    event.SetInt(m_nCurrentChoice);
    m_nCurrentChoice = 0xFFFFFFFF;
    this->OnViewChoiceChanged(event);
}

