#include "stdafx.h"
#include "Component/Component/ComponentInstanceManager.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProxy.h"
#include "DependencyDescription.h"
#include "DependencyDescriptionLine.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/Component/ComponentInstance.h"
#include "Component/Component/ComponentProject.h"

CDependencyDescription::CDependencyDescription(EDependencyType type, uint32_t dependencyGuid, CComponentProxy* pOwner, uint32_t uIndex, bool bIsList)
: m_type(type)
, m_changeAction(eDCA_Count)
, m_pChangeActionProxy(NULL)
, m_uIndex(uIndex)
, m_pOwner(pOwner)
, m_uDependencyGuid(dependencyGuid)
, m_bHideRender(false)
, m_bIsListType(bIsList)
{
    m_pOwner->AddDependencyDescription(this);
}

CDependencyDescription::~CDependencyDescription()
{
    // In CDependencyDescriptionLine's destructor function, it will reduce the number in m_pBeConnectedDependencyLines.
    while (m_dependencyLine.size() > 0)
    {
        CDependencyDescriptionLine* pLine = m_dependencyLine[0];
        BEATS_SAFE_DELETE(pLine);
    }
}

CDependencyDescriptionLine* CDependencyDescription::GetDependencyLine( uint32_t uIndex /*= 0*/ ) const
{
    BEATS_ASSERT(uIndex < m_dependencyLine.size());
    BEATS_ASSERT(uIndex == 0  || (uIndex > 0 && m_bIsListType), _T("Get denpendency by index %d is only available for list type!"), uIndex);
    return m_dependencyLine[uIndex];
}

CDependencyDescriptionLine* CDependencyDescription::SetDependency( uint32_t uIndex, CComponentProxy* pComponent )
{
    CDependencyDescriptionLine* pRet = NULL;
    BEATS_ASSERT(uIndex < m_dependencyLine.size());
    BEATS_ASSERT(pComponent != m_pOwner, _T("Component can't depends on itself!"));
    BEATS_ASSERT(uIndex == 0  || (uIndex > 0 && m_bIsListType), _T("Set denpendency by index %d is only available for list type!"), uIndex);
    if (pComponent != m_pOwner)
    {
        m_dependencyLine[uIndex]->SetConnectComponent(pComponent);
        pRet = m_dependencyLine[uIndex];
    }
    if (!m_bIsListType)
    {
        m_changeAction = eDCA_Change;
        m_pChangeActionProxy = pComponent;
        OnDependencyChanged();
    }
    return pRet;
}

uint32_t CDependencyDescription::GetDependencyLineCount() const
{
    BEATS_ASSERT(m_dependencyLine.size() <= 1 || m_bIsListType);
    return (uint32_t)m_dependencyLine.size();
}

CDependencyDescriptionLine* CDependencyDescription::AddDependency( CComponentProxy* pComponentInstance )
{
    CDependencyDescriptionLine* pRet = NULL;

#ifdef _DEBUG
    for (uint32_t i = 0; i < m_dependencyLine.size(); ++i)
    {
        BEATS_ASSERT(pComponentInstance == NULL || pComponentInstance != m_dependencyLine[i]->GetConnectedComponent(), _T("The dependency is already in its list!"));
    }
#endif
    BEATS_ASSERT(pComponentInstance != m_pOwner);
    if (pComponentInstance != m_pOwner)
    {
        BEATS_ASSERT(m_dependencyLine.size() == 0 || m_bIsListType);
        pRet = new CDependencyDescriptionLine(this, (uint32_t)m_dependencyLine.size(), pComponentInstance);
        m_dependencyLine.push_back(pRet);
        m_changeAction = eDCA_Add;
        m_pChangeActionProxy = pComponentInstance;
        OnDependencyChanged();
    }
    return pRet;
}

void CDependencyDescription::RemoveDependencyLine(CDependencyDescriptionLine* pLine)
{
    BEATS_ASSERT(pLine->GetOwnerDependency() == this);
    uint32_t uPos = pLine->GetIndex();
    RemoveDependencyByIndex(uPos);
}

void CDependencyDescription::RemoveDependencyByIndex( uint32_t uIndex )
{
    std::vector<CDependencyDescriptionLine*>::iterator iter = m_dependencyLine.begin();
    advance(iter, uIndex);
    BEATS_ASSERT(*iter == m_dependencyLine[uIndex]);
    m_pChangeActionProxy = (*iter)->GetConnectedComponent();
    m_dependencyLine.erase(iter);

    for (uint32_t i = uIndex; i < m_dependencyLine.size(); ++i)
    {
        m_dependencyLine[i]->SetIndex(i);
    }
    m_changeAction = eDCA_Delete;
    OnDependencyChanged();
}

void CDependencyDescription::SwapLineOrder(uint32_t uSourceIndex, uint32_t uTargetIndex)
{
    BEATS_ASSERT(uSourceIndex != uTargetIndex);
    BEATS_ASSERT(uSourceIndex < m_dependencyLine.size());
    BEATS_ASSERT(uTargetIndex < m_dependencyLine.size());

    CDependencyDescriptionLine* pSourceLine = m_dependencyLine[uSourceIndex];
    CDependencyDescriptionLine* pTargetLine = m_dependencyLine[uTargetIndex];
    BEATS_ASSERT(pSourceLine != pTargetLine);
    CComponentProxy* pSourceProxy = pSourceLine->GetConnectedComponent();
    pSourceLine->SetConnectComponent(pTargetLine->GetConnectedComponent());
    pTargetLine->SetConnectComponent(pSourceProxy);
    m_changeAction = eDCA_Ordered;
    m_pChangeActionProxy = pSourceProxy;
    OnDependencyChanged();
}

void CDependencyDescription::SaveToXML(rapidxml::xml_node<>* pParentNode)
{
    if (m_dependencyLine.size() == 0)
    {
        if (m_type == eDT_Strong && m_pOwner->GetId() != -1)
        {
            TCHAR szInfo[10240];
            _stprintf(szInfo, _T("Component %s with Id:%d, Unset strong dependency!"), m_pOwner->GetClassStr(), m_pOwner->GetId());
            MessageBox(BEYONDENGINE_HWND, szInfo, _T("Unset strong dependency"), MB_OK);
        }
    }
    else
    {
        rapidxml::xml_document<>* pDoc = pParentNode->document();
        rapidxml::xml_node<>* pDependencyElement = pDoc->allocate_node(rapidxml::node_element, "Dependency");
        pDependencyElement->append_attribute(pDoc->allocate_attribute("VariableName", pDoc->allocate_string(m_variableName.c_str())));
        TCHAR szBuffer[64];
        for (uint32_t i = 0; i < m_dependencyLine.size(); ++i)
        {
            rapidxml::xml_node<>* pDependencyNodeElement = pDoc->allocate_node(rapidxml::node_element, "DependencyNode");
            CComponentProxy* pProxy = m_dependencyLine[i]->GetConnectedComponent();
            _stprintf(szBuffer, "%d", pProxy->GetId());
            pDependencyNodeElement->append_attribute(pDoc->allocate_attribute("Id", pDoc->allocate_string(szBuffer)));
            char szGUIDHexStr[32] = { 0 };
            sprintf(szGUIDHexStr, "0x%x", pProxy->GetGuid());
            pDependencyNodeElement->append_attribute(pDoc->allocate_attribute("Guid", pDoc->allocate_string(szGUIDHexStr)));
            pDependencyElement->append_node(pDependencyNodeElement);
        }
        pParentNode->append_node(pDependencyElement);
    }
}

void CDependencyDescription::LoadFromXML(rapidxml::xml_node<>* pNode)
{
    rapidxml::xml_node<>* pDependencyElement = pNode->first_node("Dependency");
    while (pDependencyElement != NULL)
    {
        const char* szVariableName = pDependencyElement->first_attribute("VariableName")->value();
        if (_tcscmp(szVariableName, m_variableName.c_str()) != 0)
        {
            pDependencyElement = pDependencyElement->next_sibling("Dependency");
        }
        else
        {
            rapidxml::xml_node<>* pDependencyNodeElement = pDependencyElement->first_node("DependencyNode");
            while (pDependencyNodeElement != NULL)
            {
                const char* szGuid = pDependencyNodeElement->first_attribute("Guid")->value();
                char* pEnd = NULL;
                uint32_t uGuid = strtoul(szGuid, &pEnd, 16);
                const char* szId = pDependencyNodeElement->first_attribute("Id")->value();
                uint32_t uId = (uint32_t)atoi(szId);
                bool bIsParent = CComponentProxyManager::GetInstance()->IsParent(m_uDependencyGuid, uGuid);
                BEATS_ASSERT(bIsParent, _T("Dependency Not match in component %d"), m_pOwner->GetId());
                if (bIsParent)
                {
                    CComponentProxy* pComponent = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uId, uGuid));
                    AddDependency(pComponent);
                    if (pComponent == NULL)
                    {
                        BEATS_ASSERT(m_dependencyLine.size() >= 1);
                        CComponentProxyManager::GetInstance()->AddDependencyResolver(this, (uint32_t)m_dependencyLine.size() - 1, uGuid, uId, NULL, m_bIsListType);
                    }
                }
                pDependencyNodeElement = pDependencyNodeElement->next_sibling("DependencyNode");
            }
            break;
        }
    }
}

void CDependencyDescription::SetOwner( CComponentProxy* pOwner )
{
    BEATS_ASSERT(m_pOwner == NULL);
    m_pOwner = pOwner;
}

const TCHAR* CDependencyDescription::GetDisplayName()
{
    return m_displayName.c_str();
}

void CDependencyDescription::SetDisplayName( const TCHAR* pszName )
{
    m_displayName.assign(pszName);
}

const TCHAR* CDependencyDescription::GetVariableName()
{
    return m_variableName.c_str();
}

void CDependencyDescription::SetVariableName(const TCHAR* pszName)
{
    m_variableName.assign(pszName);
}

EDependencyType CDependencyDescription::GetType()
{
    return m_type;
}

CComponentProxy* CDependencyDescription::GetOwner()
{
    return m_pOwner;
}
void CDependencyDescription::Hide()
{
    m_bHideRender = true;
}

void CDependencyDescription::Show()
{
    m_bHideRender = false;
}

bool CDependencyDescription::IsVisible() const
{
    return !m_bHideRender;
}

bool CDependencyDescription::IsListType() const
{
    return m_bIsListType;
}

bool CDependencyDescription::IsInDependency( CComponentProxy* pComponentInstance )
{
    bool bRet = false;
    for (uint32_t i = 0; i < m_dependencyLine.size(); ++i)
    {
        if (m_dependencyLine[i]->GetConnectedComponent() == pComponentInstance)
        {
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool CDependencyDescription::IsMatch( CComponentProxy* pDependencyComponent )
{
    uint32_t uCurGuid = pDependencyComponent->GetGuid();
    bool bMatch = uCurGuid == m_uDependencyGuid;
    if (!bMatch)
    {
        std::set<uint32_t> result;
        CComponentProxyManager::GetInstance()->QueryDerivedClass(m_uDependencyGuid, result, true);
        bMatch = result.find(uCurGuid) != result.end();
    }
    return bMatch;
}

void CDependencyDescription::Serialize(CSerializer& serializer)
{
    uint32_t uLineCount = this->GetDependencyLineCount();
    serializer << uLineCount;
    if (CComponentProxyManager::GetInstance()->IsExporting())
    {
        TCHAR szBuffer[1024];
        if (uLineCount == 0 && m_type == eDT_Strong)
        {
            _stprintf(szBuffer, "Strong Dependency of component %d %s is not connected!", m_pOwner->GetId(), m_pOwner->GetClassStr());
            MessageBox(BEYONDENGINE_HWND, szBuffer, "Dependency Error", MB_OK);
        }
        if (uLineCount > 1 && !m_bIsListType)
        {
            _stprintf(szBuffer, "none-list dependency can only get one conection in component %d %s.", m_pOwner->GetId(), m_pOwner->GetClassStr());
            MessageBox(BEYONDENGINE_HWND, szBuffer, "Dependency Error", MB_OK);
        }
    }
    for (uint32_t j = 0; j < uLineCount; ++j)
    {
        CComponentProxy* pConnectedComponent = this->GetDependencyLine(j)->GetConnectedComponent();
        BEATS_ASSERT(pConnectedComponent != NULL);
        serializer << pConnectedComponent->GetId();
        serializer << pConnectedComponent->GetGuid();
    }
}

void CDependencyDescription::GetCurrActionParam(EDependencyChangeAction& action, CComponentProxy*& pProxy)
{
    action = m_changeAction;
    pProxy = m_pChangeActionProxy;
}

void CDependencyDescription::OnDependencyChanged()
{
    if (this->GetOwner()->IsInitialized())
    {
        bool bIsReady = true;
        for (uint32_t i = 0; i < m_dependencyLine.size(); ++i)
        {
            if (m_dependencyLine[i]->GetConnectedComponent() == NULL)
            {
                bIsReady = false;
                break;
            }
        }
        if (bIsReady)
        {
            if (GetOwner()->GetHostComponent())
            {
                static CSerializer serializer;
                serializer.Reset();
                Serialize(serializer);
                CDependencyDescription* pOriginalReflectDependency = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();
                CComponentProxyManager::GetInstance()->SetCurrReflectDependency(this);
                BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetCurrReflectProperty() == NULL);
                GetOwner()->GetHostComponent()->ReflectData(serializer);
                CComponentInstanceManager::GetInstance()->ResolveDependency();
                // Restore the content.
                CComponentProxyManager::GetInstance()->SetCurrReflectDependency(pOriginalReflectDependency);
            }
        }
    }
}

uint32_t CDependencyDescription::GetDependencyGuid() const
{
    return m_uDependencyGuid;
}

uint32_t CDependencyDescription::GetIndex() const
{
    return m_uIndex;
}
