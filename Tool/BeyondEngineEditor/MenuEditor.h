#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_MENUEDITOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_MENUEDITOR_H__INCLUDE

#include <wx/propgrid/propgrid.h>
#include "wxPGEditorBase.h"

class CMenuEditor : public wxPGTextCtrlAndButtonEditor
{
    DECLARE_DYNAMIC_CLASS(CMenuEditor)
    typedef wxPGTextCtrlAndButtonEditor super;
public:
    CMenuEditor();
    virtual ~CMenuEditor();

    virtual wxPGWindowList CreateControls(wxPropertyGrid* propGrid,
        wxPGProperty* property,
        const wxPoint& pos,
        const wxSize& sz) const;
    virtual bool OnEvent(wxPropertyGrid* propGrid,
        wxPGProperty* property,
        wxWindow* ctrl,
        wxEvent& event) const;

};

#endif