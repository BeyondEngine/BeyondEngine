#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_RANDOMPROPERTYDIALOG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_RANDOMPROPERTYDIALOG_H__INCLUDE

#include "EditDialogBase.h"
#include "RandomValue.h"
class CRandomPropertyDialog : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CRandomPropertyDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style = wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CRandomPropertyDialog();
    void SetRandomValue(SRandomValue* pValue);
    virtual int ShowModal() override;

private:
    void OnTypeChoiceChanged(wxCommandEvent& event);
    void UpdateDialog(ENewRandomValueType type);

private:
    wxChoice* m_pTypeChoices;
    wxTextCtrl* m_pConstantTextCtrl;
    wxTextCtrl* m_pRandomMinTextCtrl;
    wxTextCtrl* m_pRandomMaxTextCtrl;
    SRandomValue* m_pRandomValue;
};

#endif