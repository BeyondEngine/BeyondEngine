#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PTRPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PTRPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"
class CComponentProxy;

class CPtrPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CPtrPropertyDescription(CSerializer* serializer);
    CPtrPropertyDescription(const CPtrPropertyDescription& rRef);
    virtual ~CPtrPropertyDescription();

    uint32_t GetPtrGuid() const;
    void SetPtrGuid(uint32_t uGuid);
    void SetDerivedGuid(uint32_t uDerivedGuid);
    uint32_t GetDerivedGuid() const;
    bool CreateInstance(bool bCallInitFunc = true);
    bool DestroyInstance();

    virtual void SetValue(wxVariant& value, bool bSaveValue) override;

    virtual CComponentProxy* GetInstanceComponent() const override;

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) override;
    virtual wxPGProperty* CreateWxProperty() override;
    virtual void LoadFromXML(rapidxml::xml_node<>* pNode) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML) override;

    virtual CPropertyDescriptionBase* Clone(bool bCloneValue) override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;

    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual uint32_t HACK_GetPtrReflectGuid() const override;
    virtual void HACK_InformPtrPropertyToDeleteInstance() override;
private:
    void UpdateDisplayString(uint32_t uComponentGuid, bool bUpdateHostComponent = true);

private:
    // This flag indicate when we call CComponentInstance::CloneInstance, if this variable should be cloned.
    bool m_bCanBeCloned;
    uint32_t m_uDerivedGuid;
    uint32_t m_uComponentGuid;
    CComponentProxy* m_pInstance;
    // This flag indicate if this property should have instance according to XML data. 
    bool m_bHasInstance;
};
#endif