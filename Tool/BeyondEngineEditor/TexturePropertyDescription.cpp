#include "stdafx.h"
#include "TexturePropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "EngineEditor.h"
#include "Resource/ResourceManager.h"
#include "EditorMainFrame.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Render/TextureAtlas.h"

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
    for (uint32_t i = 0; i < paramUnit.size(); ++i)
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
    strNewValue = CStringHelper::GetInstance()->ToLower(strNewValue);
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
    _tcscpy(pOut, pStr->c_str());
}

bool CTexturePropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    ((TString*)pOutValue)->assign(CStringHelper::GetInstance()->ToLower(pIn));
    return true;
}

void CTexturePropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    const TString& strFileName = *(TString*)m_valueArray[eValueType];
    serializer << *(TString*)m_valueArray[eValueType];
    if (CComponentProxyManager::GetInstance()->IsExporting() && CComponentProxyManager::GetInstance()->IsCheckUselessResource())
    {
        if (!strFileName.empty())
        {
            CPropertyDescriptionBase* pRootProperty = this;
            while (pRootProperty->GetParent() != nullptr)
            {
                pRootProperty = pRootProperty->GetParent();
            }
            uint32_t uRootOwnerId = pRootProperty->GetOwner()->GetId();
            std::vector<TString> data;
            CStringHelper::GetInstance()->SplitString(strFileName.c_str(), "@", data, true);
            BEATS_ASSERT(data.size() == 2 && !data[0].empty(), "Wrong texture frag value format %s!", strFileName.c_str());
            CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
            TString strFullPath = CResourceManager::GetInstance()->GetFullPath(data[0], eRT_Texture);
            bool bRet = CFilePathTool::GetInstance()->Exists(strFullPath.c_str());
            if (bRet)
            {
                if (data.size() == 2)
                {
                    SharePtr<CTextureAtlas> pAtlas = CResourceManager::GetInstance()->GetResource<CTextureAtlas>(data[0]);
                    pAtlas->LoadCheckList();
                    std::map<TString, char>& fragCheckMap = CTextureAtlas::m_fragCheckList[data[0]];
                    if (fragCheckMap.find(data[1]) != fragCheckMap.end())
                    {
                        fragCheckMap[data[1]] = 1;
                    }
                    else
                    {
                        fragCheckMap[data[1]] = -1;
                        TCHAR szBuffer[1024];
                        TString unicodeStr = wxString::FromUTF8(pRootProperty->GetBasicInfo()->m_displayName.c_str());
                        _stprintf(szBuffer, "%d#%s", uRootOwnerId, unicodeStr.c_str());
                        CTextureAtlas::m_fragMissingInfo[strFileName].insert(szBuffer);
                    }
                }
                pMainFrame->AddExportFileFullPathList(strFullPath);
                rapidxml::file<> fdoc(strFullPath.c_str());
                rapidxml::xml_document<> ImageXML;
                try
                {
                    ImageXML.parse<rapidxml::parse_default>(fdoc.data());
                }
                catch (rapidxml::parse_error err)
                {
                    BEATS_ASSERT(false, _T("Load config file %s faled!/n%s/n"), strFullPath, err.what());
                }
                rapidxml::xml_node<>* pRootElement = ImageXML.first_node("Imageset");
                TString strImageFullPath = CResourceManager::GetInstance()->GetFullPath(pRootElement->first_attribute("Imagefile")->value(), eRT_Texture);
                pMainFrame->AddExportFileFullPathList(strImageFullPath);
            }
            else
            {
                TCHAR szBuffer[1024];
                TString unicodeStr = wxString::FromUTF8(pRootProperty->GetBasicInfo()->m_displayName.c_str());
                _stprintf(szBuffer, "ÎÄ¼þÈ±Ê§£º%d#%s", uRootOwnerId, unicodeStr.c_str());
                CTextureAtlas::m_fragMissingInfo[data[0]].insert(szBuffer);
            }
        }
    }
}

void CTexturePropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    serializer >> *(TString*)m_valueArray[eValueType];
}
