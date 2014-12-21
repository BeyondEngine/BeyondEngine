#include "stdafx.h"
#include "IntPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

CIntPropertyDescription::CIntPropertyDescription(CSerializer* pSerializer)
: super(eRPT_Int)
, m_minValue(INT_MIN)
, m_maxValue(INT_MAX)
, m_spinStep(0)
{
    int iValue = 0;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> iValue;
    }
    InitializeValue(iValue);
}

CIntPropertyDescription::CIntPropertyDescription(const CIntPropertyDescription& rRef)
: super(rRef)
, m_minValue(rRef.m_minValue)
, m_maxValue(rRef.m_maxValue)
, m_spinStep(rRef.m_spinStep)

{
    InitializeValue<int>(0);
}

CIntPropertyDescription::~CIntPropertyDescription()
{
    DestroyValue<int>();
}

bool CIntPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result)
{
    std::vector<TString> cache;
    for (uint32_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        _set_errno(0);
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            int iValue = 0;
            GetValueByTChar(cache[1].c_str(), &iValue);
            wxVariant var(iValue);
            SetValue(var, true);
            SetValueWithType(&iValue, eVT_DefaultValue);
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MinValue]) == 0)
        {
            m_minValue = _tstoi(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxValue]) == 0)
        {
            m_maxValue = _tstoi(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_SpinStep]) == 0)
        {
            m_spinStep = _tstoi(cache[1].c_str());
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for Int property %s"), cache[0].c_str());
        }
        BEATS_ASSERT(errno == 0, _T("call _tstoi failed! string: %s"), cache[1].c_str());
    }
    BEATS_ASSERT(*(int*)m_valueArray[eVT_CurrentValue] <= m_maxValue && *(int*)m_valueArray[eVT_CurrentValue] >= m_minValue);
    return true;
}
wxPGProperty* CIntPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxIntProperty(wxPG_LABEL, wxPG_LABEL, *(int*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var(*(int*)m_valueArray[eVT_DefaultValue]);
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    pProperty->SetAttribute( wxPG_ATTR_MIN, m_minValue );
    pProperty->SetAttribute( wxPG_ATTR_MAX, m_maxValue );
    if (m_spinStep > 0)
    {
        pProperty->SetEditor( wxPGEditor_SpinCtrl );
        pProperty->SetAttribute( wxT("Step"), (int)m_spinStep );
        pProperty->SetAttribute( wxT("MotionSpin"), true );
    }

    return pProperty;
}

void CIntPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    int iNewValue = value.GetInteger();
    SetValueWithType(&iNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&iNewValue, eVT_SavedValue);
    }    
}

bool CIntPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(int*)pTargetValue != *(int*)pSourceValue;
    if (bRet)
    {
        *(int*)pTargetValue = *(int*)pSourceValue;
    }
    return bRet;
}

bool CIntPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(int*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(int*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CIntPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CIntPropertyDescription(*this);
    return pNewProperty;
}

void CIntPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    int iValue = *(int*)m_valueArray[type];
    sprintf(pOut, "%d", iValue);
}

bool CIntPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    *(int*)pOutValue = _tstoi(pIn);
    return true;
}

void CIntPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << *(int*)m_valueArray[eValueType];
}

void CIntPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(int*)m_valueArray[eValueType];
}
