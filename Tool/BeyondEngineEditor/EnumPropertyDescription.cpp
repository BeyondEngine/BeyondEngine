#include "stdafx.h"
#include "EnumPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Component/Component/ComponentProxy.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

SEnumPropertyDataMapWrapper CEnumPropertyDescription::m_enumPropertyDataMap;

CEnumPropertyDescription::CEnumPropertyDescription(CSerializer* pSerializer)
    : super(eRPT_Enum)
    , m_pPropertyData(NULL)
{
    TString strValue;
    if (pSerializer != NULL)
    {
        int nValue = 0;
        TString enumTypeStr;
        (*pSerializer) >> nValue;
        (*pSerializer) >> enumTypeStr;
        std::vector<TString> vecNames;
        CStringHelper::GetInstance()->SplitString(enumTypeStr.c_str(), _T("::"), vecNames);
        //HACK: Force ignore namespace.
        enumTypeStr = vecNames.back();
        std::map<TString, SEnumPropertyData*>::iterator iter = m_enumPropertyDataMap.m_data.find(enumTypeStr);
        if (iter == m_enumPropertyDataMap.m_data.end())
        {
            const std::vector<SEnumData*>* pEnumValue = NULL;
            bool bFind = CEnumStrGenerator::GetInstance()->GetEnumValueData(enumTypeStr.c_str(), pEnumValue);
            BEATS_ASSERT(bFind, _T("Can't find the enum str for type : %s"), enumTypeStr.c_str());
            if (bFind)
            {
                m_pPropertyData = new SEnumPropertyData(pEnumValue);
                m_enumPropertyDataMap.m_data[enumTypeStr] = m_pPropertyData;
            }
        }
        else
        {
            m_pPropertyData = iter->second;
        }
        if (!QueryStringByValue(nValue, strValue))
        {
            strValue = (*m_pPropertyData->m_pData->begin())->m_str;
        }
    }
    InitializeValue(strValue);
}

CEnumPropertyDescription::CEnumPropertyDescription(const CEnumPropertyDescription& rRef)
    : super(rRef)
    , m_pPropertyData(rRef.m_pPropertyData)
{
    TString strValue;
    InitializeValue(strValue);
}

CEnumPropertyDescription::~CEnumPropertyDescription()
{
    DestroyValue<TString>();
}

const SEnumPropertyData* CEnumPropertyDescription::GetEnumPropertyData() const
{
    return m_pPropertyData;
}

bool CEnumPropertyDescription::QueryValueByString(const TString& str, int& outValue) const
{
    bool bFind = false;
    for (int i = 0; i < (int)(m_pPropertyData->m_pData->size()); ++i)
    {
        SEnumData* pData = m_pPropertyData->m_pData->at(i);
        if (pData->m_str == str)
        {
            bFind = true;
            outValue = pData->m_value;
            break;
        }
    }
    BEATS_ASSERT(bFind, _T("Can't find the enum string %s in Component Id:%d"), str.c_str(), m_pOwner->GetId());
    return bFind;
}

bool CEnumPropertyDescription::QueryStringByValue(int nValue, TString& outString) const
{
    bool bFind = false;
    for (int i = 0; i < (int)(m_pPropertyData->m_pData->size()); ++i)
    {
        SEnumData* pData = m_pPropertyData->m_pData->at(i);
        if (pData->m_value == nValue)
        {
            bFind = true;
            outString = pData->m_str;
            break;
        }
    }
    return bFind;
}

bool CEnumPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit)
{
    for (uint32_t i = 0; i < parameterUnit.size(); ++i)
    {
        TString parameter = parameterUnit[i];
        std::vector<TString> result;
        CStringHelper::GetInstance()->SplitString(parameter.c_str(), PROPERTY_KEYWORD_SPLIT_STR, result);
        if (result.size() == 2)
        {
            _set_errno(0);
            if (_tcsicmp(result[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
            {
                int iValue = 0;
                QueryValueByString(result[1], iValue);
                wxVariant var(iValue);
                SetValue(var, true);
                SetValueWithType(&result[1], eVT_DefaultValue);
            }
            else if (_tcsicmp(result[0].c_str(), UIParameterAttrStr[eUIPAT_EnumStringArray]) == 0)
            {
                BEATS_ASSERT(m_pPropertyData->m_displayString.size() == 0, _T("We have promised that every enum property will export one enum string array! See DECLARE_PROPERTY!"));
                BEATS_ASSERT(result[1].length() > 0, _T("enum string array should not be empty!"));
                CStringHelper::GetInstance()->SplitString(result[1].c_str(), _T("@"), m_pPropertyData->m_displayString);
            }
            else
            {
                BEATS_ASSERT(false, _T("Unknown parameter for Int property %s"), result[0].c_str());
            }
            BEATS_ASSERT(errno == 0, _T("call _tstoi failed! string: %s"), result[1].c_str());
        }
    }
    return true;
}

wxPGProperty* CEnumPropertyDescription::CreateWxProperty()
{
    wxArrayString labels;
    wxArrayInt values;
    const std::vector<SEnumData*>& enumData = *m_pPropertyData->m_pData;
    for (uint32_t i = 0; i < enumData.size(); ++i)
    {
        wxString strRawString = enumData[i]->m_str;
        // Manually filter the last useless options if there is.
        if (enumData.size() - i <= 2)
        {
            if (strRawString.EndsWith(_T("_Count")) ||
                strRawString.EndsWith(_T("_Force32Bit")))
            {
                break;
            }
        }
        wxString displayStr = i < m_pPropertyData->m_displayString.size() ? m_pPropertyData->m_displayString[i].c_str() : strRawString;
        labels.push_back(displayStr);
        values.push_back(enumData[i]->m_value);
    }
    int nCurrValue = 0;
    QueryValueByString(*(TString*)m_valueArray[eVT_CurrentValue], nCurrValue);
    wxPGProperty* pProperty = new wxEnumProperty(wxPG_LABEL, wxPG_LABEL, labels, values, nCurrValue);
    pProperty->SetClientData(this);
    wxVariant var(*(TString*)m_valueArray[eVT_DefaultValue]);
    pProperty->SetDefaultValue(var);
    wxVariant curVar(*(TString*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetValue(curVar);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    return pProperty;
}

void CEnumPropertyDescription::SetValue(wxVariant& value, bool bSaveValue /*= true*/)
{
    int iNewValue = value.GetInteger();
    TString strNewValue;
    QueryStringByValue(iNewValue, strNewValue);
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CEnumPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CEnumPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(TString*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CEnumPropertyDescription::CreateNewInstance()
{
    CEnumPropertyDescription* pNewProperty = new CEnumPropertyDescription(*this);
    return pNewProperty;
}

void CEnumPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    sprintf(pOut, "%s", ((TString*)m_valueArray[type])->c_str());
}

bool CEnumPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    int nValue = 0;
    bool bRet = QueryValueByString(pIn, nValue);
    if (!bRet)
    {
        _set_errno(0);
        TCHAR* pEndChar = NULL;
        int32_t iValue = _tcstol(pIn, &pEndChar, 10);
        if (_tcslen(pEndChar) == 0 && errno == 0)
        {
            std::string strRet;
            bRet = QueryStringByValue(iValue, strRet);
            if (bRet)
            {
                *(TString*)pOutValue = strRet;
            }
        }
    }
    else
    {
        *(TString*)pOutValue = pIn;
    }
    return bRet;
}

void CEnumPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    TString* pStrValue = (TString*)m_valueArray[eValueType];
    int nValue = 0;
    QueryValueByString(*pStrValue, nValue);
    serializer << nValue;
}

void CEnumPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    int nValue = 0;
    serializer >> nValue;
    QueryStringByValue(nValue, *(TString*)m_valueArray[eValueType]);
}
