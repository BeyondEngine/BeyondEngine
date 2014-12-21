#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_WXSOUNDPREVIEWEDITOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_WXSOUNDPREVIEWEDITOR_H__INCLUDE

#include <wx/propgrid/propgrid.h>
#include "wx/propgrid/editors.h"
#include "wxPGEditorBase.h"

class wxSoundPreviewEditor : public wxPGEditorBase
{
    DECLARE_DYNAMIC_CLASS(wxSoundPreviewEditor)
    typedef wxPGEditorBase super;
public:
    wxSoundPreviewEditor();
    virtual ~wxSoundPreviewEditor();

    virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid,
        wxPGProperty* property,
        const wxPoint& pos,
        const wxSize& sz ) const;
    virtual bool OnEvent( wxPropertyGrid* propGrid,
        wxPGProperty* property,
        wxWindow* ctrl,
        wxEvent& event ) const;
};

#endif