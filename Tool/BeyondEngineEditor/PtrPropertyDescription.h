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

    size_t GetPtrGuid();
    void SetDerivedGuid(size_t uDerivedGuid);
    size_t GetDerivedGuid() const;
    bool CreateInstance(bool bCallInitFunc = true);

    //When destroy instance in editor, we need to delete the component, otherwise, user will manage the host component.
    bool DestroyInstance(bool bDeleteHostComponent);

    virtual void SetValue(wxVariant& value, bool bSaveValue);

    virtual CComponentProxy* GetInstanceComponent() const override;

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit);
    virtual wxPGProperty* CreateWxProperty();
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual bool IsDataSame(bool bWithDefaultOrXML);

    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance();

    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;

private:
    void UpdateDisplayString(size_t uComponentGuid);

private:
    size_t m_uDerivedGuid;
    size_t m_uComponentGuid;
    CComponentProxy* m_pInstance;
    // This flag indicate if this property should have instance according to XML data. 
    bool m_bHasInstance;
};
#endif