#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_TEXTUREPREVIEWDIALOG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_TEXTUREPREVIEWDIALOG_H__INCLUDE

#include <wx/dialog.h>
#include <wx/listctrl.h>
#include "EditDialogBase.h"
#include "wx/event.h"
class wxSearchCtrl;

#define DefaultTexturePreviewIconSize 64

struct STexturePreviewInfo
{
    wxString fileName;
    wxString textureName;
    uint32_t m_uWidth;
    uint32_t m_uHeight;
    uint32_t m_uImageIndex;
};

class CTexturePreviewDialog : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CTexturePreviewDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CTexturePreviewDialog();
    virtual int ShowModal() override;

    void InitTexture();
    void InitCtrls();
    void RefreshListCtrl();
    void LoadingThread();
    void LoadInfoFromFile(wxString& fileName);
    void ShowTextureInfo(int nIndex);
    const STexturePreviewInfo* GetTextureInfo(int nIndex) const;
    const STexturePreviewInfo* GetTextureInfo(const TCHAR* pszFileName, const TCHAR* pszTextureName);
    int GetCurrentIndex() const;
    void SetCurrentImage(wxString fileName, wxString imageName);

protected:
    void OnSelectedImage(wxListEvent& event);
    void OnDeselectedImage(wxListEvent& event);
    void OnActivated(wxListEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnFileFilterSelect(wxListEvent& event);
    void LaunchLoadThread();
    void OnSrchIdle(wxIdleEvent& event);
    void OnSrchUpdate(wxCommandEvent& event);

private:
    int m_nCurrentSelectImageIndex;
    int m_nTexturePreviewIconSize;
    unsigned long m_uLastEnumSearchTextUpdateTime = 0;
    bool m_bEnumSearchTextUpdate = false;
    std::mutex m_textureInfoFilesLock;
    std::mutex m_insertDataLock;
    std::thread m_textureLoadingThread1;
    std::thread m_textureLoadingThread2;
    std::thread m_textureLoadingThread3;
    std::thread m_textureLoadingThread4;
    wxCheckListBox* m_pFileListBox;
    wxListCtrl* m_pImageListCtrl;
    wxStaticText* m_pInfoText;
    wxImageList* m_pTexturePreviewIconList;
    wxSearchCtrl* m_pSrchCtrl;
    // First key is file name, second key is texture name.
    std::map<wxString, std::map<wxString, STexturePreviewInfo*>> m_textureInfoMap;
    std::vector<wxString> m_textureInfoFiles;
    std::set<wxString> m_compressFiles;
    DECLARE_EVENT_TABLE();
};

#endif