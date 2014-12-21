#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SelectResolutionDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SelectResolutionDialog_H__INCLUDE
#include <wx/dialog.h>
class CEditorMainFrame;
class CCustomResolutionDialog;

class CSelectResolutionDialog :public wxDialog
{
    typedef wxDialog super;
public:
    CSelectResolutionDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style = wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);
    virtual ~CSelectResolutionDialog();
    void InitSelectResolutionDialog();
    void SetResolution(uint32_t uWidth, uint32_t uHeight);
protected:
    void OnClose(wxCloseEvent& event);
    void OnResolutionButton(wxCommandEvent& event);
    void OnButtonCustom(wxCommandEvent& event);
private:
    bool                        m_bSetResolution;
    wxBoxSizer*                 m_pSizer;
    CEditorMainFrame*           m_pMainFrame;
};
#endif