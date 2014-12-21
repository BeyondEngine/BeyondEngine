#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_SOUNDFILEPROPERTY_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_SOUNDFILEPROPERTY_H__INCLUDE

#include <wx/propgrid/propgrid.h>
class CSoundFileProperty : public wxFileProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(CSoundFileProperty)
public:
    CSoundFileProperty(const wxString& label = wxPG_LABEL,
        const wxString& name = wxPG_LABEL,
        wxString value = wxEmptyString);
    virtual ~CSoundFileProperty();

    virtual wxPGEditorDialogAdapter* GetEditorDialog() const override;
};

#endif