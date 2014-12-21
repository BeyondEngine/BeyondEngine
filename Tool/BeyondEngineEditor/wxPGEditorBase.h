#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_WXPGEDITORBASE_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PGEDITOR_WXPGEDITORBASE_H__INCLUDE
#include <wx/propgrid/propgrid.h>
class wxPGMultiButton;
class wxPGProperty;

class wxPGEditorBase : public wxPGTextCtrlEditor
{
public:
    wxPGEditorBase();
    virtual ~wxPGEditorBase();
    virtual void AddDeleteButton(wxPGProperty* property, wxPGMultiButton* pMulButton) const;
    virtual void DeleteEvent(wxPropertyGrid* propGrid, wxPGProperty* property) const;

private:

};
#endif