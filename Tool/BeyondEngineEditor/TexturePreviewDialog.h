#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_TEXTUREPREVIEWDIALOG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_TEXTUREPREVIEWDIALOG_H__INCLUDE

#include <wx/dialog.h>
#include <wx/listctrl.h>
#include "EditDialogBase.h"
#include "wx/event.h"

#define DefaultTexturePreviewIconSize 64

struct TexturePreviewInfo
{
    wxString fileName;
    wxString textureName;
    wxString with;
    wxString height;
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
    void LoadInfo();
    void InsertTextureToListCtrl();
    bool HasLoaded(wxString fileName);
    void LoadInfoFromFile(wxString& fileName);
    void ShowTextureInfo(int nIndex);
    const TexturePreviewInfo& GetTextureInfo(int nIndwx);

protected:
    void OnSize(wxSizeEvent& event);
    void OnSelected(wxListEvent& event);
    void OnDeselected(wxListEvent& event);
    void OnActivated(wxListEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

private:
    bool m_bInitialized;
    int m_nCurrentIndex;
    int m_nTexturePreviewIconSize;
    wxPanel* m_pPanel;
    wxListCtrl* m_pListCtrl;
    wxStaticText* m_pInfoText;
    wxImageList* m_pTexturePreviewIconList;
    std::vector<TexturePreviewInfo> m_texturePreviewInfoList;

DECLARE_EVENT_TABLE();
};

#endif