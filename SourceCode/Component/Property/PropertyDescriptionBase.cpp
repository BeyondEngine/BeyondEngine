#include "stdafx.h"
#include "PropertyDescriptionBase.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Component/Component/ComponentProxy.h"
#include "Component/Component/ComponentProxyManager.h"

CPropertyDescriptionBase::CPropertyDescriptionBase(EReflectPropertyType type)
: m_bInitialized(false)
, m_bHide(false)
, m_bNoSyncToHost(false)
, m_type(type)
, m_pOwner(NULL)
, m_pBasicInfo(new SharePtr<SBasicPropertyInfo>(new SBasicPropertyInfo(true, 0xFFFFFFFF)))
, m_pParent(NULL)
, m_pChildren(new std::vector<CPropertyDescriptionBase*>())
{
    for (uint32_t i = 0; i < eVT_Count; ++i)
    {
        m_valueArray[i] = NULL;
    }
}

CPropertyDescriptionBase::CPropertyDescriptionBase(const CPropertyDescriptionBase& rRef)
: m_bInitialized(false)
, m_bHide(rRef.IsHide())
, m_bNoSyncToHost(rRef.m_bNoSyncToHost)
, m_type(rRef.GetType())
, m_pOwner(NULL)
, m_pBasicInfo(new SharePtr<SBasicPropertyInfo>(*rRef.m_pBasicInfo))
, m_pParent(NULL)
, m_pChildren(new std::vector<CPropertyDescriptionBase*>())
{
    for (uint32_t i = 0; i < eVT_Count; ++i)
    {
        m_valueArray[i] = NULL;
    }
}

CPropertyDescriptionBase::~CPropertyDescriptionBase()
{
    BEATS_ASSERT(!IsInitialized(), _T("Call uninitialize before delete a CPropertyDescriptionBase"));
    if (m_pParent != nullptr)
    {
        m_pParent->RemoveChild(this, false);
    }
    std::vector<CPropertyDescriptionBase*> childrenBak = *m_pChildren;
    for (uint32_t i = 0; i < childrenBak.size(); ++i)
    {
        if (childrenBak[i]->IsInitialized())
        {
            childrenBak[i]->Uninitialize();
        }
        BEATS_SAFE_DELETE(childrenBak[i]);
    }
    BEATS_ASSERT(m_pChildren->size() == 0);
    BEATS_SAFE_DELETE(m_pChildren);
    BEATS_SAFE_DELETE(m_pBasicInfo);

}

EReflectPropertyType CPropertyDescriptionBase::GetType() const
{
    return m_type;
}

void CPropertyDescriptionBase::SetType(EReflectPropertyType type)
{
    m_type = type;
}

CComponentProxy* CPropertyDescriptionBase::GetOwner() const
{
    return m_pOwner;
}

void CPropertyDescriptionBase::SetOwner(CComponentProxy* pOwner)
{
    m_pOwner = pOwner;
}

CComponentProxy* CPropertyDescriptionBase::GetRootOwner() const
{
    const CPropertyDescriptionBase* pParentProp = this;
    if (pParentProp->GetParent() != nullptr)
    {
        pParentProp = pParentProp->GetParent();
    }
    return pParentProp->GetOwner();
}

CPropertyDescriptionBase* CPropertyDescriptionBase::GetParent() const
{
    return m_pParent;
}

void CPropertyDescriptionBase::SetParent(CPropertyDescriptionBase* pParent)
{
    BEATS_ASSERT(m_pParent == NULL);
    m_pParent = pParent;
}

void CPropertyDescriptionBase::SetHide(bool bHide)
{
    m_bHide = bHide;
}

bool CPropertyDescriptionBase::IsHide() const
{
    return m_bHide;
}

void CPropertyDescriptionBase::ResetBasicInfo(const SharePtr<SBasicPropertyInfo>& pInfo)
{
    *m_pBasicInfo = pInfo;
}

void CPropertyDescriptionBase::SetBasicInfo(const SBasicPropertyInfo& info)
{
    *m_pBasicInfo->Get() = info;
}

const SharePtr<SBasicPropertyInfo>& CPropertyDescriptionBase::GetBasicInfo() const
{
    return *m_pBasicInfo;
}

bool CPropertyDescriptionBase::DeserializeBasicInfo(CSerializer& serializer)
{
    TCHAR* buff = NULL;
    TCHAR** pbuff = &buff;
    serializer >> (*m_pBasicInfo)->m_bEditable;
    serializer >> (*m_pBasicInfo)->m_color;
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_displayName.assign(*pbuff);
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_catalog.assign(*pbuff);
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_tip.assign(*pbuff);
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_variableName.assign(*pbuff);

    return true;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::InsertChild(CPropertyDescriptionBase* pProperty, uint32_t uPreIndex)
{
#ifdef _DEBUG
    for (uint32_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_ASSERT((*m_pChildren)[i] != pProperty);
    }
#endif
    BEATS_ASSERT(pProperty != NULL);
    pProperty->m_pParent = this;
    if (uPreIndex == 0xFFFFFFFF)
    {
        m_pChildren->push_back(pProperty);
    }
    else
    {
        BEATS_ASSERT(uPreIndex < m_pChildren->size());
        std::vector<CPropertyDescriptionBase*>::iterator preIter = m_pChildren->begin();
        std::advance(preIter, uPreIndex);
        m_pChildren->insert(preIter, pProperty);
    }
    if (IsContainerProperty())
    {
        pProperty->SetOwner(m_pOwner);
    }
    return pProperty;
}

bool CPropertyDescriptionBase::RemoveChild(CPropertyDescriptionBase* pProperty, bool bDelete)
{
    if (bDelete)
    {
        pProperty->Uninitialize();
        BEATS_SAFE_DELETE(pProperty);
    }
    else
    {
        auto iter = std::find(m_pChildren->begin(), m_pChildren->end(), pProperty);
        BEATS_ASSERT(iter != m_pChildren->end());
        m_pChildren->erase(iter);
    }
    return true;
}

void CPropertyDescriptionBase::RemoveAllChild(bool bDelete)
{
    if (bDelete)
    {
        std::vector<CPropertyDescriptionBase*> childrenBak = *m_pChildren;
        for (uint32_t i = 0; i < childrenBak.size(); ++i)
        {
            if (childrenBak[i]->IsInitialized())
            {
                childrenBak[i]->Uninitialize();
            }
            BEATS_SAFE_DELETE(childrenBak[i]);
        }
    }
    else
    {
        m_pChildren->clear();
    }
    BEATS_ASSERT(m_pChildren->size() == 0);
}

const std::vector<CPropertyDescriptionBase*>& CPropertyDescriptionBase::GetChildren() const
{
    return *m_pChildren;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::Clone(bool bCloneValue)
{
    CPropertyDescriptionBase* pNewProperty = CreateNewInstance();
    pNewProperty->SetValueWithType(m_valueArray[eVT_DefaultValue], eVT_DefaultValue);
    pNewProperty->SetValueWithType(bCloneValue ? m_valueArray[eVT_SavedValue] : m_valueArray[eVT_DefaultValue], eVT_SavedValue);
    pNewProperty->SetValueWithType(bCloneValue ? m_valueArray[eVT_CurrentValue] : m_valueArray[eVT_DefaultValue], eVT_CurrentValue);
    return pNewProperty;
}

void* CPropertyDescriptionBase::GetValue(EValueType type) const
{
    return m_valueArray[type];
}

bool CPropertyDescriptionBase::CanSyncToHost() const
{
    bool bRet = !m_bNoSyncToHost && !CComponentProxyManager::GetInstance()->IsExporting();
    if (bRet && GetParent() != NULL)
    {
        bRet = GetParent()->CanSyncToHost();
    }
    return bRet;
}

void CPropertyDescriptionBase::Save()
{
    SetValueWithType(m_valueArray[eVT_CurrentValue], eVT_SavedValue);
}

void CPropertyDescriptionBase::Initialize()
{
    BEATS_ASSERT(!m_bInitialized, _T("Can't initialize a property twice!"));
    m_bInitialized = true;
}

void CPropertyDescriptionBase::Uninitialize()
{
    BEATS_ASSERT(m_bInitialized, _T("Can't Uninitialize a property twice!"));
    m_bInitialized = false;
}

bool CPropertyDescriptionBase::IsInitialized() const
{
    return m_bInitialized;
}

bool CPropertyDescriptionBase::IsContainerProperty()
{
    return false;
}

void CPropertyDescriptionBase::SerializeContainerElementLocation(CSerializer& serializer, CPropertyDescriptionBase* pChildProperty)
{
    BEATS_ASSERT(IsContainerProperty(), "Can't call this function for a non-container property!");
    uint32_t uChildIndex = GetChildIndex(pChildProperty);
    BEATS_ASSERT(uChildIndex != 0xFFFFFFFF);
    serializer << uChildIndex;
}

bool CPropertyDescriptionBase::OnChildChanged(uint32_t /*uChildIndex*/)
{
    return false;
}

void CPropertyDescriptionBase::SetValueList(const std::vector<TString>& /*valueList*/)
{
    // Do nothing.
}

CComponentProxy* CPropertyDescriptionBase::GetInstanceComponent() const
{
    return NULL;
}

uint32_t CPropertyDescriptionBase::HACK_GetPtrReflectGuid() const
{
    return 0;
}

void CPropertyDescriptionBase::HACK_InformPtrPropertyToDeleteInstance()
{

}

void CPropertyDescriptionBase::SetValueWithType(void* pValue, EValueType type, bool bForceUpdateHostComponent/* = false*/)
{
    if (CopyValue(pValue, m_valueArray[type]) || bForceUpdateHostComponent)
    {
        // We don't need to sync the property in loading phase, since after all components are loaded from XML
        // we will call CComponentProxy::UpdateHostComponent for total.
        bool bLoadingPhase = CComponentInstanceManager::GetInstance()->IsInLoadingPhase();
        if (!bLoadingPhase && CanSyncToHost() && type == eVT_CurrentValue && GetOwner())
        {
            CPropertyDescriptionBase* pDataProperty = this; // the property which can be deserialized as the minimal unit, for example: CVec3 is ok, but CVec3::x is not ok.
            EReflectOperationType reflectOperateType = EReflectOperationType::ChangeValue;
            CPropertyDescriptionBase* pCurrReflectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty(&reflectOperateType);
            if (pCurrReflectProperty != nullptr)
            {
                BEATS_ASSERT(reflectOperateType != EReflectOperationType::ChangeValue);
                BEATS_ASSERT(this->IsContainerProperty());
                if (reflectOperateType == EReflectOperationType::AddChild || reflectOperateType == EReflectOperationType::RemoveChild)
                {
                    pDataProperty = pCurrReflectProperty; // in Add/RemoveChild mode, current reflect property is the child to be added.
                }
            }

            // Some property is not the real property, such as container property.
            CPropertyDescriptionBase* pReflectProperty = this; // the property we inform user in OnPropertyChanged.
            std::vector<CPropertyDescriptionBase*> m_containerElementProperties;// contains the location of pDataProperty in pReflectProperty, can be nested.
            // Example: std::vector<std::vector<CVec3>> m_testData.
            // If I change the x value of m_testData:
            // this will be the float property description of CVec3::x.
            // CVec3 will be the pDataProperty.
            // m_testData will be the pReflectProperty.
            // if m_containerElementIndexInfo == {10, 15}, it means I changed m_testDat[10][15].x
            CPropertyDescriptionBase* pParentProperty = this->GetParent();
            std::vector<CPropertyDescriptionBase*> refreshPropertyList;
            static CSerializer serializer;
            serializer.Reset();
            serializer.SetUserData(0);
            while (pParentProperty != NULL && pReflectProperty->GetOwner() == pParentProperty->GetOwner())
            {
                if (pParentProperty->IsContainerProperty())
                {
                    m_containerElementProperties.push_back(pReflectProperty);
                }
                else
                {
                    if (pDataProperty == this && reflectOperateType != EReflectOperationType::RemoveChild) // the data property must be the current reflect property in remove child mode.
                    {
                        pDataProperty = pParentProperty;
                    }
                }
                refreshPropertyList.push_back(pParentProperty);
                pReflectProperty = pParentProperty;
                pParentProperty = pParentProperty->GetParent();
            }

            CComponentBase* pHostComponent = pReflectProperty->GetOwner()->GetHostComponent();
            CPropertyDescriptionBase* pRootProperty = this;
            while (pRootProperty->GetParent() != NULL)
            {
                pRootProperty = pRootProperty->GetParent();
            }
            // Don't reflect for template component because we only want to change the value of it.
            // Template component will never take effect on logic.
            bool bIsTemplateProperty = pRootProperty->GetOwner()->GetTemplateFlag();
            if (pHostComponent && !bIsTemplateProperty)
            {
                // Record the original value to avoid wrong set in recursive call.
                CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pReflectProperty, reflectOperateType);
                for (auto rIter = m_containerElementProperties.rbegin(); rIter != m_containerElementProperties.rend(); ++rIter)
                {
                    CPropertyDescriptionBase* pParent = (*rIter)->GetParent();
                    BEATS_ASSERT(pParent != nullptr && pParent->IsContainerProperty());
                    pParent->SerializeContainerElementLocation(serializer, *rIter);
                }
                serializer.SetUserData((void*)serializer.GetWritePos());
                if (reflectOperateType == EReflectOperationType::AddChild)
                {
                    BEATS_ASSERT(pDataProperty->GetParent() != nullptr&& pDataProperty->GetParent()->IsContainerProperty());
                    pDataProperty->GetParent()->SerializeContainerElementLocation(serializer, pDataProperty);
                }
                else if (reflectOperateType == EReflectOperationType::RemoveChild || reflectOperateType == EReflectOperationType::ChangeListOrder)
                {
                    CSerializer& removeChildInfo = CComponentProxyManager::GetInstance()->GetRemoveChildInfo();
                    if (removeChildInfo.GetWritePos() != removeChildInfo.GetReadPos())
                    {
                        serializer.Serialize(removeChildInfo);
                        removeChildInfo.Reset();
                    }
                }
                pDataProperty->Serialize(serializer, eVT_CurrentValue);
                pHostComponent->ReflectData(serializer);
                const std::vector<CComponentInstance*>& syncComponents = pReflectProperty->GetOwner()->GetSyncComponents();
                for (uint32_t i = 0; i < syncComponents.size(); ++i)
                {
                    serializer.SetReadPos(0);
                    syncComponents[i]->ReflectData(serializer);
                }
                CComponentProxyManager::GetInstance()->SetCurrReflectProperty(nullptr, EReflectOperationType::ChangeValue);//Reset to default operation type
                for (size_t i = 0; i < refreshPropertyList.size(); ++i)
                {
                    uint32_t uIndex = 0;
                    CPropertyDescriptionBase* pChildProperty = i == 0 ? pDataProperty : refreshPropertyList[i - 1];
                    uIndex = refreshPropertyList[i]->GetChildIndex(pChildProperty);
                    if (refreshPropertyList[i]->OnChildChanged(uIndex))
                    {
                        break;
                    }
                }
            }
        }
    }
}

void CPropertyDescriptionBase::SetValueByString(const TCHAR* pszStr, EValueType type, bool bUpdateHost)
{
    GetValueByTChar(pszStr, GetValue(type));
    if (bUpdateHost)
    {
        SetValueWithType(GetValue(type), type, true);
    }
}

void CPropertyDescriptionBase::SetNoSyncHost(bool bValue)
{
    m_bNoSyncToHost = bValue;
}

uint32_t CPropertyDescriptionBase::GetChildIndex(const CPropertyDescriptionBase* pChildProperty) const
{
    uint32_t uRet = 0xFFFFFFFF;
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        if (pChildProperty == m_pChildren->at(i))
        {
            uRet = i;
            break;
        }
    }
    return uRet;
}
