#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_COMPONENTINFOWND_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_COMPONENTINFOWND_H__INCLUDE

class wxGrid;
class CComponentInfoWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CComponentInfoWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CComponentInfoWnd();
    virtual bool Show(bool bShow = true) override;

private:
    void OnClose(wxCloseEvent& event);
    void OnViewChoiceChanged( wxCommandEvent& event );
    void OnRefreshBtnClicked(wxCommandEvent& event);
private:
    wxGrid* m_pGrid;
    wxChoice* m_pViewChoice;
    wxButton* m_pRefreshBtn;
    int m_nCurrentChoice;

};


#endif