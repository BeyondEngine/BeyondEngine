#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_COMPONENTINFOWND_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_COMPONENTINFOWND_H__INCLUDE

#include "wx/generic/grid.h"
class wxSearchCtrl;

class CComponentInfoWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CComponentInfoWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CComponentInfoWnd();
    virtual bool Show(bool bShow = true) override;
    void ShowInstanceComponent(wxString szText = wxT(""));
    void ShowAllComponent(wxString szText = wxT(""));
    void ClearGrid();

private:
    void OnClose(wxCloseEvent& event);
    void OnViewChoiceChanged( wxCommandEvent& event );
    void OnRefreshBtnClicked(wxCommandEvent& event);
    void OnShowUnInvokedCheckBox(wxCommandEvent& event);
    void OnCellLeftClick(wxGridEvent& event);
    void OnCellLeftDclick(wxGridEvent& event);
    void OnGridColSort(wxGridEvent& event);
    void OnSrchIdle(wxIdleEvent& event);
    void OnSrchUpdate(wxCommandEvent& event);
private:
    wxGrid*         m_pGrid;
    wxChoice*       m_pViewChoice;
    wxButton*       m_pRefreshBtn;
    wxCheckBox*     m_pShowUnInvokedCheckBox;
    wxSearchCtrl*   m_pSrchCtrl;
    int             m_nCurrentChoice;
    unsigned long   m_uLastEnumSearchTextUpdateTime;
    bool            m_bEnumSearchTextUpdate;
    bool            m_bShowUnInvoked;

    DECLARE_EVENT_TABLE()
};


#endif