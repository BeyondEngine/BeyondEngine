#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_ENUMPROPERTYDESCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_ENUMPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

struct SEnumData;
struct SEnumPropertyData
{
    SEnumPropertyData(const std::vector<SEnumData*>* pData)
        : m_pData(pData)
    {
        BEATS_ASSERT(pData != NULL);
    }
    ~SEnumPropertyData()
    {
    }

    const std::vector<SEnumData*>* m_pData;
    std::vector<TString> m_displayString;
};

// this Wrapper only for recycle memory after the global static variable is destructed.
struct SEnumPropertyDataMapWrapper
{
    SEnumPropertyDataMapWrapper()
    {

    }
    ~SEnumPropertyDataMapWrapper()
    {
        BEATS_SAFE_DELETE_MAP(m_data);
    }

    std::map<TString, SEnumPropertyData*> m_data;
};
class CEnumPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CEnumPropertyDescription(CSerializer* pSerializer);
    CEnumPropertyDescription(const CEnumPropertyDescription& rRef);
    virtual ~CEnumPropertyDescription();

    const SEnumPropertyData* GetEnumPropertyData() const;
    bool QueryValueByString(const TString& str, int& outValue) const;
    bool QueryStringByValue(int nValue, TString& outString) const;

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

private:
    SEnumPropertyData* m_pPropertyData;
    static SEnumPropertyDataMapWrapper m_enumPropertyDataMap;
};
#endif