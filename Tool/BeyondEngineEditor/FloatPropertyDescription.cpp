#include "stdafx.h"
#include "FloatPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include <float.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

CFloatPropertyDescription::CFloatPropertyDescription(CSerializer* pSerializer)
: super(eRPT_Float)
, m_fMinValue(-FLT_MAX)
, m_fMaxValue(FLT_MAX)
, m_fSpinStep(0)
{
    float fValue = 0;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> fValue;
    }
    InitializeValue(fValue);
}

CFloatPropertyDescription::CFloatPropertyDescription(const CFloatPropertyDescription& rRef)
: super(rRef)
, m_fMinValue(rRef.m_fMinValue)
, m_fMaxValue(rRef.m_fMaxValue)
, m_fSpinStep(rRef.m_fSpinStep)
{
    InitializeValue(0.F);
}

CFloatPropertyDescription::~CFloatPropertyDescription()
{
    DestroyValue<float>();
}

bool CFloatPropertyDescription::AnalyseUIParameterImpl( const std::vector<TString>& result)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            float fValue = 0;
            GetValueByTChar(cache[1].c_str(), &fValue);
            wxVariant var(fValue);
            SetValue(var, true);
            SetValueWithType(&fValue, eVT_DefaultValue);
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MinValue]) == 0)
        {
            m_fMinValue = (float)_tstof(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxValue]) == 0)
        {
            m_fMaxValue = (float)_tstof(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_SpinStep]) == 0)
        {
            m_fSpinStep = (float)_tstof(cache[1].c_str());
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for float property %s"), cache[0].c_str());
        }
    }
    BEATS_ASSERT(*(float*)m_valueArray[eVT_CurrentValue] <= m_fMaxValue && *(float*)m_valueArray[eVT_CurrentValue] >= m_fMinValue);
    return true;
}

wxPGProperty* CFloatPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxFloatProperty(wxPG_LABEL, wxPG_LABEL, *(float*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant defaultValue(*(float*)m_valueArray[eVT_DefaultValue]);
    pProperty->SetDefaultValue(defaultValue);
    pProperty->SetModifiedStatus(!IsDataSame(true));

    pProperty->SetAttribute(wxPG_ATTR_MIN, m_fMinValue);
    pProperty->SetAttribute(wxPG_ATTR_MAX, m_fMaxValue );
    if (m_fSpinStep > 0)
    {
        pProperty->SetEditor(wxPGEditor_SpinCtrl);
        pProperty->SetAttribute( wxT("Step"), m_fSpinStep );
        pProperty->SetAttribute( wxT("MotionSpin"), true );
    }
    return pProperty;
}

void CFloatPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    float fNewValue = (float)value.GetDouble();
    SetValueWithType(&fNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&fNewValue, eVT_SavedValue);
    }
}

bool CFloatPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(float*)pTargetValue != *(float*)pSourceValue;
    if (bRet)
    {
        *(float*)pTargetValue = *(float*)pSourceValue;
    }
    return bRet;
}

bool CFloatPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(float*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(float*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

void CFloatPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    float* pValue = (float*)m_valueArray[type];
    sprintf(pOut, "%g", *pValue);
}

bool CFloatPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    *(float*)pOutValue = (float)_tstof(pIn);
    return true;
}

CPropertyDescriptionBase* CFloatPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CFloatPropertyDescription(*this);
    return pNewProperty;
}

void CFloatPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << *(float*)m_valueArray[eValueType];
}

void CFloatPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(float*)m_valueArray[eValueType];
}
