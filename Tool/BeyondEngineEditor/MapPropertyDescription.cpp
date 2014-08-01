#include "stdafx.h"
#include "MapPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "EngineEditor.h"
#include "ListPropertyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/TinyXML/tinyxml.h"
#include <wx/propgrid/propgrid.h>
#include "StringPropertyDescription.h"

static const TString EMPTY_STRING = _T("Empty");

CMapPropertyDescription::CMapPropertyDescription(CSerializer* pSerializer)
    : super(eRPT_Map)
    , m_pKeyPropertyTemplate(NULL)
    , m_pValuePropertyTemplate(NULL)
    , m_uValuePtrGuid(0xFFFFFFFF)
{
    if (pSerializer != NULL)
    {
        EReflectPropertyType keyType;
        (*pSerializer) >> keyType;
        BEATS_ASSERT(keyType != eRPT_Ptr, _T("Key can't be ePT_Ptr! It's not implemented yet!"));
        m_pKeyPropertyTemplate = CComponentProxyManager::GetInstance()->CreateProperty(keyType, pSerializer);
        EReflectPropertyType valueType;
        (*pSerializer) >> valueType;
        m_pValuePropertyTemplate = CComponentProxyManager::GetInstance()->CreateProperty(valueType, pSerializer);
    }
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::CMapPropertyDescription(const CMapPropertyDescription& rRef)
    : super(rRef)
    , m_pKeyPropertyTemplate(rRef.m_pKeyPropertyTemplate->Clone(true))
    , m_pValuePropertyTemplate(rRef.m_pValuePropertyTemplate->Clone(true))
    , m_uValuePtrGuid(rRef.m_uValuePtrGuid)
{
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::~CMapPropertyDescription()
{
    BEATS_SAFE_DELETE(m_pKeyPropertyTemplate);
    BEATS_SAFE_DELETE(m_pValuePropertyTemplate);
    DestroyValue<TString>();
}

wxPGProperty* CMapPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
    pProperty->SetClientData(this);
    pProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetPtrEditor());
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    pProperty->SetValue(var);
    wxVariant defaultVar(EMPTY_STRING);
    pProperty->SetDefaultValue(defaultVar);
    pProperty->SetModifiedStatus(m_pChildren->size() > 0);
    return pProperty;
}

void CMapPropertyDescription::SetValue( wxVariant& value, bool bSaveValue/* = true*/ )
{
    TString strNewValue = value.GetString();
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CMapPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CMapPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    return bWithDefaultOrXML && m_pChildren->size() == 0;
}

bool CMapPropertyDescription::IsContainerProperty()
{
    return true;
}

CPropertyDescriptionBase* CMapPropertyDescription::AddChild(CPropertyDescriptionBase* pChild)
{
    if (pChild == NULL)
    {
        pChild = CreateInstance();
    }
    super::AddChild(pChild);
    ResetName();

    return pChild;
}

CPropertyDescriptionBase* CMapPropertyDescription::CreateInstance()
{
    TCHAR szChildName[32];
    _stprintf(szChildName, _T("Map_Child_%d"), m_pChildren->size());
    SBasicPropertyInfo basicInfo = *(GetBasicInfo().Get());
    basicInfo.m_displayName.assign(szChildName);
    basicInfo.m_variableName.assign(szChildName);
    basicInfo.m_bEditable = false;

    CPropertyDescriptionBase* pRet = CComponentProxyManager::GetInstance()->CreateProperty(eRPT_Str, NULL);
    pRet->Initialize();
    pRet->SetBasicInfo(basicInfo);
    pRet->SetOwner(this->GetOwner());
    down_cast<CStringPropertyDescription*>(pRet)->SetIsMapStringProperty(true);

    basicInfo.m_bEditable = true;// Only label can't be changed.
    CPropertyDescriptionBase* pKey = m_pKeyPropertyTemplate->Clone(false);
    pKey->Initialize();
    basicInfo.m_displayName.assign(_T("Key"));
    basicInfo.m_variableName.assign(_T("Key"));
    pKey->SetBasicInfo(basicInfo);
    pKey->SetOwner(this->GetOwner());

    CPropertyDescriptionBase* pValue = m_pValuePropertyTemplate->Clone(false);
    basicInfo.m_displayName.assign(_T("Value"));
    basicInfo.m_variableName.assign(_T("Value"));
    pValue->SetBasicInfo(basicInfo);
    pValue->SetOwner(this->GetOwner());
    pValue->Initialize();

    pRet->AddChild(pKey);
    pRet->AddChild(pValue);
    return pRet;
}

bool CMapPropertyDescription::DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder)
{
    BEATS_ASSERT((*m_pChildren).size() > 0 && pProperty != NULL);
    bool bRet = super::DeleteChild(pProperty, bKeepOrder);
    if (bRet)
    {
        ResetChildName();
        ResetName();
    }
    BEATS_ASSERT(bRet, _T("Can't Find the property to delete!"));
    return bRet;
}

void CMapPropertyDescription::DeleteAllChild()
{
    super::DeleteAllChild();
    ResetName();
}


void CMapPropertyDescription::ResetChildName()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), i);
        (*m_pChildren)[i]->GetBasicInfo()->m_displayName.assign(szChildName);
        (*m_pChildren)[i]->GetBasicInfo()->m_variableName.assign(szChildName);
    }
}


void CMapPropertyDescription::ResetName()
{
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    SetValue(var, false);
}

void CMapPropertyDescription::GetCurrentName( TCHAR* pszName )
{
    if (m_pChildren->size() == 0)
    {
        _stprintf(pszName, EMPTY_STRING.c_str());
    }
    else
    {
        _stprintf(pszName, _T("%dx%s"), m_pChildren->size(), szPropertyTypeStr[(*m_pChildren)[0]->GetType()]);
    }
}

void CMapPropertyDescription::LoadFromXML( TiXmlElement* pNode )
{
    super::LoadFromXML(pNode);
    TiXmlElement* pVarElement = pNode->FirstChildElement("VariableNode");
    while (pVarElement != NULL)
    {
        int iVarType = 0;
        pVarElement->Attribute("Type", &iVarType);
        BEATS_ASSERT(iVarType == eRPT_Str);
        if (iVarType == eRPT_Str)
        {
            CPropertyDescriptionBase* pNewProperty = AddChild(NULL);
            BEATS_ASSERT(pNewProperty != 0, _T("Create property failed when load from xml for list property description."));
            if (pNewProperty != NULL)
            {
                TiXmlElement* pChildVarElement = pVarElement->FirstChildElement("VariableNode");
                pNewProperty->GetChild(0)->LoadFromXML(pChildVarElement);
                pChildVarElement = pChildVarElement->NextSiblingElement("VariableNode");
                pNewProperty->GetChild(1)->LoadFromXML(pChildVarElement);
            }
        }
        else
        {
            BEATS_WARNING(false, _T("UnMatch type of property!"));
        }
        pVarElement = pVarElement->NextSiblingElement("VariableNode");
    }
}

CPropertyDescriptionBase* CMapPropertyDescription::Clone(bool bCloneValue)
{
    CMapPropertyDescription* pNewProperty = static_cast<CMapPropertyDescription*>(super::Clone(bCloneValue));
    if (bCloneValue)
    {
        for (size_t i = 0; i < m_pChildren->size(); ++i)
        {
            CPropertyDescriptionBase* pPropertyBase = (*m_pChildren)[i];
            CPropertyDescriptionBase* pNewChildPropertyBase = pPropertyBase->Clone(true);
            BEATS_ASSERT(pPropertyBase->GetChildrenCount() == 2, _T("Map property must contain two property childern for each element."));
            CPropertyDescriptionBase* pKeyProperty = pPropertyBase->GetChild(0);
            CPropertyDescriptionBase* pNewKeyProperty = pKeyProperty->Clone(true);
            CPropertyDescriptionBase* pValueProperty = pPropertyBase->GetChild(1);
            CPropertyDescriptionBase* pNewValueProperty = pValueProperty->Clone(true);
            pNewChildPropertyBase->AddChild(pNewKeyProperty);
            pNewChildPropertyBase->AddChild(pNewValueProperty);
            pNewProperty->AddChild(pNewChildPropertyBase);
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CMapPropertyDescription::CreateNewInstance()
{
    CMapPropertyDescription* pNewProperty = new CMapPropertyDescription(*this);
    return pNewProperty;
}

void CMapPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    CStringHelper::GetInstance()->ConvertToCHAR(((TString*)GetValue(type))->c_str(), pOut, 128);
}

bool CMapPropertyDescription::GetValueByTChar(const TCHAR* /*pIn*/, void* /*pOutValue*/)
{
    // Do nothing.
    return true;
}

void CMapPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << m_pKeyPropertyTemplate->GetType();
    serializer << m_pValuePropertyTemplate->GetType();
    serializer << m_pChildren->size();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_ASSERT((*m_pChildren)[i]->GetChildrenCount() == 2, _T("An element of map must contain two children!"));
        (*m_pChildren)[i]->GetChild(0)->Serialize(serializer, eValueType);
        (*m_pChildren)[i]->GetChild(1)->Serialize(serializer, eValueType);
    }
}

void CMapPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    DeleteAllChild();
    size_t uChildrenCount = 0;
    serializer >> uChildrenCount;
    for (size_t i = 0; i < uChildrenCount; ++i)
    {
        (*m_pChildren)[i]->GetChild(0)->Deserialize(serializer, eValueType);
        (*m_pChildren)[i]->GetChild(1)->Deserialize(serializer, eValueType);
    }
}

bool CMapPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*parameterUnit*/)
{
    return true;
}