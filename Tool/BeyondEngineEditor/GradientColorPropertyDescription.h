#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_GRADIENTCOLORPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_GRADIENTCOLORPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CGradientColorPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CGradientColorPropertyDescription(CSerializer* pSerializer);
    CGradientColorPropertyDescription(const CGradientColorPropertyDescription& rRef);

    virtual ~CGradientColorPropertyDescription();

    virtual void Initialize() override;
    virtual wxPGProperty* CreateWxProperty() override;
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) override;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) override;
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML) override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;

    void ApplyToDialog();
    TString WriteToString() const;
    void ReadFromString(const TString& strValue);
    std::map<float, CColor>& GetColorMap();
    std::map<float, uint8_t>& GetAlphaMap();
private:
    std::map<float, CColor> m_colorMap;
    std::map<float, uint8_t> m_alphaMap;
};

#endif