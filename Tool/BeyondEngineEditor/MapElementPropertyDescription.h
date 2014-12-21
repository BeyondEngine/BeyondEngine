#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_MAPELEMENTPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_MAPELEMENTPROPERTYDESCRIPTION_H__INCLUDE

#include "StringPropertyDescription.h"

class CMapElementPropertyDescription : public CStringPropertyDescription
{
    typedef CStringPropertyDescription super;
public:
    CMapElementPropertyDescription(CSerializer* pSerializer);
    CMapElementPropertyDescription(const CMapElementPropertyDescription& rRef);

    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
    virtual void SetOwner(CComponentProxy* pOwner) override;
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue) override;
    virtual bool IsContainerProperty() override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;

};

#endif