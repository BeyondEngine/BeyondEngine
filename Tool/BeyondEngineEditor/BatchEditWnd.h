#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_BATCHEDITWND_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_BATCHEDITWND_H__INCLUDE

class CBatchEditWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CBatchEditWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CBatchEditWnd();

    virtual bool Show(bool bShow = true) override;

private:
    void OnClose(wxCloseEvent& event);
    void OnApplyBtnClicked(wxCommandEvent& event);

private:
    wxChoice* m_pOperatingRange;
    wxTextCtrl* m_pGuidTextCtrl;
    wxTextCtrl* m_pPropertyNameCtrl;
    wxTextCtrl* m_pNewValueCtrl;
    wxCheckBox* m_pApplyToDerivedCheckBox;
    wxButton* m_pApplyBtn;
};


#endif