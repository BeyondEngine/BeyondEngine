#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_FLOATPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_FLOATPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CFloatPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CFloatPropertyDescription(CSerializer* pSerializer);
    CFloatPropertyDescription(const CFloatPropertyDescription& rRef);

    virtual ~CFloatPropertyDescription();

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
    void SetSpinStep(float fSpinStep);

private:
    float m_fMinValue;
    float m_fMaxValue;
    float m_fSpinStep;
};

#endif