#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_KMVECPROPERTY_VEC3FPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_KMVECPROPERTY_VEC3FPROPERTYDESCRIPTION_H__INCLUDE

#include "ListPropertyDescription.h"

class CVec3PropertyDescription : public CListPropertyDescription
{
    typedef CListPropertyDescription super;
public:
    CVec3PropertyDescription(CSerializer* pSerializer);
    CVec3PropertyDescription(const CVec3PropertyDescription& rRef);

    virtual ~CVec3PropertyDescription();

    virtual void Initialize() override;

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML) override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue) override;
    virtual bool IsContainerProperty() override;
    virtual void LoadFromXML( rapidxml::xml_node<>* pNode ) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
private:
    void ResetChildName();
    virtual TString GetCurrentName() override;

public:
    static SharePtr<SBasicPropertyInfo> m_pZBasicPropertyInfo;
};

#endif