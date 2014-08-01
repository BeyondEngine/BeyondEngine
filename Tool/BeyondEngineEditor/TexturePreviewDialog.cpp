#include "stdafx.h"
#include "TexturePreviewDialog.h"
#include "EngineEditor.h"
#include "Resource\ResourcePathManager.h"
#include "wx\filesys.h"
#include "TinyXML\tinyxml.h"
#include "Utility\BeatsUtility\StringHelper.h"


enum ECtrlID
{
    LIST_CTRL = 2222,
    BUTTON_OK,
    BUTTON_CANCEL,
};

BEGIN_EVENT_TABLE(CTexturePreviewDialog, wxDialog)
    EVT_SIZE(CTexturePreviewDialog::OnSize)
    EVT_BUTTON(BUTTON_OK, CTexturePreviewDialog::OnOK)
    EVT_BUTTON(BUTTON_CANCEL, CTexturePreviewDialog::OnCancel)
    EVT_LIST_ITEM_SELECTED(LIST_CTRL, CTexturePreviewDialog::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LIST_CTRL, CTexturePreviewDialog::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(LIST_CTRL, CTexturePreviewDialog::OnActivated)
END_EVENT_TABLE()

CTexturePreviewDialog::CTexturePreviewDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_bInitialized(false)
    , m_nCurrentIndex(INVALID_DATA)
    , m_pListCtrl(NULL)
    , m_pPanel(NULL)
    , m_nTexturePreviewIconSize(DefaultTexturePreviewIconSize)
{
    std::thread thread(std::bind(&CTexturePreviewDialog::InitTexture, this));
    thread.detach();

    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_pTexturePreviewIconList = new wxImageList(m_nTexturePreviewIconSize, m_nTexturePreviewIconSize, true);
    InitCtrls();
}

int CTexturePreviewDialog::ShowModal()
{
    if (m_bInitialized)
    {
        LoadInfo();//check new
        CenterOnScreen();
        //TODO:set the last selection, but it may doesn't work
        m_pListCtrl->SetItemState(m_nCurrentIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        if (wxDialog::ShowModal() == wxID_CANCEL)
        {
            m_nCurrentIndex = INVALID_DATA;
        }
    }
    else
    {
        wxMessageBox(_T("in initialization please wait a moment"));
    }

    return m_nCurrentIndex;
}

CTexturePreviewDialog::~CTexturePreviewDialog()
{
    BEATS_SAFE_DELETE(m_pTexturePreviewIconList);
}

void CTexturePreviewDialog::InitTexture()
{
    LoadInfo();
    m_bInitialized = true;
}

void CTexturePreviewDialog::InitCtrls()
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pPanel = new wxPanel(this);
    m_pInfoText = new wxStaticText(m_pPanel, wxID_ANY, _T("Information :"), wxDefaultPosition, wxSize(0, 90));
    wxButton* pButtonOK = new wxButton(m_pPanel, BUTTON_OK, _T("OK"));
    wxButton* pButtonCancel = new wxButton(m_pPanel, BUTTON_CANCEL, _T("Cancel"));
    
    m_pListCtrl = new wxListCtrl(this, LIST_CTRL, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxBORDER_THEME);
    m_pListCtrl->SetImageList(m_pTexturePreviewIconList, wxIMAGE_LIST_NORMAL);
    wxSize size = GetClientSize();
    wxCoord y = (2*size.y)/3;
    m_pListCtrl->SetSize(0, 0, size.x, y);

    pButtonSizer->Add(m_pInfoText, 1, wxGROW|wxALL, 0);
    pButtonSizer->Add(pButtonOK, 0, wxGROW|wxALL, 0);
    pButtonSizer->Add(pButtonCancel, 0, wxGROW|wxALL, 0);
    m_pPanel->SetSizer(pButtonSizer);

    pSizer->Add(m_pListCtrl, 1, wxGROW|wxALL, 0);
    pSizer->Add(m_pPanel, 0, wxGROW|wxALL, 0);
    SetSizer(pSizer);
}

void CTexturePreviewDialog::LoadInfo()
{
    wxFileSystem fileSystem;
    wxString texturePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Texture);
    fileSystem.ChangePathTo(texturePath, true);
    std::vector<wxString> files;
    wxString fileName = fileSystem.FindFirst(_T("*.xml"));
    while (!fileName.IsNull())
    {
        files.push_back(fileSystem.URLToFileName(fileName).GetFullPath());
        fileName = fileSystem.FindNext();
    }

    for (auto itr : files)
    {
        if (!HasLoaded(itr))
        {
            LoadInfoFromFile(itr);
        }
    }
}

void CTexturePreviewDialog::LoadInfoFromFile(wxString& fileName)
{
    wxString path, simpleName, ext;
    wxFileName::SplitPath(fileName, &path, &simpleName, &ext);
    TiXmlDocument doc;
    doc.LoadFile(fileName);
    TiXmlElement *root = doc.RootElement();
    BEATS_ASSERT(root && strcmp(root->Value(), "Imageset") == 0);
    const char *textureFileName = root->Attribute("Imagefile");
    BEATS_ASSERT(textureFileName != NULL);
    path += _T("\\");

    wxBitmap texture;
    {
        //suppress the warning from loading png
        wxLogNull logNo;
        texture.LoadFile(path + textureFileName, wxBITMAP_TYPE_PNG);
    }

    for(TiXmlElement *elemImage = root->FirstChildElement("Image");
        elemImage != nullptr; elemImage = elemImage->NextSiblingElement("Image"))
    {
        const char *textureName = elemImage->Attribute("Name");
        BEATS_ASSERT(textureName);
        kmVec2 point;
        kmVec2Fill(&point, 0.f, 0.f);
        kmVec2 size;
        kmVec2Fill(&size, 0.f, 0.f);
        elemImage->QueryFloatAttribute("XPos", &point.x);
        elemImage->QueryFloatAttribute("YPos", &point.y);
        elemImage->QueryFloatAttribute("Width", &size.x);
        elemImage->QueryFloatAttribute("Height", &size.y);

        wxRect rect(point.x, point.y, size.x, size.y);
        wxIcon icon;
        icon.CopyFromBitmap(texture.GetSubBitmap(rect));
        m_pTexturePreviewIconList->Add(icon);
        TexturePreviewInfo info;
        info.fileName = simpleName + _T(".") + ext;
        info.textureName = textureName;
        info.with.Printf(_T("%d"), rect.GetWidth());
        info.height.Printf(_T("%d"), rect.GetHeight());
        m_texturePreviewInfoList.push_back(info);
    }

    BEATS_ASSERT(m_pTexturePreviewIconList->GetImageCount() == (int)m_texturePreviewInfoList.size());
}

void CTexturePreviewDialog::OnSelected(wxListEvent& event)
{
    m_nCurrentIndex = event.GetIndex();
    ShowTextureInfo(m_nCurrentIndex);
}

void CTexturePreviewDialog::OnActivated(wxListEvent& event)
{
    m_nCurrentIndex = event.GetIndex();
    EndModal(wxID_OK);
}

void CTexturePreviewDialog::OnOK(wxCommandEvent& /*event*/)
{
    if (m_nCurrentIndex == INVALID_DATA)
    {
        wxMessageBox(_T("no one is selected"));
    }
    else
    {
        EndModal(wxID_OK);
    }
}

void CTexturePreviewDialog::OnCancel(wxCommandEvent& /*event*/)
{
    m_nCurrentIndex = INVALID_DATA;
    EndModal(wxID_CANCEL);
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
        BEATS_ASSERT(nIndex < (int)m_texturePreviewInfoList.size());
        TexturePreviewInfo info = m_texturePreviewInfoList[nIndex];
        text += wxString::Format(_T("file : %s \n"), info.fileName.c_str());
        text += wxString::Format(_T("name : %s \n"), info.textureName.c_str());
        text += wxString::Format(_T("width : %s \n"), info.with.c_str());
        text += wxString::Format(_T("height : %s \n"), info.height.c_str());
    }
    m_pInfoText->SetLabelText(text);
}

void CTexturePreviewDialog::OnDeselected(wxListEvent& /*event*/)
{
    ShowTextureInfo(INVALID_DATA);
}

const TexturePreviewInfo& CTexturePreviewDialog::GetTextureInfo(int nIndwx)
{
    return m_texturePreviewInfoList[nIndwx];
}

void CTexturePreviewDialog::InsertTextureToListCtrl()
{
    m_pListCtrl->ClearAll();
    int nTextureCount = m_pTexturePreviewIconList->GetImageCount();
    wxString label;
    for ( int i = 0; i < nTextureCount; i++ )
    {
        label = m_texturePreviewInfoList[i].textureName;
        m_pListCtrl->InsertItem(i, label, i);
    }
}

bool CTexturePreviewDialog::HasLoaded(wxString fileName)
{
    bool bNeedLoad = false;
    wxString path, simpleName, ext;
    wxFileName::SplitPath(fileName, &path, &simpleName, &ext);
    for(auto infoItr : m_texturePreviewInfoList)
    {
        if (infoItr.fileName == simpleName + _T(".") + ext)
        {
            bNeedLoad = true;
            break;
        }
    }
    return bNeedLoad;
}

void CTexturePreviewDialog::OnSize(wxSizeEvent& event)
{
    wxSizer* pSizer = GetSizer();
    pSizer->DeleteWindows();
    InitCtrls();
    InsertTextureToListCtrl();
    if (m_nCurrentIndex != INVALID_DATA)
    {
        m_pListCtrl->SetItemState(m_nCurrentIndex, 1, wxLIST_STATE_SELECTED);
        ShowTextureInfo(m_nCurrentIndex);
    }
    event.Skip();
}

