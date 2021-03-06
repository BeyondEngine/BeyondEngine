#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_COLORPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_COLORPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CColorPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CColorPropertyDescription(CSerializer* pSerializer);
    CColorPropertyDescription(const CColorPropertyDescription& rRef);

    virtual ~CColorPropertyDescription();

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) override;
    virtual wxPGProperty* CreateWxProperty() override;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) override;
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML) override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
};

#endif