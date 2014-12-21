#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_WXWIDGETSPROPERTYBASE_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_WXWIDGETSPROPERTYBASE_H__INCLUDE

#include "Component/Property/PropertyDescriptionBase.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "PropertyTrigger.h"
#include "Utility/RapidXML/rapidxml.hpp"
#include "Utility/RapidXML/rapidxml_utils.hpp"

class wxEnumProperty;
class wxPGProperty;
class wxVariant;
class wxImage;
class CWxwidgetsPropertyBase : public CPropertyDescriptionBase
{
    typedef CPropertyDescriptionBase super;
public:
    CWxwidgetsPropertyBase(EReflectPropertyType type);
    CWxwidgetsPropertyBase(const CWxwidgetsPropertyBase& rRef);

    virtual ~CWxwidgetsPropertyBase();

    virtual void Initialize() override;

    void AddEffectProperties(CWxwidgetsPropertyBase* pProperty);
    std::set<CWxwidgetsPropertyBase*>& GetEffectProperties();
    bool CheckVisibleTrigger();
    wxEnumProperty* GetComboProperty() const;
    wxEnumProperty* CreateComboProperty() const;
    bool ShowChildrenInGrid() const;
    void SetShowChildrenInGrid(bool bShow);
    const wxImage& GetValueImage();
    void SetValueImage(const wxImage& image);
    bool IsExpanded() const;
    void SetExpandFlag(bool bExpand);

    virtual void SetValueList(const std::vector<TString>& valueList) override;
    virtual wxPGProperty* CreateWxProperty() = 0;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) = 0;
    virtual void SaveToXML(rapidxml::xml_node<>* pParentNode) override;
    virtual void LoadFromXML(rapidxml::xml_node<>* pNode) override;
    virtual void AnalyseUIParameter(const TCHAR* parameter) override;
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) = 0;

private:
    bool m_bExpand = false;
    bool m_bShowChildrenInGrid;
    wxEnumProperty* m_pComboProperty;
    SharePtr<CPropertyTrigger> m_pVisibleWhenTrigger;
    std::set<CWxwidgetsPropertyBase*> m_effctProperties;
    wxImage* m_pValueImage = nullptr;
};


#endif