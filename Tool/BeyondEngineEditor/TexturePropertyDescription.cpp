#include "stdafx.h"
#include "TexturePropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "EngineEditor.h"

CTexturePropertyDescription::CTexturePropertyDescription(CSerializer* pSerializer)
    : super(eRPT_Texture)
{
    TString strValue;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> strValue;
    }
    InitializeValue(strValue);
}

CTexturePropertyDescription::CTexturePropertyDescription(const CTexturePropertyDescription& rRef)
    : super(rRef)
{
    TString strValue;
    InitializeValue<TString>(strValue);
}

CTexturePropertyDescription::~CTexturePropertyDescription()
{
    DestroyValue<TString>();
}

bool CTexturePropertyDescription::AnalyseUIParameterImpl( const std::vector<TString>& paramUnit)
{
    BEATS_ASSERT(paramUnit.size() <= 1);
    std::vector<TString> cache;
    for (size_t i = 0; i < paramUnit.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(paramUnit[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            TString value(_T("..."));
            wxVariant var(value);
            SetValue(var, true);
            SetValueWithType(&value, eVT_DefaultValue);
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for bool property %s"), cache[0].c_str());
        }
    }

    return true;
}

wxPGProperty* CTexturePropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, ((TString*)m_valueArray[eVT_CurrentValue])->c_str());
    wxVariant defaultValue(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(defaultValue);
    pProperty->SetClientData(this);
    pProperty->SetModifiedStatus(GetInstanceComponent() != NULL);
    pProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetTextureEditor());
    return pProperty;
}

void CTexturePropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    TString strNewValue = value.GetString();
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }  
}

bool CTexturePropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CTexturePropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = _tcscmp(((TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)])->c_str(), ((TString*)m_valueArray[eVT_CurrentValue])->c_str()) == 0;
    return bRet;
}

CPropertyDescriptionBase* CTexturePropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CTexturePropertyDescription(*this);
    return pNewProperty;
}

void CTexturePropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    TString* pStr = (TString*)m_valueArray[type];
    CStringHelper::GetInstance()->ConvertToCHAR(pStr->c_str(), pOut, 10240);
}

bool CTexturePropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    ((TString*)pOutValue)->assign(pIn);
    return true;
}

void CTexturePropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << *(TString*)m_valueArray[eValueType];
}

void CTexturePropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(TString*)m_valueArray[eValueType];
}
