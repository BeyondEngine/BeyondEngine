#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SETVERSIONDIALOG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SETVERSIONDIALOG_H__INCLUDE

#include "EditDialogBase.h"

class CSetVersionDialog : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CSetVersionDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CSetVersionDialog();
    wxString GetAppVersion();
    void SetAppVersion(wxString strAppVersion);
    wxString GetResVersion();
    void SetResVersion(wxString strResVersion);
    void SetConvertTypeText(wxString strType);
    wxString GetConvertTypeText();

protected:
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
private:
    wxTextCtrl* m_pAppVerTextCtrl;
    wxTextCtrl* m_pResVerTextCtrl;
    wxTextCtrl* m_pConverPNGTypeTextCtrl;
};

#endif