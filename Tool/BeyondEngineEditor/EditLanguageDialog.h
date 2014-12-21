#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditLanguageDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditLanguageDialog_H__INCLUDE
#include <wx/dialog.h>
#include "EditDialogBase.h"
#include "wx/aui/framemanager.h"

class wxGrid;
class wxSearchCtrl;
class wxGridEvent;
class CEditLanguageDialog : public CEditDialogBase
{
    enum
    {
        ID_LANGUAGE_GRID,
        ID_BUTTON_ADD,
        ID_BUTTON_EXPORT,
        ID_BUTTON_IMPORT,
        ID_BUTTON_SAVE,
        ID_SERACH_LANGUAGE,
    };
    typedef CEditDialogBase super;
public:
    CEditLanguageDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CEditLanguageDialog();
    virtual void LanguageSwitch();

    void InitChoiceCtrl();
    void InitLanguageGrid();

    void SearchEnum(TString str);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonExport(wxCommandEvent& event);
    void OnButtonImport(wxCommandEvent& event);
    void OnSearchLanguage(wxCommandEvent& event);
    void OnSearchEnterLanguange(wxCommandEvent& event);
    void OnCancleSrch(wxCommandEvent& event);
    void OnButtonSave(wxCommandEvent& event);
    void OnSrchUpdate(wxCommandEvent& event);
    void OnSrchIdle(wxIdleEvent& event);
    void OnMenuEvent(wxCommandEvent& event);
    void OnGridDataChanged(wxGridEvent& event);
    void OnRightClickOnGridLabel(wxGridEvent& event);

private:
    bool AddNewLanguageText(const TString& strEnum);
    bool RemoveLanguageText(const TString& strEnum, int nRow);
    bool ExamLanguageText(const TString& strEnum, bool bCheckExist);

private:
    wxGrid*     m_pLanguageGrid;
    wxButton*               m_pButtonAdd;
    wxButton*               m_pButtonExport;
    wxButton*               m_pButtonImport;
    wxButton*               m_pButtonSave;
    wxSearchCtrl*           m_pSrchCtrl;
    unsigned long           m_uLastEnumSearchTextUpdateTime;
    bool                    m_bEnumSearchTextUpdate;
DECLARE_EVENT_TABLE();
};

#endif