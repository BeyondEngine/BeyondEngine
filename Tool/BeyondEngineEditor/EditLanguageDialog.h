#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditLanguageDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditLanguageDialog_H__INCLUDE
#include <wx/dialog.h>
#include "EditDialogBase.h"
#include "wx/aui/framemanager.h"
#include "wx/dataview.h"

class wxSearchCtrl;

class CEditLanguageDialog : public CEditDialogBase
{
    enum
    {
        ID_DATAVIEW_LANGUAGE,
        ID_BUTTON_ADD,
        ID_BUTTON_EXPORT,
        ID_BUTTON_CLEAR,
        ID_TEXT_ENUM,
        ID_TEXT_CHINESE,
        ID_TEXT_ENGLISH,
        ID_SERACH_LANGUAGE,
        ID_POPMENU_CHANGE,
        ID_POPMENU_DLE,
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
    void InitLanguageMap();
    void InitDataViewListCtrl();

    void AppendDataViewListCtrl();
    void ClearTextCtrl();
    void SearchEnum(TString str);
    void ChangeLanguage();
    void DleLanguage();
    void AddLanguage();
    void AddSaveButtonPrompt();
    void SetTextCtrlEnum();

    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonExport(wxCommandEvent& event);
    void OnSearchLanguage(wxCommandEvent& event);
    void OnSearchEnterLanguange(wxCommandEvent& event);
    void OnCancleSrch(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnSelectDataView(wxDataViewEvent& event);
    void OnSrchUpdate(wxCommandEvent& event);
    void OnSrchIdle(wxIdleEvent& event);
    void OnContextMenu( wxDataViewEvent& event);
    void OnMenuEvent(wxCommandEvent& event);
    void OnEnumTextUpdate(wxCommandEvent& event);
private:
    wxDataViewListCtrl*     m_pDataViewListLanguage;
    wxButton*               m_pButtonAdd;
    wxButton*               m_pButtonExport;
    wxButton*               m_pButtonClear;
    wxTextCtrl*             m_pTextEnum;
    wxTextCtrl*             m_pTextChinese;
    wxTextCtrl*             m_pTextEnglish;
    wxStaticText*           m_pStaticTextEnum;
    wxStaticText*           m_pStaticTextChinese;
    wxStaticText*           m_pStaticTextEnglish;
    wxSearchCtrl*           m_pSrchCtrl;
    wxStaticText*           m_pSrchText;
    int                     m_selectRow;
    unsigned long           m_uLastEnumSearchTextUpdateTime;
    bool                    m_bEnumSearchTextUpdate;
    TString                 m_strChangeEnum;
    wxString                m_tmpBeforeStr;
    bool                    m_bAddSaveSuc;
    bool                    m_bIsChange;
    bool                    m_bISLanguageSwitch;
    bool                    m_bIsSave;
    std::map<TString, std::vector<TString>>    m_languageMap;
DECLARE_EVENT_TABLE();
};

#endif