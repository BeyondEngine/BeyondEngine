#include "stdafx.h"
#include "TexturePreviewDialog.h"
#include "EngineEditor.h"
#include "Resource/ResourceManager.h"
#include "wx/filesys.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "wx/dir.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"
#include <wx/srchctrl.h>

enum ECtrlID
{
    IMAGE_LIST_CTRL = 2222,
    FILE_LIST_CTRL,
    BUTTON_OK,
    BUTTON_CANCEL,
};

BEGIN_EVENT_TABLE(CTexturePreviewDialog, wxDialog)
    EVT_BUTTON(BUTTON_OK, CTexturePreviewDialog::OnOK)
    EVT_BUTTON(BUTTON_CANCEL, CTexturePreviewDialog::OnCancel)
    EVT_LIST_ITEM_SELECTED(IMAGE_LIST_CTRL, CTexturePreviewDialog::OnSelectedImage)
    EVT_LIST_ITEM_DESELECTED(IMAGE_LIST_CTRL, CTexturePreviewDialog::OnDeselectedImage)
    EVT_LIST_ITEM_ACTIVATED(IMAGE_LIST_CTRL, CTexturePreviewDialog::OnActivated)
    EVT_IDLE(CTexturePreviewDialog::OnSrchIdle)
END_EVENT_TABLE()

CTexturePreviewDialog::CTexturePreviewDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_nCurrentSelectImageIndex(INVALID_DATA)
    , m_pFileListBox(NULL)
    , m_pImageListCtrl(NULL)
    , m_pSrchCtrl(NULL)
    , m_nTexturePreviewIconSize(DefaultTexturePreviewIconSize)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_pTexturePreviewIconList = new wxImageList(m_nTexturePreviewIconSize, m_nTexturePreviewIconSize, true);
    InitCtrls();
    InitTexture();
    SetSize(800, 600);
}

int CTexturePreviewDialog::ShowModal()
{
    int nRet = wxID_CANCEL;
    RefreshListCtrl();
    CenterOnScreen();
    nRet = wxDialog::ShowModal();
    return nRet;
}

CTexturePreviewDialog::~CTexturePreviewDialog()
{
    m_pSrchCtrl->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CTexturePreviewDialog::OnSrchUpdate), NULL, this);
    BEATS_SAFE_DELETE(m_pTexturePreviewIconList);
    for (auto iter = m_textureInfoMap.begin(); iter != m_textureInfoMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            BEATS_SAFE_DELETE(subIter->second);
        }
    }
}

void CTexturePreviewDialog::InitTexture()
{
    CSerializer configData;
    std::vector<std::string> fileList;
    TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    strWorkingPath.append(_T("\\")).append(COMPRESS_TEXTURE_CONFIG_NAME);
	if (CFilePathTool::GetInstance()->Exists(strWorkingPath.c_str()) && CFilePathTool::GetInstance()->LoadFile(&configData, strWorkingPath.c_str(), "rt") && configData.GetWritePos() > 0)
    {
        TCHAR szBuffer[10240];
        configData.Deserialize(szBuffer, configData.GetWritePos());
        szBuffer[configData.GetWritePos()] = 0;
        BEATS_ASSERT(configData.GetReadPos() == configData.GetWritePos());
        CStringHelper::GetInstance()->SplitString(szBuffer, "\n", fileList, true);
    }
    wxString texturePath = CStringHelper::GetInstance()->ToLower(CResourceManager::GetInstance()->GetResourcePath(eRT_Texture));
    m_textureInfoFiles.clear();
    wxString fileName;
    wxDir textureDir(texturePath);
    bool cont = textureDir.GetFirst(&fileName, _T("*.xml"), wxDIR_FILES);
    fileName = CStringHelper::GetInstance()->ToLower((TString)fileName);
    while (cont)
    {
        TString strFilePath = texturePath + _T("\\") + fileName;
        m_textureInfoFiles.push_back(strFilePath);
        m_pFileListBox->AppendString(fileName);
        bool bNeedCompress = std::find(fileList.begin(), fileList.end(), fileName) != fileList.end();
        m_pFileListBox->Check(m_pFileListBox->GetCount() - 1, bNeedCompress);
        cont = textureDir.GetNext(&fileName);
        fileName = CStringHelper::GetInstance()->ToLower((TString)fileName);
    }
}

void CTexturePreviewDialog::InitCtrls()
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pFunctionAreaSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pInfoSizer = new wxBoxSizer(wxVERTICAL);
    m_pInfoText = new wxStaticText(this, wxID_ANY, _T("Information :"), wxDefaultPosition, wxSize(0, 90));
    pInfoSizer->Add(m_pInfoText, 1, wxGROW|wxALL, 0);

    m_pSrchCtrl = new wxSearchCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_pSrchCtrl->ShowCancelButton(true);
    m_pSrchCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CTexturePreviewDialog::OnSrchUpdate), NULL, this);
    pInfoSizer->Add(m_pSrchCtrl, 0, wxALIGN_CENTER | wxDOWN, 10);

    pFunctionAreaSizer->Add(pInfoSizer, 1,  wxGROW|wxALL, 0);

    wxBoxSizer* pRightSizer = new wxBoxSizer(wxVERTICAL);
    m_pFileListBox = new wxCheckListBox(this, FILE_LIST_CTRL, wxDefaultPosition, wxDefaultSize, 0, 0, wxLC_SINGLE_SEL);
    m_pFileListBox->AppendString(_T("All files"));
    pRightSizer->Add(m_pFileListBox, 1, wxGROW | wxALL, 5);

    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* pButtonOK = new wxButton(this, BUTTON_OK, _T("OK"));
    wxButton* pButtonCancel = new wxButton(this, BUTTON_CANCEL, _T("Cancel"));
    pButtonSizer->Add(pButtonOK, 1, wxALL, 5);
    pButtonSizer->Add(pButtonCancel, 1, wxALL, 5);
    pRightSizer->Add(pButtonSizer, 0, wxALL, 5);

    pFunctionAreaSizer->Add(pRightSizer, 0,  wxGROW|wxALL, 0);

    m_pImageListCtrl = new wxListCtrl(this, IMAGE_LIST_CTRL, wxDefaultPosition, wxDefaultSize, wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxLC_ICON);
    m_pImageListCtrl->SetImageList(m_pTexturePreviewIconList, wxIMAGE_LIST_NORMAL);
    wxSize size = GetClientSize();
    wxCoord y = (2 * size.y) / 3;
    m_pImageListCtrl->SetSize(0, 0, size.x, y);

    pSizer->Add(m_pImageListCtrl, 1, wxGROW|wxALL, 0);
    pSizer->Add(pFunctionAreaSizer, 0, wxGROW|wxALL, 0);
    SetSizer(pSizer);
    m_pFileListBox->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxListEventHandler(CTexturePreviewDialog::OnFileFilterSelect), NULL, this);
}

void CTexturePreviewDialog::LoadInfoFromFile(wxString& fileName)
{
    wxString path, simpleName, ext;
    wxFileName::SplitPath(fileName, &path, &simpleName, &ext);
    rapidxml::file<> fdoc(fileName);
    rapidxml::xml_document<> doc;
    try
    {
        doc.parse<rapidxml::parse_default>(fdoc.data());
    }
    catch (rapidxml::parse_error err)
    {
        BEATS_ASSERT(false, _T("Load config file %s faled!/n%s/n"), fileName, err.what());
    }
    rapidxml::xml_node<> *root = doc.first_node("Imageset");
    BEATS_ASSERT(root != nullptr);
    const char *textureFileName = root->first_attribute("Imagefile")->value();
    BEATS_ASSERT(textureFileName != NULL);
    path += _T("/");

    wxBitmap texture;
    bool bRet = texture.LoadFile(path + textureFileName, wxBITMAP_TYPE_PNG);
    BEATS_ASSERT(bRet, "Load png file %s failed!", TString(fileName).c_str());
    if (bRet)
    {
        for (rapidxml::xml_node<> *elemImage = root->first_node("Image");
            elemImage != nullptr; elemImage = elemImage->next_sibling("Image"))
        {
            TString strTextureName = CStringHelper::GetInstance()->ToLower(elemImage->first_attribute("Name")->value());
            BEATS_ASSERT(!strTextureName.empty());
            CVec2 point = CVec2(0.f, 0.f);
            CVec2 size = CVec2(0.f, 0.f);
            point.X() = (float)_tstof(elemImage->first_attribute("XPos")->value());
            point.Y() = (float)_tstof(elemImage->first_attribute("YPos")->value());
            size.X() = (float)_tstof(elemImage->first_attribute("Width")->value());
            size.Y() = (float)_tstof(elemImage->first_attribute("Height")->value());

            wxRect rect(point.X(), point.Y(), size.X(), size.Y());
            wxIcon icon;
            BEATS_ASSERT(rect.x + rect.width <= texture.GetWidth() && rect.y + rect.height <= texture.GetHeight());
            icon.CopyFromBitmap(texture.GetSubBitmap(rect));
            STexturePreviewInfo* pInfo = new STexturePreviewInfo;
            wxString strFileName = simpleName + _T(".") + ext;
            uint32_t uImageIndex = 0xFFFFFFFF;
            {
                std::lock_guard<std::mutex> locker(m_insertDataLock);
                m_pTexturePreviewIconList->Add(icon);
                uImageIndex = m_pTexturePreviewIconList->GetImageCount() - 1;
                BEATS_ASSERT(m_textureInfoMap[strFileName].find(strTextureName) == m_textureInfoMap[strFileName].end());
                m_textureInfoMap[strFileName][strTextureName] = pInfo;
            }
            pInfo->fileName = strFileName;
            BEATS_ASSERT(CStringHelper::GetInstance()->FindFirstString(strFileName.c_str(), " ", false) == TString::npos, "file name should not contain space in %s", strFileName.c_str());
            pInfo->textureName = strTextureName;
            BEATS_ASSERT(CStringHelper::GetInstance()->FindFirstString(strTextureName.c_str(), " ", false) == TString::npos, "Texture name should not contain space in %s", strTextureName.c_str());
            pInfo->m_uWidth = (uint32_t)rect.GetWidth();
            pInfo->m_uHeight = (uint32_t)rect.GetHeight();
            pInfo->m_uImageIndex = uImageIndex;
        }
    }
}

void CTexturePreviewDialog::OnSelectedImage(wxListEvent& event)
{
    m_nCurrentSelectImageIndex = event.GetImage();
    ShowTextureInfo(m_nCurrentSelectImageIndex);
}

void CTexturePreviewDialog::OnActivated(wxListEvent& event)
{
    m_nCurrentSelectImageIndex = event.GetImage();
    EndModal(wxID_OK);
}

void CTexturePreviewDialog::OnOK(wxCommandEvent& /*event*/)
{
    wxArrayInt checkedItems;
    m_pFileListBox->GetCheckedItems(checkedItems);
    TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    strWorkingPath.append(_T("\\")).append(COMPRESS_TEXTURE_CONFIG_NAME);

    CSerializer configData;
    for (size_t i = 0; i < checkedItems.size(); ++i)
    {
        TString strCheckedString = m_pFileListBox->GetString(checkedItems[i]);
        strCheckedString.append("\n");
        configData.Serialize((void*)strCheckedString.c_str(), strCheckedString.length());
    }
    configData.Deserialize(strWorkingPath.c_str(), "wt+");
    EndModal(wxID_OK);
}

void CTexturePreviewDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void CTexturePreviewDialog::OnFileFilterSelect(wxListEvent& /*event*/)
{
    m_pSrchCtrl->SetValue(_T(""));
    m_bEnumSearchTextUpdate = false;
    m_nCurrentSelectImageIndex = INVALID_DATA;
    RefreshListCtrl();
}

void CTexturePreviewDialog::LaunchLoadThread()
{
    m_textureLoadingThread1 = std::thread(&CTexturePreviewDialog::LoadingThread, this);
    m_textureLoadingThread2 = std::thread(&CTexturePreviewDialog::LoadingThread, this);
    m_textureLoadingThread3 = std::thread(&CTexturePreviewDialog::LoadingThread, this);
    m_textureLoadingThread4 = std::thread(&CTexturePreviewDialog::LoadingThread, this);
    m_textureLoadingThread1.join();
    m_textureLoadingThread2.join();
    m_textureLoadingThread3.join();
    m_textureLoadingThread4.join();
}

void CTexturePreviewDialog::ShowTextureInfo(int nIndex)
{
    wxString text = _T("Information : \n");
    if (nIndex == INVALID_DATA)
    {
        text += _T("...");
    }
    else
    {
        const STexturePreviewInfo* pInfo = GetTextureInfo(nIndex);
        BEATS_ASSERT(pInfo != NULL);
        text += wxString::Format(_T("file : %s \n"), pInfo->fileName.c_str());
        text += wxString::Format(_T("name : %s \n"), pInfo->textureName.c_str());
        text += wxString::Format(_T("width : %d \n"), pInfo->m_uWidth);
        text += wxString::Format(_T("height : %d \n"), pInfo->m_uHeight);
    }
    m_pInfoText->SetLabelText(text);
    Layout();
}

void CTexturePreviewDialog::OnDeselectedImage(wxListEvent& /*event*/)
{
    m_nCurrentSelectImageIndex = INVALID_DATA;
    ShowTextureInfo(INVALID_DATA);
}

const STexturePreviewInfo* CTexturePreviewDialog::GetTextureInfo(int nIndex) const
{
    STexturePreviewInfo* pInfo = NULL;
    int nCount = m_pImageListCtrl->GetItemCount();
    for (int i = 0; i < nCount; i++)
    {
        STexturePreviewInfo* pData = (STexturePreviewInfo*)m_pImageListCtrl->GetItemData(i);
        if ((int)pData->m_uImageIndex == nIndex)
        {
            pInfo = pData;
            break;
        }
    }
    return pInfo;
}

const STexturePreviewInfo* CTexturePreviewDialog::GetTextureInfo(const TCHAR* pszFileName, const TCHAR* pszTextureName)
{
    STexturePreviewInfo* pRet = NULL;
    auto iter = m_textureInfoMap.find(pszFileName);
    if (iter == m_textureInfoMap.end())
    {
        wxString strFullPath;
        {
            std::lock_guard<std::mutex> locker(m_textureInfoFilesLock);
            for (auto infoIter = m_textureInfoFiles.begin(); infoIter != m_textureInfoFiles.end(); ++infoIter)
            {
                if (CFilePathTool::GetInstance()->FileName(*infoIter) == pszFileName)
                {
                    strFullPath = *infoIter;
                    m_textureInfoFiles.erase(infoIter);
                    break;
                }
            }
        }
        BEATS_ASSERT(!strFullPath.empty());
        LoadInfoFromFile(strFullPath);
        iter = m_textureInfoMap.find(pszFileName);
    }
    BEATS_ASSERT(iter != m_textureInfoMap.end());
    auto subIter = iter->second.find(pszTextureName);
    if (subIter != iter->second.end())
    {
        pRet = subIter->second;
    }
    return pRet;
}

int CTexturePreviewDialog::GetCurrentIndex() const
{
    return m_nCurrentSelectImageIndex;
}

void CTexturePreviewDialog::SetCurrentImage(wxString fileName, wxString imageName)
{
    if (!fileName.empty() && m_textureInfoMap.find(fileName) != m_textureInfoMap.end())
    {
        std::map<wxString, STexturePreviewInfo*>& imageMap = m_textureInfoMap[fileName];
        BEATS_ASSERT(imageMap.find(imageName) != imageMap.end());
        for (size_t i = 0; i < m_pFileListBox->GetCount(); ++i)
        {
            if (m_pFileListBox->GetString(i) == fileName)
            {
                m_pFileListBox->Select(i);
                break;
            }
        }
        m_nCurrentSelectImageIndex = imageMap[imageName]->m_uImageIndex;
    }
    else
    {
        m_nCurrentSelectImageIndex = INVALID_DATA;
    }
}

void CTexturePreviewDialog::RefreshListCtrl()
{
    wxString strCurrentSelectFileName = m_pFileListBox->GetStringSelection();
    m_pImageListCtrl->ClearAll();
    if (!strCurrentSelectFileName.empty())
    {
        wxString szSrchText = m_pSrchCtrl->GetValue().Lower();
        if (strCurrentSelectFileName == _T("All files"))
        {
            if (m_textureInfoFiles.size() > 0)
            {
                LaunchLoadThread();
            }
            for (auto iter = m_textureInfoMap.begin(); iter != m_textureInfoMap.end(); ++iter)
            {
                for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
                {
                    wxListItem listItem;
                    listItem.SetData(subIter->second);
                    listItem.SetImage(subIter->second->m_uImageIndex);
                    listItem.SetText(subIter->second->textureName);
                    listItem.SetId(subIter->second->m_uImageIndex);
                    bool bMatch = subIter->second->textureName.Lower().find(szSrchText.utf8_str()) != -1;
                    if (bMatch)
                    {
                        m_pImageListCtrl->InsertItem(listItem);
                    }
                }
            }
        }
        else
        {
            auto iter = m_textureInfoMap.find(strCurrentSelectFileName);
            if (iter == m_textureInfoMap.end())
            {
                wxString strFullPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Texture) + "\\" + strCurrentSelectFileName;
                strFullPath = CStringHelper::GetInstance()->ToLower((TString)strFullPath);
                {
                    std::lock_guard<std::mutex> locker(m_textureInfoFilesLock);
                    auto fileIter = std::find(m_textureInfoFiles.begin(), m_textureInfoFiles.end(), strFullPath);
                    BEATS_ASSERT(fileIter != m_textureInfoFiles.end());
                    m_textureInfoFiles.erase(fileIter);
                }
                LoadInfoFromFile(strFullPath);
                iter = m_textureInfoMap.find(strCurrentSelectFileName);
            }
            BEATS_ASSERT(iter != m_textureInfoMap.end());
            for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
            {
                wxListItem listItem;
                listItem.SetData(subIter->second);
                listItem.SetImage(subIter->second->m_uImageIndex);
                listItem.SetText(subIter->second->textureName);
                listItem.SetId(subIter->second->m_uImageIndex);
                bool bMatch = subIter->second->textureName.Lower().find(szSrchText.utf8_str()) != -1;
                long currIndex = 0;
                if (bMatch)
                {
                    currIndex = m_pImageListCtrl->InsertItem(listItem);
                }
                if (m_nCurrentSelectImageIndex == (int32_t)subIter->second->m_uImageIndex)
                {
                    m_pImageListCtrl->SetFocus();
                    m_pImageListCtrl->SetItemState(currIndex, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, 0xFFFFFFFF);
                    m_pImageListCtrl->EnsureVisible(currIndex);
                }
            }
        }
    }
}

void CTexturePreviewDialog::LoadingThread()
{
    while (true)
    {
        wxString strFileName;
        {
            std::lock_guard<std::mutex> locker(m_textureInfoFilesLock);
            if (m_textureInfoFiles.size() > 0)
            {
                strFileName = m_textureInfoFiles.back();
                m_textureInfoFiles.pop_back();
            }
            else
            {
                break;
            }
        }
        if (strFileName != wxEmptyString)
        {
            LoadInfoFromFile(strFileName);
        }
    }
}

void CTexturePreviewDialog::OnSrchIdle(wxIdleEvent& /*event*/)
{
    if (m_bEnumSearchTextUpdate && GetTickCount() - m_uLastEnumSearchTextUpdateTime > 700)
    {
        m_bEnumSearchTextUpdate = false;
        RefreshListCtrl();
    }
}

void CTexturePreviewDialog::OnSrchUpdate(wxCommandEvent& /*event*/)
{
    m_uLastEnumSearchTextUpdateTime = GetTickCount();
    m_bEnumSearchTextUpdate = true;
}
