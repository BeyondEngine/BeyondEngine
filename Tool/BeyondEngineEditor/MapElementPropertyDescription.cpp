#include "stdafx.h"
#include "MapElementPropertyDescription.h"

CMapElementPropertyDescription::CMapElementPropertyDescription(CSerializer* pSerializer)
: super(pSerializer)
{
    SetType(eRPT_MapElement);
}

CMapElementPropertyDescription::CMapElementPropertyDescription(const CMapElementPropertyDescription& rRef)
: super(rRef)
{

}

void CMapElementPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType)
{
    BEATS_ASSERT(GetChildren().size() == 2, _T("An element of map must contain two children!"));
    GetChildren()[0]->Serialize(serializer, eValueType);
    GetChildren()[1]->Serialize(serializer, eValueType);
}

void CMapElementPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType)
{
    BEATS_ASSERT(GetChildren().size() == 2, _T("An element of map must contain two children!"));
    GetChildren()[0]->Deserialize(serializer, eValueType);
    GetChildren()[1]->Deserialize(serializer, eValueType);
}

void CMapElementPropertyDescription::SetOwner(CComponentProxy* pOwner)
{
    super::SetOwner(pOwner);
    BEATS_ASSERT(GetChildren().size() == 2, _T("An element of map must contain two children!"));
    GetChildren()[0]->SetOwner(pOwner);
    GetChildren()[1]->SetOwner(pOwner);
}

CPropertyDescriptionBase* CMapElementPropertyDescription::Clone(bool bCloneValue)
{
    CMapElementPropertyDescription* pNewProperty = static_cast<CMapElementPropertyDescription*>(super::Clone(bCloneValue));
    BEATS_ASSERT(GetChildren().size() == 2, _T("Map property must contain two property childern for each element."));
    CPropertyDescriptionBase* pKeyProperty = GetChildren()[0];
    CPropertyDescriptionBase* pNewKeyProperty = pKeyProperty->Clone(true);
    CPropertyDescriptionBase* pValueProperty = GetChildren()[1];
    CPropertyDescriptionBase* pNewValueProperty = pValueProperty->Clone(true);
    pNewProperty->InsertChild(pNewKeyProperty);
    pNewProperty->InsertChild(pNewValueProperty);
    return pNewProperty;
}

bool CMapElementPropertyDescription::IsContainerProperty()
{
    return true;
}

CPropertyDescriptionBase* CMapElementPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CMapElementPropertyDescription(*this);
    return pNewProperty;
}