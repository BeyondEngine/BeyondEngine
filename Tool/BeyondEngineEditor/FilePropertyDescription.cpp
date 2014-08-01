#include "stdafx.h"
#include "FilePropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include <wx/propgrid/propgrid.h>

CFilePropertyDescription::CFilePropertyDescription(CSerializer* pSerializer)
    : super(eRPT_File)
{
    TString strValue;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> strValue;
    }
    InitializeValue<TString>(strValue);
}

CFilePropertyDescription::CFilePropertyDescription(const CFilePropertyDescription& rRef)
    : super(rRef)
{
    TString strValue;
    InitializeValue<TString>(strValue);
}

CFilePropertyDescription::~CFilePropertyDescription()
{
    DestroyValue<TString>();
}

bool CFilePropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*result*/)
{
    return true;
}

wxPGProperty* CFilePropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxFileProperty(wxPG_LABEL, wxPG_LABEL, *(TString*)(m_valueArray[eVT_CurrentValue]));
    pProperty->SetClientData(this);
    wxVariant var(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));

    return pProperty;
}

void CFilePropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    TString strNewValue = CFilePathTool::GetInstance()->FileName(value.GetString().c_str());
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CFilePropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CFilePropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = _tcscmp(((TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)])->c_str(), ((TString*)m_valueArray[eVT_CurrentValue])->c_str()) == 0;
    return bRet;
}

CPropertyDescriptionBase* CFilePropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CFilePropertyDescription(*this);
    return pNewProperty;
}

void CFilePropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    TString* pStr = (TString*)m_valueArray[type];
    CStringHelper::GetInstance()->ConvertToCHAR(pStr->c_str(), pOut, 10240);
}

bool CFilePropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    ((TString*)pOutValue)->assign(pIn);
    return true;
}

void CFilePropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << *(TString*)m_valueArray[eValueType];
}

void CFilePropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(TString*)m_valueArray[eValueType];
}
