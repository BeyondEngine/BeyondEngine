#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CListPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CListPropertyDescription(CSerializer* pSerializer);
    CListPropertyDescription(const CListPropertyDescription& rRef);
    virtual ~CListPropertyDescription();

    virtual bool IsContainerProperty() override;
    bool IsFixed() const;
    void SetFixed(bool bFixedFlag);
    uint32_t GetMaxCount() const;
    void SetMaxCount(uint32_t uMaxCount);
    void SetTemplateProperty(CPropertyDescriptionBase* pTemplateProperty);
    CPropertyDescriptionBase* GetTemplateProperty() const;

    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual CPropertyDescriptionBase* InsertChild(CPropertyDescriptionBase* pProperty, uint32_t uPreIndex = 0xFFFFFFFF) override;
    virtual bool RemoveChild(CPropertyDescriptionBase* pProperty, bool bDelete = true) override;
    virtual void RemoveAllChild(bool bDelete = true) override;

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) override;
    virtual wxPGProperty* CreateWxProperty() override;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) override;
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML) override;
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue) override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void LoadFromXML(rapidxml::xml_node<>* pNode) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
    virtual bool OnChildChanged(uint32_t uChildIndex) override;

    virtual void SetOwner(CComponentProxy* pOwner) override;
    void ChangeOrder(CPropertyDescriptionBase* pChild, uint32_t uPreIndex);
protected:
    virtual TString GetCurrentName();
    void ResetName(bool bForceSet = false);
    CPropertyDescriptionBase* CreateInstance();

private:
    bool m_bGridStyle;
    bool m_bFixCount;
    uint32_t m_uMaxCount;
    CPropertyDescriptionBase* m_pChildTemplate;
};

#endif