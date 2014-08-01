#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_ABOUTDLG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_ABOUTDLG_H__INCLUDE

#include "EditDialogBase.h"

class CAboutDlg : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CAboutDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CAboutDlg();

    virtual int ShowModal() override;

private:
    void OnShowAfterLaunchCheckBoxClick(wxCommandEvent& event);

private:
    wxPanel* m_pBackgroundPanel;
    wxTextCtrl* m_pTextCtrl;
    wxCheckBox* m_pShowAfterLaunchCheckBox;
};

#endif