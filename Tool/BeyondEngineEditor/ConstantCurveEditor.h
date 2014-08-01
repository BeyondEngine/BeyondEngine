#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_CONSTANTCURVEEDITOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_CONSTANTCURVEEDITOR_H__INCLUDE

#include <wx/propgrid/propgrid.h>

class CConstantCurveEditor : public wxPGTextCtrlAndButtonEditor
{
public:
    CConstantCurveEditor();
    virtual ~CConstantCurveEditor();
    bool GetPropertyCtrlValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl );
    bool GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const;
    virtual void DrawValue (wxDC &dc, const wxRect &rect, wxPGProperty *property, const wxString &text) const;
    virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz ) const;
private:

};



#endif