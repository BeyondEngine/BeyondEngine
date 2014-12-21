#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_WXDIALOGEDITOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_WXDIALOGEDITOR_H__INCLUDE

#include <wx/propgrid/propgrid.h>
#include "wxPGEditorBase.h"

class wxDialogEditor : public wxPGEditorBase
{
    DECLARE_DYNAMIC_CLASS(wxDialogEditor)
    typedef wxPGEditorBase super;
public:
    wxDialogEditor();
    wxDialogEditor(wxDialog* pDialog);
    virtual ~wxDialogEditor();

    virtual wxString GetName() const;
    virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid,
        wxPGProperty* property,
        const wxPoint& pos,
        const wxSize& sz ) const;
    virtual bool OnEvent( wxPropertyGrid* propGrid,
        wxPGProperty* property,
        wxWindow* ctrl,
        wxEvent& event ) const;
    void SetDialog(wxDialog* pDialog);

private:
    wxDialog* m_pDialog;
};

#endif