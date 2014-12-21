#include "stdafx.h"
#include "StringPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>

#include "EngineEditor.h"

CStringPropertyDescription::CStringPropertyDescription(CSerializer* pSerializer)
: super(eRPT_Str)
, m_maxLength(0)
{
    std::string strValue;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> strValue;
    }
    InitializeValue<std::string>(strValue);
}

CStringPropertyDescription::CStringPropertyDescription(const CStringPropertyDescription& rRef)
: super(rRef)
, m_maxLength(rRef.m_maxLength)
{
    std::string strValue;
    InitializeValue<std::string>(strValue);
}

CStringPropertyDescription::~CStringPropertyDescription()
{
    DestroyValue<std::string>();
}

bool CStringPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result)
{
    std::set<TString> spaceFilter;
    spaceFilter.insert(_T(" "));
    std::vector<TString> cache;
    for (uint32_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache, false);
        // Manually filter the space of keyword because we specify "bIgnoreSpace = false" to SplitString(to avoid filter the space in the string content).
        cache[0] = CStringHelper::GetInstance()->FilterString(cache[0].c_str(), spaceFilter);
        BEATS_ASSERT(cache.size() <= 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            if (cache.size() == 2)
            {
                TString str;
                GetValueByTChar(cache[1].c_str(), &str);
                wxVariant var(str.c_str());
                SetValue(var, true);
                SetValueWithType(&str, eVT_DefaultValue);
            }
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxCount]) == 0)
        {
            m_maxLength = _tstoi(cache[1].c_str());
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for String property %s"), cache[0].c_str());
        }
    }
    return true;
}

wxPGProperty* CStringPropertyDescription::CreateWxProperty()
{
    wxString strCurrValue = ((TString*)m_valueArray[eVT_CurrentValue])->c_str();
    wxString strDefaultValue = ((TString*)m_valueArray[eVT_DefaultValue])->c_str();
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, wxString::FromUTF8(strCurrValue));
    pProperty->SetClientData(this);
    wxVariant var(wxString::FromUTF8(strDefaultValue));
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    return pProperty;
}

void CStringPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    TString strNewValue = value.GetString().ToUTF8();
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CStringPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CStringPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = _tcscmp(((TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)])->c_str(), ((TString*)m_valueArray[eVT_CurrentValue])->c_str()) == 0;
    return bRet;
}

CPropertyDescriptionBase* CStringPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CStringPropertyDescription(*this);
    return pNewProperty;
}

void CStringPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    std::string* pStr = (std::string*)m_valueArray[type];
    _tcscpy(pOut, pStr->c_str());
}

bool CStringPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    ((TString*)pOutValue)->assign(pIn);
    return true;
}

void CStringPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << *(TString*)m_valueArray[eValueType];
}

void CStringPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(TString*)m_valueArray[eValueType];
}

