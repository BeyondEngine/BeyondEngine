#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_CustomResolutionDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_CustomResolutionDialog_H__INCLUDE
#include <wx/dialog.h>
class wxListBox;
class wxButton;

class CCustomResolutionDialog : public wxDialog
{
    typedef wxDialog super;
public:
    CCustomResolutionDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style = wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);
    ~CCustomResolutionDialog();
    void InitResolutonListBox();
    void OnClose(wxCloseEvent& event);

protected:
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
private:
    wxListBox*  m_pResolutionListBox;
};
#endif