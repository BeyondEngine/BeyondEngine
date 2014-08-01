#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_WXWIDGETSPROPERTYBASE_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_WXWIDGETSPROPERTYBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/ComponentSystem/Property/PropertyDescriptionBase.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "PropertyTrigger.h"

class wxEnumProperty;
class wxPGProperty;
class wxVariant;

class CWxwidgetsPropertyBase : public CPropertyDescriptionBase
{
    typedef CPropertyDescriptionBase super;
public:
    CWxwidgetsPropertyBase(EReflectPropertyType type);
    CWxwidgetsPropertyBase(const CWxwidgetsPropertyBase& rRef);

    virtual ~CWxwidgetsPropertyBase();

    virtual void Initialize();

    void AddEffectProperties(CWxwidgetsPropertyBase* pProperty);
    std::set<CWxwidgetsPropertyBase*>& GetEffectProperties();
    bool CheckVisibleTrigger();
    wxEnumProperty* GetComboProperty() const;
    wxEnumProperty* CreateComboProperty() const;
    bool ShowChildrenInGrid() const;
    void SetShowChildrenInGrid(bool bShow);

    virtual void SetValueList(const std::vector<TString>& valueList) override;
    virtual wxPGProperty* CreateWxProperty() = 0;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) = 0;
    virtual void SaveToXML(TiXmlElement* pParentNode);
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual void AnalyseUIParameter(const TCHAR* parameter);
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) = 0;

private:
    bool m_bShowChildrenInGrid;
    wxEnumProperty* m_pComboProperty;
    SharePtr<CPropertyTrigger> m_pVisibleWhenTrigger;
    std::set<CWxwidgetsPropertyBase*> m_effctProperties;
};


#endif