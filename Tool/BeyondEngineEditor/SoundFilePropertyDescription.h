#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_SOUNDFILEPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_SOUNDFILEPROPERTYDESCRIPTION_H__INCLUDE

#include "FilePropertyDescription.h"

class CSoundFilePropertyDescription : public CFilePropertyDescription
{
    typedef CFilePropertyDescription super;
public:
    CSoundFilePropertyDescription(CSerializer* pSerializer);
    CSoundFilePropertyDescription(const CSoundFilePropertyDescription& rRef);
    virtual ~CSoundFilePropertyDescription();

    virtual CSoundFilePropertyDescription* CreateNewInstance() override;
    virtual wxPGProperty* CreateWxProperty() override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
};

#endif