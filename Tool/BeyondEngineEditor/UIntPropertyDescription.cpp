#include "stdafx.h"
#include "UIntPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

CUIntPropertyDescription::CUIntPropertyDescription(CSerializer* pSerializer)
: super(eRPT_UInt)
, m_minValue(0)
, m_maxValue(ULONG_MAX)
, m_spinStep(0)
{
    uint32_t value = 0;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> value;
    }
    InitializeValue(value);
}

CUIntPropertyDescription::CUIntPropertyDescription(const CUIntPropertyDescription& rRef)
: super(rRef)
, m_minValue(rRef.m_minValue)
, m_maxValue(rRef.m_maxValue)
, m_spinStep(rRef.m_spinStep)

{
    uint32_t value = 0;
    InitializeValue(value);
}

CUIntPropertyDescription::~CUIntPropertyDescription()
{
    DestroyValue<uint32_t>();
}

bool CUIntPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result)
{
    std::vector<TString> cache;
    for (uint32_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            uint32_t uValue = 0;
            GetValueByTChar(cache[1].c_str(), &uValue);
            wxVariant var((wxLongLong)uValue);
            SetValue(var, true);
            uValue = var.GetULongLong().GetLo();
            SetValueWithType(&uValue, eVT_DefaultValue);
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
            BEATS_ASSERT(false, _T("Unknown parameter for UInt property %s"), cache[0].c_str());
        }
    }
    BEATS_ASSERT(*(uint32_t*)m_valueArray[eVT_CurrentValue] <= m_maxValue && *(uint32_t*)m_valueArray[eVT_CurrentValue] >= m_minValue && m_minValue >= 0);
    return true;
}

wxPGProperty* CUIntPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxUIntProperty(wxPG_LABEL, wxPG_LABEL, *(uint32_t*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var((wxLongLong)(*(uint32_t*)m_valueArray[eVT_CurrentValue]));
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    pProperty->SetAttribute( wxPG_ATTR_MIN, (wxULongLong)m_minValue );
    pProperty->SetAttribute( wxPG_ATTR_MAX, (wxULongLong)m_maxValue );
    if (m_spinStep > 0)
    {
        pProperty->SetEditor( wxPGEditor_SpinCtrl );
        pProperty->SetAttribute( wxT("Step"), (long)m_spinStep );
        pProperty->SetAttribute( wxT("MotionSpin"), true );
    }
    return pProperty;
}

void CUIntPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    uint32_t uNewValue = value.GetULongLong().GetLo();
    SetValueWithType(&uNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&uNewValue, eVT_SavedValue);
    }
}

bool CUIntPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(uint32_t*)pTargetValue != *(uint32_t*)pSourceValue;
    if (bRet)
    {
        *(uint32_t*)pTargetValue = *(uint32_t*)pSourceValue;
    }
    return bRet;
}

bool CUIntPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(uint32_t*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(uint32_t*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CUIntPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CUIntPropertyDescription(*this);
    return pNewProperty;
}

void CUIntPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    uint32_t uValue = *(uint32_t*)m_valueArray[type];
    sprintf(pOut, "%u", uValue);
}

bool CUIntPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    TCHAR* pEndChar = NULL;
    _set_errno(0);
    uint32_t uValue = _tcstoul(pIn, &pEndChar, 10);
    BEATS_ASSERT(pIn[0] != _T('-'), _T("Negative number can't be set into CUIntPropertyDescription"));
    BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read uint from string %s error, stop at %s"), pIn, pEndChar);
    BEATS_ASSERT(errno == 0, _T("Call _tcstoul failed! string %s radix: 10"), pIn);
    *(uint32_t*)pOutValue = uValue;
    return true;
}

void CUIntPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << *(uint32_t*)m_valueArray[eValueType];
}

void CUIntPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(uint32_t*)m_valueArray[eValueType];
}
