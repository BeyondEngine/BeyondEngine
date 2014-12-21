#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_RANDOMCOLORPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_RANDOMCOLORPROPERTYDESCRIPTION_H__INCLUDE

#include "wxWidgetsPropertyBase.h"
#include "RandomColor.h"
class CGradientColorPropertyDescription;
class CColorPropertyDescription;
class CRandomColorPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CRandomColorPropertyDescription(CSerializer* pSerializer);
    CRandomColorPropertyDescription(const CRandomColorPropertyDescription& rRef);

    virtual ~CRandomColorPropertyDescription();
    virtual void Initialize() override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML) override;
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) override;
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) override;
    virtual wxPGProperty* CreateWxProperty() override;
    void SetColorType(ERandomColorType type);
    ERandomColorType GetColorType() const;
    void UpdatePropertyByType(ERandomColorType type);
    wxPGProperty* CreateRandomColorProperty(ERandomColorType type, wxPGProperty*& minProperty);
    bool IsOnlyGradient() const;
    bool IsOnlyColor() const;
private:
    bool m_bOnlyGradient = false;
    bool m_bOnlyColor = false;
    ERandomColorType m_colorType = ERandomColorType::eRCT_ConstantColor;
    CGradientColorPropertyDescription* m_pMinGradientColorProperty = nullptr;
    CGradientColorPropertyDescription* m_pMaxGradientColorProperty = nullptr;
    CColorPropertyDescription* m_pMinColorProperty = nullptr;
    CColorPropertyDescription* m_pMaxColorProperty = nullptr;
};
#endif