#include "stdafx.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "EngineEditor.h"
#include "ListPropertyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/TinyXML/tinyxml.h"
#include <wx/propgrid/propgrid.h>

static const TString EMPTY_STRING = _T("Empty");

CListPropertyDescription::CListPropertyDescription(CSerializer* pSerializer)
: super(eRPT_List)
, m_uMaxCount(INT_MAX)
, m_bGridStyle(false)
, m_bFixCount(false)
, m_pChildTemplate(NULL)
{
    if (pSerializer != NULL)
    {
        EReflectPropertyType childType;
        (*pSerializer) >> childType;
        m_pChildTemplate = CComponentProxyManager::GetInstance()->CreateProperty(childType, pSerializer);
    }
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescription::CListPropertyDescription(const CListPropertyDescription& rRef)
: super(rRef)
, m_bGridStyle(rRef.m_bGridStyle)
, m_uMaxCount(rRef.m_uMaxCount)
, m_bFixCount(rRef.m_bFixCount)
, m_pChildTemplate(rRef.m_pChildTemplate->Clone(true))
{
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescription::~CListPropertyDescription()
{
    BEATS_SAFE_DELETE(m_pChildTemplate);
    DestroyValue<TString>();
}

bool CListPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache, false);
        BEATS_ASSERT(cache.size() == 2 || cache.size() == 1);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxCount]) == 0)
        {
            m_uMaxCount = _tstoi(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_FixCount]) == 0)
        {
            if (_tcsicmp(cache[1].c_str(), _T("true")) == 0)
            {
                m_bFixCount = true;
            }
            else if (_tcsicmp(cache[1].c_str(), _T("false")) == 0)
            {
                m_bFixCount = false;
            }
            else
            {
                BEATS_ASSERT(false, _T("Unknown value for List property %s"), cache[1].c_str());
            }
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            if (cache[1].compare(_T("GridStyle")) == 0)
            {
                m_bGridStyle = true;
                SetShowChildrenInGrid(false);
            }
            else
            {
                BEATS_ASSERT(false, _T("eUIPAT_DefaultValue for List property is not implemented!"));
            }
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for List property %s"), cache[0].c_str());
        }
    }
    return true;
}

wxPGProperty* CListPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
    pProperty->SetClientData(this);
    if (!m_bFixCount)
    {
        CEngineEditor* pEnginEditor = static_cast<CEngineEditor*>(wxApp::GetInstance());
        wxPGEditor* pEditor = m_bGridStyle ? pEnginEditor->GetGridEditor() : pEnginEditor->GetPtrEditor();
        pProperty->SetEditor(pEditor);
    }
    wxString strName = GetCurrentName();
    wxVariant var(strName);
    pProperty->SetValue(var);
    wxVariant defaultVar(EMPTY_STRING);
    pProperty->SetDefaultValue(defaultVar);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    return pProperty;
}

void CListPropertyDescription::SetValue( wxVariant& value, bool bSaveValue/* = true*/ )
{
    TString strNewValue = value.GetString();
    SetValueWithType(&strNewValue, eVT_CurrentValue);

    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CListPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CListPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    ResetName();
    return bWithDefaultOrXML && m_pChildren->size() == 0;
}

bool CListPropertyDescription::IsFixed() const
{
    return m_bFixCount;
}

void CListPropertyDescription::SetFixed(bool bFixedFlag)
{
    m_bFixCount = bFixedFlag;
}

size_t CListPropertyDescription::GetMaxCount() const
{
    return m_uMaxCount;
}

void CListPropertyDescription::SetMaxCount(size_t uMaxCount)
{
    m_uMaxCount = uMaxCount;
}

void CListPropertyDescription::SetTemplateProperty(CPropertyDescriptionBase* pTemplateProperty)
{
    BEATS_SAFE_DELETE(m_pChildTemplate);
    m_pChildTemplate = pTemplateProperty;
}

CPropertyDescriptionBase* CListPropertyDescription::GetTemplateProperty() const
{
    return m_pChildTemplate;
}

CPropertyDescriptionBase* CListPropertyDescription::CreateInstance()
{
    CPropertyDescriptionBase* pRet = NULL;
    if (m_pChildren->size() < m_uMaxCount)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("List_Child_%d"), m_pChildren->size());
        SBasicPropertyInfo basicInfo = *(GetBasicInfo().Get());
        basicInfo.m_displayName.assign(szChildName);
        basicInfo.m_variableName.assign(szChildName);
        CPropertyDescriptionBase* pProperty = m_pChildTemplate->Clone(true);
        pProperty->SetBasicInfo(basicInfo);
        pProperty->SetOwner(this->GetOwner());
        pProperty->SetParent(this);
        pRet = pProperty;
    }
    BEATS_ASSERT(pRet != NULL);
    return pRet;
}

bool CListPropertyDescription::IsContainerProperty()
{
    return true;
}

void CListPropertyDescription::Initialize()
{
    super::Initialize();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        CPropertyDescriptionBase* pPropertyBase = m_pChildren->at(i);
        BEATS_ASSERT(pPropertyBase != NULL);
        pPropertyBase->Initialize();
    }
    ResetName();
}

void CListPropertyDescription::Uninitialize()
{
    super::Uninitialize();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        CPropertyDescriptionBase* pPropertyBase = m_pChildren->at(i);
        BEATS_ASSERT(pPropertyBase != NULL);
        pPropertyBase->Uninitialize();
    }
}

CPropertyDescriptionBase* CListPropertyDescription::AddChild(CPropertyDescriptionBase* pProperty)
{
    if (pProperty == NULL)
    {
        pProperty = CreateInstance();
    }
    super::AddChild(pProperty);
    ResetName();
    return pProperty;
}

bool CListPropertyDescription::DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder)
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

void CListPropertyDescription::DeleteAllChild()
{
    super::DeleteAllChild();
    ResetName();
}

void CListPropertyDescription::ResetChildName()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), i);
        (*m_pChildren)[i]->GetBasicInfo()->m_displayName.assign(szChildName);
        (*m_pChildren)[i]->GetBasicInfo()->m_variableName.assign(szChildName);
    }
}

void CListPropertyDescription::ResetName()
{
    wxString strName = GetCurrentName();
    const TCHAR* pszCurValue = ((TString*)GetValue(eVT_CurrentValue))->c_str();
    if (strName.compare(pszCurValue) != 0)
    {
        wxVariant var(strName);
        SetValue(var, false);
    }
}

TString CListPropertyDescription::GetCurrentName()
{
    wxString strName = EMPTY_STRING;
    if (m_pChildren->size() > 0)
    {
        strName = wxString::Format(_T("%dx%s"), m_pChildren->size(), szPropertyTypeStr[(*m_pChildren)[0]->GetType()]);
    }
    return TString(strName);
}

void CListPropertyDescription::LoadFromXML( TiXmlElement* pNode )
{
    super::LoadFromXML(pNode);
    TiXmlElement* pVarElement = pNode->FirstChildElement("VariableNode");
    while (pVarElement != NULL)
    {
        int iVarType = 0;
        pVarElement->Attribute("Type", &iVarType);
        if (iVarType == m_pChildTemplate->GetType())
        {
            CPropertyDescriptionBase* pNewProperty = AddChild(NULL);
            BEATS_ASSERT(pNewProperty != 0, _T("Create property failed when load from xml for list property description."));
            if (pNewProperty != NULL)
            {
                pNewProperty->LoadFromXML(pVarElement);
            }
        }
        else
        {
            BEATS_WARNING(false, _T("UnMatch type of property!"));
        }
        pVarElement = pVarElement->NextSiblingElement("VariableNode");
    }
}

CPropertyDescriptionBase* CListPropertyDescription::Clone(bool bCloneValue)
{
    CListPropertyDescription* pNewProperty = static_cast<CListPropertyDescription*>(super::Clone(bCloneValue));
    if (bCloneValue)
    {
        for (size_t i = 0; i < m_pChildren->size(); ++i)
        {
            CPropertyDescriptionBase* pPropertyBase = (*m_pChildren)[i];
            CPropertyDescriptionBase* pNewChildPropertyBase = pPropertyBase->Clone(true);
            pNewProperty->AddChild(pNewChildPropertyBase);
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CListPropertyDescription::CreateNewInstance()
{
    CListPropertyDescription* pNewProperty = new CListPropertyDescription(*this);
    return pNewProperty;
}

void CListPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    CStringHelper::GetInstance()->ConvertToCHAR(((TString*)GetValue(type))->c_str(), pOut, 128);
}

bool CListPropertyDescription::GetValueByTChar(const TCHAR* /*pIn*/, void* /*pOutValue*/)
{
    // Do nothing.
    return true;
}

void CListPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << m_pChildren->size();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->Serialize(serializer, eValueType);
    }
}

void CListPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    DeleteAllChild();
    size_t uChildCount = 0;
    serializer >> uChildCount;
    for (size_t i = 0; i < uChildCount; ++i)
    {
        CPropertyDescriptionBase* pChild = AddChild(NULL);
        pChild->Deserialize(serializer, eValueType);
    }
}

