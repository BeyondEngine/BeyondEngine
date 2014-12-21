#include "stdafx.h"
#include "LanguageTextPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include <wx/propgrid/propgrid.h>
#include "Language/LanguageManager.h"
#include "EngineEditor.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProxy.h"

CLanguageTextPropertyDescription::CLanguageTextPropertyDescription(CSerializer* pSerializer)
: super(eRPT_LanguageText)
{
    TString strValue;
    int nType = 0xFFFFFFFF;
    (*pSerializer) >> nType;
    if (nType != 0xFFFFFFFF)
    {
        const std::vector<SEnumData*>* pEnumStringData = NULL;
        bool bFindEnum = CEnumStrGenerator::GetInstance()->GetEnumValueData(_T("ELanguageTextType"), pEnumStringData);
        BEATS_ASSERT(bFindEnum, _T("Can't find ELanguageTextType enum type in language property!"));
        if (bFindEnum)
        {
            for (uint32_t i = 0; i < pEnumStringData->size(); ++i)
            {
                if (pEnumStringData->at(i)->m_value == nType)
                {
                    strValue = pEnumStringData->at(i)->m_str;
                    break;
                }
            }
        }
    }
    InitializeValue<std::string>(strValue);
}

CLanguageTextPropertyDescription::CLanguageTextPropertyDescription(const CLanguageTextPropertyDescription& rRef)
: super(rRef)
{
    std::string strValue;
    InitializeValue<std::string>(strValue);
}

CLanguageTextPropertyDescription::~CLanguageTextPropertyDescription()
{
    DestroyValue<std::string>();
}

bool CLanguageTextPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*result*/)
{
    return true;
}

wxPGProperty* CLanguageTextPropertyDescription::CreateWxProperty()
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

void CLanguageTextPropertyDescription::SetValue(wxVariant& value, bool bSaveValue /*= true*/)
{
    TString strNewValue = value.GetString().ToUTF8();
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CLanguageTextPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CLanguageTextPropertyDescription::IsDataSame(bool bWithDefaultOrXML)
{
    bool bRet = _tcscmp(((TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)])->c_str(), ((TString*)m_valueArray[eVT_CurrentValue])->c_str()) == 0;
    return bRet;
}

CPropertyDescriptionBase* CLanguageTextPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CLanguageTextPropertyDescription(*this);
    return pNewProperty;
}

void CLanguageTextPropertyDescription::GetValueAsChar(EValueType type, char* pOut) const
{
    std::string* pStr = (std::string*)m_valueArray[type];
    _tcscpy(pOut, pStr->c_str());
}

bool CLanguageTextPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    ((TString*)pOutValue)->assign(pIn);
    return true;
}

void CLanguageTextPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    int nType = 0xFFFFFFFF;
    const TString* pCurStringValue = (const TString*)m_valueArray[eValueType];
    if( !pCurStringValue->empty() )
    {
        nType = 0xFFFFFFFE;
        std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
        auto iter = languageMap.find(*pCurStringValue);
        bool bFoundString = iter != languageMap.end();
        BEATS_ASSERT(bFoundString, "Language text %s can't be found in component %d!", pCurStringValue->c_str(), GetRootOwner()->GetId());
        if (bFoundString)
        {
            nType = std::distance(languageMap.begin(), iter);
        }
    }
    std::set<TString>& exportLanguage = CLanguageManager::GetInstance()->GetExportLanguageList();
    if (CComponentProxyManager::GetInstance()->IsExporting())
    {
        exportLanguage.insert(*pCurStringValue);
    }
    serializer << nType;
}

void CLanguageTextPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    int nType = 0xFFFFFFFF;
    serializer >> nType;
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    if (languageMap.size() > (uint32_t)nType)
    {
        auto iter = languageMap.begin();
        std::advance(iter, nType);
        *(TString*)m_valueArray[eValueType] = iter->second[CLanguageManager::GetInstance()->GetCurrentLanguage()];
    }
}

