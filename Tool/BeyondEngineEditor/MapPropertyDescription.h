#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_MAPPROPERTYDESCRIPTIONEX_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_MAPPROPERTYDESCRIPTIONEX_H__INCLUDE

#include "wxwidgetsPropertyBase.h"
class CMapElementPropertyDescription;
class CMapPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CMapPropertyDescription(CSerializer* pSerializer);
    CMapPropertyDescription(const CMapPropertyDescription& rRef);
    virtual ~CMapPropertyDescription();

    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual bool IsContainerProperty() override;
    virtual CPropertyDescriptionBase* InsertChild(CPropertyDescriptionBase* pChild, uint32_t uPreIndex = 0xFFFFFFFF) override;
    virtual bool RemoveChild(CPropertyDescriptionBase* pProperty, bool bDelete = true) override;
    virtual void RemoveAllChild(bool bDelete = true) override;

    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void LoadFromXML(rapidxml::xml_node<>* pNode);
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
    virtual void SetOwner(CComponentProxy* pOwner) override;
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit);
    virtual void SerializeContainerElementLocation(CSerializer& serializer, CPropertyDescriptionBase* pChildProperty) override;
    CPropertyDescriptionBase* GetKeyPropertyTemplate() const;
    CPropertyDescriptionBase* GetValuePropertyTemplate() const;
    CMapElementPropertyDescription* CreateMapElementProp();

private:
    void ResetName();
    void GetCurrentName(TCHAR* pName);
private:
    CPropertyDescriptionBase* m_pKeyPropertyTemplate;
    CPropertyDescriptionBase* m_pValuePropertyTemplate;
};


#endif