#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SearchComponentDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SearchComponentDialog_H__INCLUDE

#include <wx/dialog.h>
#include <wx/listctrl.h>
class wxChoice;
class wxTextCtrl;
class wxButton;

class CSearchComponentDialog : public wxDialog
{
    enum
    {
        ID_TextCtrl_Input,
    };
    typedef wxDialog super;
public:
    CSearchComponentDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style = wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CSearchComponentDialog();

private:
    void OnViewChoiceChanged(wxCommandEvent& event);
    void OnSearchBtnClicked(wxCommandEvent& event);
    void OnFileFilterSelect(wxListEvent& event);
    void SearchComponent();
    void OnKeyClicked(wxKeyEvent& event);

private:
    wxChoice* m_pSearchViewChoice = nullptr;
    wxTextCtrl* m_pInputText = nullptr;
    wxButton* m_pSearchBtn = nullptr;
    wxCheckListBox* m_pComponentListBox = nullptr;
    int m_nCurrentChoice = 0;
    std::map<wxString, uint32_t> m_componentIdMap;
    DECLARE_EVENT_TABLE();
};

#endif