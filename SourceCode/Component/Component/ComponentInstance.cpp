#include "stdafx.h"
#include "ComponentInstance.h"
#include "ComponentInstanceManager.h"
#include "Utility/BeatsUtility/IdManager.h"
#include "ComponentProxy.h"
#include "ComponentProject.h"

CComponentInstance::CComponentInstance()
    : m_uDataPos(0xFFFFFFFF)
    , m_uDataSize(0xFFFFFFFF)
    , m_pReflectComponentOwner(nullptr)
    , m_pProxyComponent(NULL)
    , m_pSyncProxyComponent(NULL)
{

}

CComponentInstance::~CComponentInstance()
{
    if (m_pReflectComponentOwner != nullptr)
    {
#ifdef _DEBUG
        BEATS_ASSERT(m_bReflectPropertyCheck == true, "Can't delte a reflect comopnent manually!");
#endif
        m_pReflectComponentOwner->UnregisterReflectComponent(this);
        m_pReflectComponentOwner = nullptr;
    }
    std::set<CComponentInstance*> bak = m_reflectComponents;
    for (auto iter = bak.begin(); iter != bak.end(); ++iter)
    {
        CComponentInstance* pReflectComponent = *iter;
        BEATS_ASSERT(pReflectComponent->GetReflectOwner() == this);
        BEATS_ASSERT(!pReflectComponent->IsLoaded() && !pReflectComponent->IsInitialized());
        BEATS_ASSERT(pReflectComponent->GetId() == 0xFFFFFFFF);
#ifdef _DEBUG
        pReflectComponent->m_bReflectPropertyCheck = true;
#endif
        BEATS_SAFE_DELETE(pReflectComponent);
    }
    BEATS_ASSERT(m_reflectComponents.size() == 0, "All reflect components should be deleted!");
    SetSyncProxyComponent(NULL);
    if (m_pProxyComponent != NULL)
    {
        // If this component is an instance of ptr property, we need to inform ptr to refresh the m_pInstance.
        if (m_pProxyComponent->GetPtrPropertyOwner() != nullptr)
        {
            m_pProxyComponent->GetPtrPropertyOwner()->HACK_InformPtrPropertyToDeleteInstance();
        }
        else
        {
            if (m_pProxyComponent->IsInitialized())
            {
                m_pProxyComponent->Uninitialize();
            }
            BEATS_SAFE_DELETE(m_pProxyComponent);
        }
    }
}

void CComponentInstance::Initialize()
{
#ifdef _DEBUG
    BEATS_ASSERT(!m_bReflectPropertyCritical);
    m_bReflectPropertyCritical = true;
#endif
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        BEATS_ASSERT((*iter) != nullptr && (*iter)->GetId() == 0xFFFFFFFF);
        (*iter)->Initialize();
        BEATS_ASSERT((*iter)->IsInitialized(), "Component %s is not initialized after call Initialize", (*iter)->GetClassStr());
    }
#ifdef _DEBUG
    m_bReflectPropertyCritical = false;
#endif
    // For Editor operation, we always call proxy initialize before instance's initialize.
    // But if we call Initialize manually from code, we need to call proxy initialize.
    if (m_pProxyComponent != NULL && !m_pProxyComponent->IsInitialized())
    {
        m_pProxyComponent->Initialize();
    }
    uint32_t uComponentId = GetId();
    if (uComponentId != 0xFFFFFFFF && CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId, GetGuid()) == nullptr)
    {
        // We must call RegisterInstance before Initialize (so we can resolve dependency).
        // But if we call Uninitialize in code and call Initialize again, we have to Register this component again manually.
        CComponentProxyManager::GetInstance()->GetIdManager()->ReserveId(uComponentId);
        CComponentProxyManager::GetInstance()->RegisterInstance(m_pProxyComponent);
        CComponentInstanceManager::GetInstance()->GetIdManager()->ReserveId(uComponentId);
        CComponentInstanceManager::GetInstance()->RegisterInstance(this);
    }

    super::Initialize();
}

void CComponentInstance::Uninitialize()
{
#ifdef _DEBUG
    BEATS_ASSERT(!m_bReflectPropertyCritical);
    m_bReflectPropertyCritical = true;
#endif
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        BEATS_ASSERT((*iter) != nullptr && (*iter)->GetId() == 0xFFFFFFFF && (*iter)->IsInitialized());
        (*iter)->Uninitialize();
    }
#ifdef _DEBUG
    m_bReflectPropertyCritical = false;
#endif

    if (m_pProxyComponent != NULL)
    {
        BEATS_ASSERT(m_pProxyComponent->GetHostComponent() == this);
        if (m_pProxyComponent->IsInitialized())
        {
            m_pProxyComponent->Uninitialize();
        }
    }
    uint32_t uComponentId = GetId();
    if (uComponentId != 0xFFFFFFFF)
    {
        CComponentInstanceManager::GetInstance()->UnregisterInstance(this);
        CComponentInstanceManager::GetInstance()->GetIdManager()->RecycleId(uComponentId);
    }
    super::Uninitialize();
}

bool CComponentInstance::Load()
{
#ifdef _DEBUG
    BEATS_ASSERT(!m_bReflectPropertyCritical);
    m_bReflectPropertyCritical = true;
#endif
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        BEATS_ASSERT((*iter) != nullptr && (*iter)->GetId() == 0xFFFFFFFF);
        if (!(*iter)->IsLoaded())
        {
            (*iter)->Load();
        }
    }
#ifdef _DEBUG
    m_bReflectPropertyCritical = false;
#endif
    return super::Load();
}

bool CComponentInstance::Unload()
{
#ifdef _DEBUG
    BEATS_ASSERT(!m_bReflectPropertyCritical);
    m_bReflectPropertyCritical = true;
#endif
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        BEATS_ASSERT((*iter) != nullptr && (*iter)->GetId() == 0xFFFFFFFF);
        if ((*iter)->IsLoaded())
        {
            (*iter)->Unload();
        }
    }
#ifdef _DEBUG
    m_bReflectPropertyCritical = false;
#endif
    return super::Unload();
}

void CComponentInstance::SetDataPos(uint32_t uDataPos)
{
    m_uDataPos = uDataPos;
}

uint32_t CComponentInstance::GetDataPos() const
{
    return m_uDataPos;
}

void CComponentInstance::SetDataSize(uint32_t uDataSize)
{
    m_uDataSize = uDataSize;
}

uint32_t CComponentInstance::GetDataSize() const
{
    return m_uDataSize;
}

void CComponentInstance::SetProxyComponent(CComponentProxy* pProxy)
{
    BEATS_ASSERT(m_pProxyComponent == NULL || pProxy == NULL, _T("Proxy component can only set once!"));
    m_pProxyComponent = pProxy;
}

CComponentProxy* CComponentInstance::GetProxyComponent() const
{
    return m_pProxyComponent;
}

void CComponentInstance::SetSyncProxyComponent(CComponentProxy* pProxy)
{
    if (pProxy != m_pSyncProxyComponent)
    {
        if (m_pSyncProxyComponent != NULL)
        {
            m_pSyncProxyComponent->RemoveSyncComponent(this);
        }
        m_pSyncProxyComponent = pProxy;
        if (m_pSyncProxyComponent != NULL)
        {
            m_pSyncProxyComponent->AddSyncComponent(this);
        }
    }
}

CComponentInstance* CComponentInstance::CloneInstance() const
{
    bool bOriginalValue = CComponentInstanceManager::GetInstance()->IsInClonePhase();
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(true);
    CSerializer* pSerializer = nullptr;
#ifdef EDITOR_MODE
    static CSerializer serializer;
    serializer.Reset();
    m_pProxyComponent->ExportDataToHost(serializer, eVT_CurrentValue);
    BEATS_ASSERT(m_uDataSize == 0xFFFFFFFF && m_uDataPos == 0xFFFFFFFF);
    pSerializer = &serializer;
#else
    CSerializer dataProxy;
    dataProxy.SetBuffer(CComponentInstanceManager::GetInstance()->GetFileSerializer()->GetBuffer(), CComponentInstanceManager::GetInstance()->GetFileSerializer()->GetWritePos(), false);
    pSerializer = &dataProxy;
    BEATS_ASSERT(m_uDataPos != 0xFFFFFFFF && m_uDataSize != 0xFFFFFFFF, _T("Can't clone a component which is not created by data"));
    pSerializer->SetReadPos(m_uDataPos);
#endif
#ifdef _DEBUG
    uint32_t uDataSize, uGuid, uId;
    (*pSerializer) >> uDataSize >> uGuid >> uId;
    BEATS_ASSERT(uGuid == GetGuid() && uId == GetId());
#else
    pSerializer->SetReadPos(pSerializer->GetReadPos() + 12);
#endif
    CComponentBase* pNewInstance = Clone(false, pSerializer, 0xFFFFFFFF, false);
#ifndef EDITOR_MODE
    dataProxy.SetBuffer(nullptr, 0, false);
#endif
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(bOriginalValue);
    return down_cast<CComponentInstance*>(pNewInstance);
}

void CComponentInstance::SetReflectOwner(CComponentInstance* pReflectOwner)
{
    m_pReflectComponentOwner = pReflectOwner;
}

CComponentInstance* CComponentInstance::GetReflectOwner() const
{
    return m_pReflectComponentOwner;
}

const std::set<CComponentInstance*>& CComponentInstance::GetReflectComponents() const
{
    return m_reflectComponents;
}

void CComponentInstance::ClearReflectComponents()
{
#ifdef _DEBUG
    BEATS_ASSERT(!m_bReflectPropertyCritical);
    m_bReflectPropertyCritical = true;
#endif
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        (*iter)->SetReflectOwner(nullptr);
    }
#ifdef _DEBUG
    m_bReflectPropertyCritical = false;
#endif
    m_reflectComponents.clear();
}

void CComponentInstance::RegisterReflectComponent(CComponentInstance* pComponent)
{
#ifdef _DEBUG
    BEATS_ASSERT(!m_bReflectPropertyCritical);
#endif
    BEATS_ASSERT(m_reflectComponents.find(pComponent) == m_reflectComponents.end());
    m_reflectComponents.insert(pComponent);
    BEATS_ASSERT(pComponent->GetReflectOwner() == nullptr && pComponent->GetId() == 0xFFFFFFFF);
    pComponent->SetReflectOwner(this);
}

void CComponentInstance::UnregisterReflectComponent(CComponentInstance* pComponent)
{
#ifdef _DEBUG
    BEATS_ASSERT(!m_bReflectPropertyCritical);
#endif
    BEATS_ASSERT(m_reflectComponents.find(pComponent) != m_reflectComponents.end());
    m_reflectComponents.erase(pComponent);
    BEATS_ASSERT(pComponent->GetReflectOwner() == this && pComponent->GetId() == 0xFFFFFFFF);
    pComponent->SetReflectOwner(nullptr);
}

#ifdef EDITOR_MODE
bool CComponentInstance::OnPropertyChange(void* /*pVariableAddr*/, CSerializer* /*pNewValueToBeSet*/)
{
    return false;
}

bool CComponentInstance::OnDependencyChange(void* /*pComponentAddr*/, CComponentBase* /*pComponent*/)
{
    return false;
}

bool CComponentInstance::OnDependencyListChange(void* /*pComponentAddr*/, enum EDependencyChangeAction /*action*/, CComponentBase* /*pComponent*/)
{
    return false;
}

void CComponentInstance::OnSave()
{
    // Do nothing
}

bool CComponentInstance::OnPropertyImport(const TString& /*strPropertyName*/, const TString& /*strFile*/)
{
    // Do nothing
    return false;
}

bool CComponentInstance::OnExport()
{
    // Do nothing
    return false;
}
#endif