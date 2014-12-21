#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SCANFILEDIALOG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SCANFILEDIALOG_H__INCLUDE
#include "wx/generic/grid.h"

class CScanFileDialog : public wxDialog
{
    typedef wxDialog super;
public:
    CScanFileDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style = wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);
    virtual ~CScanFileDialog();
    virtual bool Show(bool bShow) override;
    void ShowGridInfo();
    void OnClose(wxCloseEvent& event);
    void AddPackageFileFullPathList(TString strFullPath);

private:
    void OnDeleteFileBtnClicked(wxCommandEvent& event);

private:
    wxBoxSizer*         m_pSizer;
    wxGrid*             m_pGrid;
    wxButton*       m_pDeleteFileBtn = nullptr;
    std::set<TString> m_packageFileFullPathList;
    std::set<TString> m_noUseFileFullPathList;
    std::set<TString> m_noUseLanguageList;
};

#endif