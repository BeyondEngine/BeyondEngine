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
        typedef std::map<TString, SEnumPropertyData*> TEnumPropertyDataType;
        BEATS_SAFE_DELETE_MAP(m_data, TEnumPropertyDataType);
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
    int QueryValueByString(const TString& str) const;
    TString QueryStringByValue(int nValue) const;

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit);
    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;

private:
    SEnumPropertyData* m_pPropertyData;
    static SEnumPropertyDataMapWrapper m_enumPropertyDataMap;
};
#endif