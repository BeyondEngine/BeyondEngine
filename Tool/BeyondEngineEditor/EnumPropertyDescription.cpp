#include "stdafx.h"
#include "EnumPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

SEnumPropertyDataMapWrapper CEnumPropertyDescription::m_enumPropertyDataMap;

CEnumPropertyDescription::CEnumPropertyDescription(CSerializer* pSerializer)
    : super(eRPT_Enum)
    , m_pPropertyData(NULL)
{
    int iValue = 0;
    if (pSerializer != NULL)
    {
        TString enumTypeStr;
        (*pSerializer) >> iValue;
        (*pSerializer) >> enumTypeStr;
        std::vector<TString> vecNames;
        CStringHelper::GetInstance()->SplitString(enumTypeStr.c_str(), _T("::"), vecNames);
        //HACK: Force ignore namespace.
        enumTypeStr = vecNames[vecNames.size() - 1];
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
    }
    InitializeValue(iValue);
}

CEnumPropertyDescription::CEnumPropertyDescription(const CEnumPropertyDescription& rRef)
    : super(rRef)
    , m_pPropertyData(rRef.m_pPropertyData)
{
    InitializeValue(0);
}

CEnumPropertyDescription::~CEnumPropertyDescription()
{
    DestroyValue<int>();
}

const SEnumPropertyData* CEnumPropertyDescription::GetEnumPropertyData() const
{
    return m_pPropertyData;
}

int CEnumPropertyDescription::QueryValueByString(const TString& str) const
{
    bool bFind = false;
    int nRet = -1;
    for (int i = 0; i < (int)(m_pPropertyData->m_pData->size()); ++i)
    {
        SEnumData* pData = m_pPropertyData->m_pData->at(i);
        if (pData->m_str == str)
        {
            bFind = true;
            nRet = pData->m_value;
            break;
        }
    }
    BEATS_ASSERT(bFind, _T("Can't find the enum string %s in %s"), str.c_str(), m_pPropertyData->m_displayString[0].c_str());
    return nRet;
}

TString CEnumPropertyDescription::QueryStringByValue(int nValue) const
{
    bool bFind = false;
    TString strRet;
    for (int i = 0; i < (int)(m_pPropertyData->m_pData->size()); ++i)
    {
        SEnumData* pData = m_pPropertyData->m_pData->at(i);
        if (pData->m_value == nValue)
        {
            bFind = true;
            strRet = pData->m_str;
            break;
        }
    }
    BEATS_ASSERT(bFind, _T("Can't find the enum value %d in %s"), nValue, m_pPropertyData->m_displayString[0].c_str());
    return strRet;
}

bool CEnumPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit)
{
    for (size_t i = 0; i < parameterUnit.size(); ++i)
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
                GetValueByTChar(result[1].c_str(), &iValue);
                wxVariant var(iValue);
                SetValue(var, true);
                SetValueWithType(&iValue, eVT_DefaultValue);
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
    for (size_t i = 0; i < enumData.size(); ++i)
    {
        labels.push_back(i < m_pPropertyData->m_displayString.size() ? m_pPropertyData->m_displayString[i].c_str() : enumData[i]->m_str.c_str());
        values.push_back(enumData[i]->m_value);
    }

    wxPGProperty* pProperty = new wxEnumProperty(wxPG_LABEL, wxPG_LABEL, labels, values, *(int*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var(*(int*)m_valueArray[eVT_DefaultValue]);
    pProperty->SetDefaultValue(var);
    wxVariant curVar(*(int*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetValue(curVar);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    return pProperty;
}

void CEnumPropertyDescription::SetValue(wxVariant& value, bool bSaveValue /*= true*/)
{
    int iNewValue = value.GetInteger();
    SetValueWithType(&iNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&iNewValue, eVT_SavedValue);
    }
}

bool CEnumPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(int*)pTargetValue != *(int*)pSourceValue;
    if (bRet)
    {
        *(int*)pTargetValue = *(int*)pSourceValue;
    }
    return bRet;
}

bool CEnumPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(int*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(int*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CEnumPropertyDescription::CreateNewInstance()
{
    CEnumPropertyDescription* pNewProperty = new CEnumPropertyDescription(*this);
    return pNewProperty;
}

void CEnumPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    int iValue = *(int*)m_valueArray[type];
    sprintf(pOut, "%d", iValue);
}

bool CEnumPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    *(int*)pOutValue = _tstoi(pIn);
    return true;
}

void CEnumPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << *(int*)m_valueArray[eValueType];
}

void CEnumPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(int*)m_valueArray[eValueType];
}
