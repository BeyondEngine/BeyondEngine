#include "stdafx.h"
#include "EngineEditor.h"
#include "GradientColorPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

CGradientColorPropertyDescription::CGradientColorPropertyDescription(CSerializer* /*pSerializer*/)
    : super(eRPT_GradientColor)
{
    TString strValue;
    strValue = _T("...");
    InitializeValue(strValue);
}

CGradientColorPropertyDescription::CGradientColorPropertyDescription(const CGradientColorPropertyDescription& rRef)
    : super(rRef)
{
    TString strValue;
    InitializeValue<TString>(strValue);
}

CGradientColorPropertyDescription::~CGradientColorPropertyDescription()
{
    DestroyValue<TString>();
}

bool CGradientColorPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            TString value(_T("..."));
            wxVariant var(value);
            SetValue(var, true);
            SetValueWithType(&value, eVT_DefaultValue);
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for UInt property %s"), cache[0].c_str());
        }
    }
    return true;
}

wxPGProperty* CGradientColorPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, ((TString*)m_valueArray[eVT_CurrentValue])->c_str());
    wxVariant defaultValue(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(defaultValue);
    pProperty->SetClientData(this);
    pProperty->SetModifiedStatus(GetInstanceComponent() != NULL);
    pProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetGradientColorEditor());

    return pProperty;
}

void CGradientColorPropertyDescription::SetValue( wxVariant& /*value*/, bool bSaveValue /*= true*/ )
{
    TString strNewValue = FormatSplineValue();
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    } 
}

bool CGradientColorPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    //return true means always do value sync( from PropertyDescription.Serialize() to Component.ReflectData() )
    return true;
}

bool CGradientColorPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = _tcscmp(((TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)])->c_str(), ((TString*)m_valueArray[eVT_CurrentValue])->c_str()) == 0;
    return bRet;
}

CPropertyDescriptionBase* CGradientColorPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CGradientColorPropertyDescription(*this);
    return pNewProperty;
}

void CGradientColorPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    TString* pStr = (TString*)m_valueArray[type];
    CStringHelper::GetInstance()->ConvertToCHAR(pStr->c_str(), pOut, 1024);
}

bool CGradientColorPropertyDescription::GetValueByTChar(const TCHAR* /*pIn*/, void* /*pOutValue*/)
{
    // Do nothing.
    return true;
}

void CGradientColorPropertyDescription::Serialize(CSerializer& serializer, EValueType /*eValueType*/ /*= eVT_SavedValue*/)
{
    serializer << FormatSplineValue();
}

void CGradientColorPropertyDescription::Deserialize(CSerializer& /*serializer*/, EValueType /*eValueType*/ /*= eVT_CurrentValue*/)
{
    BEATS_ASSERT(false, _T("Implement this later!"));
}

CColorSpline* CGradientColorPropertyDescription::GetColorSpline()
{
    return &m_colorSplineValue;
}

TString CGradientColorPropertyDescription::FormatSplineValue()
{
    TString strNewValue;
    TCHAR buffer[16] = {0};
    int nCount = m_colorSplineValue.m_mapColors.size();
    sprintf(buffer, _T("%x"), nCount);
    strNewValue += buffer;
    for (auto itr : m_colorSplineValue.m_mapColors)
    {
        ZeroMemory(buffer, sizeof(buffer));
        sprintf(buffer, _T("%f"), itr.first);
        strNewValue += _T("@");
        strNewValue += buffer;
        ZeroMemory(buffer, sizeof(buffer));
        sprintf(buffer, _T("%x"), (size_t)itr.second);
        strNewValue += _T("@");
        strNewValue += buffer;
    }
    return strNewValue;
}
