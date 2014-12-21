#include "stdafx.h"
#include "ScanFileDialog.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "Resource/ResourceManager.h"
#include "Render/TextureAtlas.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "wx/dir.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"

CScanFileDialog::CScanFileDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_pSizer(NULL)
    , m_pGrid(NULL)
{
    m_pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_pSizer);
    m_pGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0));
    m_pGrid->CreateGrid(0, 0);
    m_pSizer->Add(m_pGrid, 1, wxALL, 0);
    wxBoxSizer* pBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pDeleteFileBtn = new wxButton(this, wxID_ANY, wxT("删除物理文件"));
    pBtnSizer->Add(m_pDeleteFileBtn, 0, wxALL, 5);
    m_pSizer->Add(pBtnSizer, 0, wxALL, 0);
    m_pDeleteFileBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CScanFileDialog::OnDeleteFileBtnClicked), NULL, this);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CScanFileDialog::OnClose), NULL, this);
}

CScanFileDialog::~CScanFileDialog()
{
    m_pDeleteFileBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CScanFileDialog::OnDeleteFileBtnClicked), NULL, this);
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CScanFileDialog::OnClose), NULL, this);
}

bool CScanFileDialog::Show(bool bShow)
{
    if (bShow)
    {
        ShowGridInfo();
    }
    return super::Show(bShow);
}

void CScanFileDialog::ShowGridInfo()
{
    if (m_pGrid->GetRows() > 0)
    {
        m_pGrid->DeleteRows(0, m_pGrid->GetRows());
    }
    if (m_pGrid->GetCols() > 0)
    {
        m_pGrid->DeleteCols(0, m_pGrid->GetCols());
    }
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    const std::set<TString>& exportFileFullPathList = pMainFrame->GetExportFileFullPathList();

    TString strCurrentPath = wxGetCwd();
    TString strResourcePath = strCurrentPath + _T("\\..\\Resource");
    TString strBCFResourcePath = strResourcePath + _T("\\bcf");
    TString strTexturePath = strResourcePath + _T("\\Texture");
    TString strETCPath = strTexturePath + _T("\\ETC_temp");
    TString strETCTexturePath = strETCPath + _T("\\Texture");
    TString strVersionInfoFile = strCurrentPath + _T("\\..\\VersionInfo.bin");
    wxArrayString files;
    wxDir::GetAllFiles(strResourcePath, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        wxFileName fn(files[i]);
        TString path = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_NO_SEPARATOR);
        if (path.find(strBCFResourcePath) == -1 && path.find(strTexturePath) == -1)
        {
            AddPackageFileFullPathList(files[i].mb_str());
        }
    }
    AddPackageFileFullPathList(strVersionInfoFile);
    files.Clear();
    wxDir::GetAllFiles(strETCTexturePath, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        AddPackageFileFullPathList(files[i].mb_str());
    }

    m_noUseFileFullPathList.clear();
    m_noUseLanguageList.clear();

    m_pGrid->AppendCols(1);
    m_pGrid->SetColLabelValue(0, _T("path"));
    uint32_t uRowCounter = 0;
    for (auto iter : m_packageFileFullPathList)
    {
        if (exportFileFullPathList.find(iter) == exportFileFullPathList.end())
        {
            m_pGrid->AppendRows();
            m_pGrid->SetCellValue(wxString::Format(_T("%s"), iter), uRowCounter, 0);
            uRowCounter++;
            m_noUseFileFullPathList.insert(iter);
        }
    }
    for (auto iter : CLanguageManager::GetInstance()->GetLanguageMap())
    {
        if (CStringHelper::GetInstance()->FindFirstString(iter.first.c_str(), "eLTT_Editor", false) == std::string::npos)
        {
            if (!CEnumStrGenerator::GetInstance()->IsLanguageEnumInvokeInCode(iter.first))
            {
                if (CLanguageManager::GetInstance()->GetExportLanguageList().find(iter.first) == CLanguageManager::GetInstance()->GetExportLanguageList().end())
                {
                    m_pGrid->AppendRows();
                    m_pGrid->SetCellValue(wxString::Format(_T("%s"), iter.first), uRowCounter, 0);
                    uRowCounter++;
                    m_noUseLanguageList.insert(iter.first);
                }
            }
        }
    }
    for (auto iter = CTextureAtlas::m_fragMissingInfo.begin(); iter != CTextureAtlas::m_fragMissingInfo.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            m_pGrid->AppendRows();
            m_pGrid->SetCellValue(wxString::Format(_T("Missing texture frag %s Location: %s"), iter->first.c_str(), subIter->c_str()), uRowCounter, 0);
            m_pGrid->SetCellBackgroundColour(*wxRED, uRowCounter, 0);
            uRowCounter++;
        }
    }
    for (auto iter = CTextureAtlas::m_fragCheckList.begin(); iter != CTextureAtlas::m_fragCheckList.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            if (subIter->second == 0)
            {
                m_pGrid->AppendRows();
                m_pGrid->SetCellValue(wxString::Format(_T("Uninvoked texture frag %s@%s"), CFilePathTool::GetInstance()->FileName(iter->first.c_str()), subIter->first.c_str()), uRowCounter, 0);
                uRowCounter++;
            }
        }
    }
    m_packageFileFullPathList.clear();
    CLanguageManager::GetInstance()->GetExportLanguageList().clear();
    CTextureAtlas::m_fragCheckList.clear();
    CTextureAtlas::m_fragMissingInfo.clear();
    m_pGrid->AutoSizeColumns();
}

void CScanFileDialog::OnClose(wxCloseEvent& /*event*/)
{
    Show(false);
}

void CScanFileDialog::AddPackageFileFullPathList(TString strFullPath)
{
    TCHAR szBufferPath[MAX_PATH];
    CFilePathTool::GetInstance()->Canonical(szBufferPath, strFullPath.c_str());
    CFilePathTool::GetInstance()->ConvertToWindowsPath(szBufferPath);
    strFullPath = szBufferPath;
    if (strFullPath.find("\\aidata.bin") == -1 && strFullPath.find("\\Language\\") == -1 && strFullPath.find("\\VersionInfo.bin") == -1 &&
        strFullPath.find("\\Particle\\") == -1 && strFullPath.find("\\Shader\\") == -1 && strFullPath.find("\\TerrainData\\") == -1 &&
        strFullPath.find("\\effect\\") == -1 && strFullPath.find("\\splash.png") == -1 && strFullPath.find("\\Script\\") == -1 &&
        strFullPath.find("\\LanguageConfig.bin") == -1 && strFullPath.find("\\Font\\") == -1 && strFullPath.find("\\LanguageConfig.xml") == -1 &&
        strFullPath.find("\\ExcelData\\") == -1 && strFullPath.find("\\temp_texture.png") == -1 &&
        strFullPath.find("\\number_") == -1 && strFullPath.find("\\Component.png") == -1 && strFullPath.find("\\water_normals.png") == -1 &&
        strFullPath.find("\\foam.png") == -1 && strFullPath.find("\\playerstar") == -1)
    {
        uint32_t uBegin = strFullPath.find("ETC_temp\\Texture\\");
        if (uBegin != -1)
        {
            strFullPath.erase(uBegin, TString("ETC_temp\\Texture\\").size());
        }
        std::transform(strFullPath.begin(), strFullPath.end(), strFullPath.begin(), tolower);
        m_packageFileFullPathList.insert(strFullPath);
    }
}

void CScanFileDialog::OnDeleteFileBtnClicked(wxCommandEvent& /*event*/)
{
    for (auto iter : m_noUseFileFullPathList)
    {
        ::DeleteFile(iter.c_str());
    }
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    for (auto iter : m_noUseLanguageList)
    {
        auto subIter = languageMap.find(iter);
        if (subIter != languageMap.end())
        {
            subIter = languageMap.erase(subIter);
        }
    }
    CLanguageManager::GetInstance()->SaveLanguageListToFile();

    wxMessageBox(wxString::Format(_T("删除完成")));
}
