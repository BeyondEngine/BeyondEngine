#include "stdafx.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "mainapp.h"
#include "ListPropertyDescriptionEx.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/TinyXML/tinyxml.h"

static const TString EMPTY_STRING = _T("Empty");

CListPropertyDescriptionEx::CListPropertyDescriptionEx(CSerializer* pSerializer)
: super(ePT_List)
, m_maxCount(INT_MAX)
, m_bFixCount(false)
{
    if (pSerializer != NULL)
    {
        (*pSerializer) >> m_childType;
        if (m_childType == ePT_Ptr)
        {
            (*pSerializer) >> m_childPtrGuid;
        }
    }
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescriptionEx::CListPropertyDescriptionEx(const CListPropertyDescriptionEx& rRef)
: super(rRef)
, m_maxCount(rRef.m_maxCount)
, m_bFixCount(rRef.m_bFixCount)
, m_childPtrGuid(rRef.m_childPtrGuid)
, m_childType(rRef.m_childType)
{
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescriptionEx::~CListPropertyDescriptionEx()
{
    DestoryValue<TString>();
}

bool CListPropertyDescriptionEx::AnalyseUIParameterImpl(const std::vector<TString>& result, bool /* bSerializePhase= false */)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), _T(":"), cache, false);
        BEATS_ASSERT(cache.size() == 2 || cache.size() == 1);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxCount]) == 0)
        {
            m_maxCount = _tstoi(cache[1].c_str());
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
            // Do nothing, because it is only for UI text display.
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for List property %s"), cache[0].c_str());
        }
    }
    return true;
}

wxPGProperty* CListPropertyDescriptionEx::CreateWxProperty()
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


void CListPropertyDescriptionEx::SetValue( wxVariant& value, bool bSaveValue/* = true*/ )
{
    TString strNewValue = value.GetString();
    SetValue(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&strNewValue, eVT_SavedValue);
    }
}

void CListPropertyDescriptionEx::SetValue( void* pValue, EValueType type )
{
    *(TString*)m_valueArray[type] = *(TString*)pValue;
}

bool CListPropertyDescriptionEx::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = false;
    if (bWithDefaultOrXML)
    {
        bRet = m_pChildren->size() == 0;
    }
    else
    {
        bRet = false;
    }
    return bRet;
}


CPropertyDescriptionBase* CListPropertyDescriptionEx::AddListChild()
{
    CPropertyDescriptionBase* pNewChild = CreateInstance();
    if (pNewChild)
    {
        AddChild(pNewChild);
        ResetName();
    }
    return pNewChild;
}

void CListPropertyDescriptionEx::AddListChild(CPropertyDescriptionBase* pChild)
{
    if (pChild)
    {
        AddChild(pChild);
        ResetName();
    }
}

CPropertyDescriptionBase* CListPropertyDescriptionEx::CreateInstance()
{
    CPropertyDescriptionBase* bRet = NULL;
    if (m_pChildren->size() < m_maxCount)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), m_pChildren->size());
        SBasicPropertyInfo basicInfo = GetBasicInfo();
        basicInfo.m_displayName.assign(szChildName);
        basicInfo.m_variableName.assign(szChildName);
        CSerializer serializer;
        serializer << m_childPtrGuid;
        CPropertyDescriptionBase* pProperty = CComponentManager::GetInstance()->CreateProperty(m_childType, m_childType == ePT_Ptr ? &serializer : NULL);
        pProperty->Initialize();
        pProperty->SetBasicInfo(basicInfo);
        pProperty->SetOwner(this->GetOwner());
        pProperty->SetParent(this);
        bRet = pProperty;
    }
    return bRet;
}


void CListPropertyDescriptionEx::DeleteListChild(CPropertyDescriptionBase* pProperty)
{
    BEATS_ASSERT((*m_pChildren).size() > 0 && pProperty != NULL);
    bool bRet = DeleteChild(pProperty, true);
    if (bRet)
    {
        ResetChildName();
        ResetName();
    }
    BEATS_ASSERT(bRet, _T("Can't Find the property to delete!"));
}


void CListPropertyDescriptionEx::DeleteAllListChild()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_SAFE_DELETE((*m_pChildren)[i]);
    }
    m_pChildren->clear();
    ResetName();
}


void CListPropertyDescriptionEx::ResetChildName()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), i);
        (*m_pChildren)[i]->GetBasicInfo().m_displayName.assign(szChildName);
        (*m_pChildren)[i]->GetBasicInfo().m_variableName.assign(szChildName);
    }
}


void CListPropertyDescriptionEx::ResetName()
{
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    SetValue(var, false);
}

void CListPropertyDescriptionEx::GetCurrentName( TCHAR* pszName )
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


void CListPropertyDescriptionEx::LoadFromXML( TiXmlElement* pNode )
{
    super::LoadFromXML(pNode);
    TiXmlElement* pVarElement = pNode->FirstChildElement("VariableNode");
    while (pVarElement != NULL)
    {
        int iVarType = 0;
        pVarElement->Attribute("Type", &iVarType);
        if (iVarType == m_childType)
        {
            CPropertyDescriptionBase* pNewProperty = AddListChild();
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

CPropertyDescriptionBase* CListPropertyDescriptionEx::Clone(bool bCloneValue)
{
    CListPropertyDescriptionEx* pNewProperty = static_cast<CListPropertyDescriptionEx*>(super::Clone(bCloneValue));
    if (bCloneValue)
    {
        for (size_t i = 0; i < m_pChildren->size(); ++i)
        {
            CPropertyDescriptionBase* pPropertyBase = (*m_pChildren)[i];
            CPropertyDescriptionBase* pNewChildPropertyBase = pPropertyBase->Clone(true);
            pNewProperty->AddListChild(pNewChildPropertyBase);
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CListPropertyDescriptionEx::CreateNewInstance()
{
    CListPropertyDescriptionEx* pNewProperty = new CListPropertyDescriptionEx(*this);
    return pNewProperty;
}

void CListPropertyDescriptionEx::GetValueAsChar( EValueType type, char* pOut )
{
    CStringHelper::GetInstance()->ConvertToCHAR(((TString*)GetValue(type))->c_str(), pOut, 128);
}

void CListPropertyDescriptionEx::Serialize( CSerializer& serializer )
{
    serializer << m_childType;
    serializer << m_pChildren->size();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->Serialize(serializer);
    }
}
