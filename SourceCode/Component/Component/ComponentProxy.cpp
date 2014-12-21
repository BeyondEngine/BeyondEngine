#include "stdafx.h"
#include "ComponentProxy.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/IdManager.h"
#include "Component/Property/PropertyDescriptionBase.h"
#include "DependencyDescriptionLine.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentInstanceManager.h"
#include "Component/Component/ComponentProject.h"
#include "DependencyDescription.h"
#include "ComponentGraphic.h"
#include "ComponentInstance.h"

CComponentProxy::CComponentProxy(CComponentGraphic* pGraphics)
: m_bIsTemplate(false)
, m_uGuid(0)
, m_uParentGuid(0)
, m_pGraphics(pGraphics)
, m_pHostComponent(NULL)
{
    BEATS_ASSERT(false, _T("Do never call this!"));
    m_pGraphics->SetOwner(this);
    m_pDependenciesDescription = new std::vector<CDependencyDescription*>;
    m_pBeConnectedDependencyLines = new std::vector<CDependencyDescriptionLine*>;
    m_pSerializeOrder = new std::vector<char>;
    m_pProperties = new std::vector<CPropertyDescriptionBase*>;
}

CComponentProxy::CComponentProxy(CComponentGraphic* pGraphics, uint32_t uGuid, uint32_t uParentGuid, const TCHAR* pszClassName)
: m_bIsTemplate(false)
, m_uGuid(uGuid)
, m_uParentGuid(uParentGuid)
, m_pGraphics(pGraphics)
, m_pHostComponent(NULL)
, m_strClassName(pszClassName)
{
    m_pDependenciesDescription = new std::vector<CDependencyDescription*>;
    m_pBeConnectedDependencyLines = new std::vector<CDependencyDescriptionLine*>;
    m_pSerializeOrder = new std::vector<char>;
    m_pProperties = new std::vector<CPropertyDescriptionBase*>;
    m_pGraphics->SetOwner(this);
}

CComponentProxy::~CComponentProxy()
{
    BEATS_ASSERT(m_pHostComponent == NULL || m_pHostComponent->GetId() == GetId());
    ClearProperty();
    if (m_pDependenciesDescription != NULL)
    {
        for (uint32_t i = 0; i < m_pDependenciesDescription->size(); ++i)
        {
            BEATS_SAFE_DELETE((*m_pDependenciesDescription)[i]);
        }
        BEATS_SAFE_DELETE(m_pDependenciesDescription);
    }

    // In CDependencyDescriptionLine's destructor function, it will reduce the number in m_pBeConnectedDependencyLines.
    while (m_pBeConnectedDependencyLines->size() > 0)
    {
        CDependencyDescriptionLine* pLine = (*m_pBeConnectedDependencyLines)[0];
        BEATS_SAFE_DELETE(pLine);
    }
    BEATS_SAFE_DELETE(m_pBeConnectedDependencyLines);

    BEATS_SAFE_DELETE(m_pProperties);
    BEATS_SAFE_DELETE(m_pSerializeOrder);
    BEATS_SAFE_DELETE(m_pGraphics);
    if (m_pHostComponent != NULL)
    {
        m_pHostComponent->SetProxyComponent(NULL);
    }
}

CComponentBase* CComponentProxy::Clone(bool bCloneValue, CSerializer* /*pSerializer*/, uint32_t id, bool bCallInitFunc /*= true*/) const
{
    bool bOriginClonePhase = CComponentInstanceManager::GetInstance()->IsInClonePhase();
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(true);
    CComponentProxy* pNewProxy = new CComponentProxy(m_pGraphics->Clone(), m_uGuid, m_uParentGuid, m_strClassName.c_str());
    pNewProxy->SetDisplayName(m_strDisplayName.c_str());
    pNewProxy->SetUserDefineDisplayName(m_strUserDefineDisplayName.c_str());
    pNewProxy->SetCatalogName(m_strCatalogName.c_str());
    BEATS_ASSERT(pNewProxy->GetGuid() == GetGuid(), _T("Can't assign between two different type (0x%x and 0x%x) of Reflect Base!"), pNewProxy->GetGuid(), GetGuid());
    pNewProxy->ClearProperty();
    for (uint32_t i = 0; i < (*m_pProperties).size(); ++i)
    {
        CPropertyDescriptionBase* pNewProperty = (*m_pProperties)[i]->Clone(bCloneValue);
        pNewProxy->AddProperty(pNewProperty);
    }
    if (m_pDependenciesDescription != NULL)
    {
        for (uint32_t i = 0; i < m_pDependenciesDescription->size(); ++i)
        {
            CDependencyDescription* pDependency = (*m_pDependenciesDescription)[i];
            CDependencyDescription* pNewDependency = new CDependencyDescription(pDependency->GetType(), pDependency->GetDependencyGuid(), pNewProxy, (uint32_t)pNewProxy->GetDependencies()->size(), pDependency->IsListType());
            pNewDependency->SetDisplayName(pDependency->GetDisplayName());
            pNewDependency->SetVariableName(pDependency->GetVariableName());
        }
    }
    pNewProxy->m_pSerializeOrder->assign(m_pSerializeOrder->begin(), m_pSerializeOrder->end());
    pNewProxy->GetGraphics()->CaculateSize();
    pNewProxy->SetId(id);
    if (m_pHostComponent != NULL)
    {
        if (CComponentProxyManager::GetInstance()->IsEnableCreateInstanceWithProxy())
        {
            // Create an clean host component. sync all value to it from the proxy.
            CComponentInstance* pNewHostComponent = down_cast<CComponentInstance*>(CComponentInstanceManager::GetInstance()->CreateComponent(m_pHostComponent->GetGuid(), false, id == 0xFFFFFFFF, id, true, nullptr, false));
            pNewProxy->SetHostComponent(pNewHostComponent);
        }
    }
    if (bCallInitFunc)
    {
        BEATS_ASSERT(pNewProxy->IsInitialized() == false, _T("Impossible to Initialize compnent twice!"));
        pNewProxy->Initialize();
        if (pNewProxy->GetHostComponent() != NULL)
        {
            BEATS_ASSERT(pNewProxy->GetHostComponent()->IsInitialized() == false, _T("Impossible to Initialize comopnent twice!"));
            pNewProxy->m_pHostComponent->Initialize();
        }
    }
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(bOriginClonePhase);//restore
    return pNewProxy;
}

void CComponentProxy::ExportDataToHost( CSerializer& serializer, EValueType eValueType)
{
    uint32_t startPos = serializer.GetWritePos();
    uint32_t totalSize = 0;
    serializer << totalSize;
    serializer << GetGuid();
    serializer << GetId();
    uint32_t uPropertyCounter = 0;
    uint32_t uDepedencyCoutner = 0;
    for (uint32_t i = 0; i < m_pSerializeOrder->size(); ++i)
    {
        if ((*m_pSerializeOrder)[i] > 0)
        {
            // HACK: to avoid trouble, if we are exporting, we MUST serialize eVT_CurrentValue.
            // We don't care if we have saved, because we must save every property before exporting.
            if (CComponentProxyManager::GetInstance()->IsExporting())
            {
                eValueType = eVT_CurrentValue;
            }
            (*m_pProperties)[uPropertyCounter]->Serialize(serializer, eValueType);
            ++uPropertyCounter;
        }
        else
        {
            CDependencyDescription* pDependencyDesc = (*m_pDependenciesDescription)[uDepedencyCoutner];
            BEATS_ASSERT(pDependencyDesc != NULL);
            pDependencyDesc->Serialize(serializer);
            ++uDepedencyCoutner;
        }
    }
    totalSize = serializer.GetWritePos() - startPos;
    serializer.SetWritePos(startPos);
    serializer << totalSize;
    serializer.SetWritePos(totalSize + startPos);
}

uint32_t CComponentProxy::GetGuid() const
{
    return m_uGuid;
}

uint32_t CComponentProxy::GetParentGuid() const
{
    return m_uParentGuid;
}

const TCHAR* CComponentProxy::GetClassStr() const
{
    return m_strClassName.c_str();
}

const TString& CComponentProxy::GetDisplayName() const
{
    return m_strDisplayName;
}

void CComponentProxy::SetDisplayName(const TCHAR* pDisplayName)
{
    m_strDisplayName.assign(pDisplayName);
}

const TString& CComponentProxy::GetCatalogName() const
{
    return m_strCatalogName;
}

void CComponentProxy::SetCatalogName(const TCHAR* pCatalogName)
{
    if (pCatalogName == NULL)
    {
        pCatalogName = _T("");
    }
    m_strCatalogName.assign(pCatalogName);
}

const TString& CComponentProxy::GetUserDefineDisplayName() const
{
    return m_strUserDefineDisplayName;
}

void CComponentProxy::SetUserDefineDisplayName(const TCHAR* pszUserDefineDisplayName)
{
    m_strUserDefineDisplayName.assign(pszUserDefineDisplayName);
}

CComponentGraphic* CComponentProxy::GetGraphics()
{
    return m_pGraphics;
}

void CComponentProxy::SetGraphics(CComponentGraphic* pGraphics)
{
    m_pGraphics = pGraphics;
    m_pGraphics->SetOwner(this);
}

void CComponentProxy::SetHostComponent(CComponentInstance* pComponent)
{
    BEATS_ASSERT(m_pHostComponent == NULL || pComponent == NULL, _T("SetHostComponent should be called only once!"));
    m_pHostComponent = pComponent;
    if (m_pHostComponent != NULL)
    {
        m_pHostComponent->SetProxyComponent(this);
    }
}

CComponentInstance* CComponentProxy::GetHostComponent() const
{
    return m_pHostComponent;
}

void CComponentProxy::UpdateHostComponent()
{
    bool bIsExporting = CComponentProxyManager::GetInstance()->IsExporting();
    // Don't do any update host stuff when exporting, because it's useless and will cause dependency resolve failed.
    if (m_pHostComponent && !bIsExporting)
    {
        BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetCurrReflectDependency() == nullptr && CComponentProxyManager::GetInstance()->GetCurrReflectProperty() == nullptr,
                    "Reflect check flag MUST be false, or we can not refresh all property!");
        CComponentProxy* pOriginValue = CComponentProxyManager::GetInstance()->GetCurrUpdateProxy();
        CComponentProxyManager::GetInstance()->SetCurrUpdateProxy(this);
        static CSerializer serializer;
        serializer.Reset();
        ExportDataToHost(serializer, eVT_CurrentValue);
        uint32_t uTotalSize = 0;
        uint32_t uGuid = 0;
        uint32_t uId = 0;
        serializer >> uTotalSize;
        serializer >> uGuid;
        serializer >> uId;
        m_pHostComponent->ClearReflectComponents();// Clear all reflected components, since we will rebuild it in ReflectData.
        m_pHostComponent->ReflectData(serializer);
        CComponentInstanceManager::GetInstance()->ResolveDependency();
        CComponentProxyManager::GetInstance()->SetCurrUpdateProxy(pOriginValue);
    }
}

void CComponentProxy::SetTemplateFlag(bool bFlag)
{
    m_bIsTemplate = bFlag;
}

bool CComponentProxy::GetTemplateFlag() const
{
    return m_bIsTemplate;
}

void CComponentProxy::SaveToXML(rapidxml::xml_node<>* pNode, bool bSaveOnlyNoneNativePart/* = false*/)
{
    if (m_pHostComponent != NULL && !GetTemplateFlag())
    {
#ifdef EDITOR_MODE
        m_pHostComponent->OnSave();
#endif
    }
    rapidxml::xml_document<>* pDoc = pNode->document();
    BEATS_ASSERT(pDoc);
    rapidxml::xml_node<>* pInstanceElement = pDoc->allocate_node(rapidxml::node_element, "Instance");
    pNode->append_node(pInstanceElement);
    TCHAR szBuffer[256];
    _stprintf(szBuffer, "%d", GetId());
    pInstanceElement->append_attribute(pDoc->allocate_attribute("Id", pDoc->allocate_string(szBuffer)));
    int posX = 0;
    int posY = 0;
    if (m_pGraphics)
    {
        m_pGraphics->GetPosition(&posX, &posY);
    }
    _stprintf(szBuffer, "%d", posX);
    pInstanceElement->append_attribute(pDoc->allocate_attribute("PosX", pDoc->allocate_string(szBuffer)));
    _stprintf(szBuffer, "%d", posY);
    pInstanceElement->append_attribute(pDoc->allocate_attribute("PosY", pDoc->allocate_string(szBuffer)));
    if (m_strUserDefineDisplayName.length() > 0)
    {
        pInstanceElement->append_attribute(pDoc->allocate_attribute("UserDefineName", pDoc->allocate_string(m_strUserDefineDisplayName.c_str())));
    }
    for (uint32_t i = 0; i < m_pProperties->size(); ++i)
    {
        CPropertyDescriptionBase* pProperty = (*m_pProperties)[i];
        if (!bSaveOnlyNoneNativePart || !pProperty->IsDataSame(true))
        {
            if (!pProperty->IsHide())
            {
                pProperty->SaveToXML(pInstanceElement);
            }
        }
    }

    for (uint32_t i = 0; i < m_pDependenciesDescription->size(); ++i)
    {
        (*m_pDependenciesDescription)[i]->SaveToXML(pInstanceElement);
    }
}

void CComponentProxy::LoadFromXML(rapidxml::xml_node<>* pNode)
{
    if (pNode->first_attribute("PosX") && pNode->first_attribute("PosY"))
    {
        m_pGraphics->SetPosition(atoi(pNode->first_attribute("PosX")->value()), atoi(pNode->first_attribute("PosY")->value()));
    }
    const char* pszUserDefineName = NULL;
    if (pNode->first_attribute("UserDefineName") != nullptr)
    {
         pszUserDefineName = pNode->first_attribute("UserDefineName")->value();
    }
    if (pszUserDefineName != NULL)
    {
        m_strUserDefineDisplayName.assign(pszUserDefineName);
    }
    std::map<TString, CPropertyDescriptionBase*> unInitializedproperties;
    for (uint32_t k = 0; k < m_pProperties->size(); ++k)
    {
        BEATS_ASSERT(unInitializedproperties.find((*m_pProperties)[k]->GetBasicInfo()->m_variableName) == unInitializedproperties.end(),
                    _T("It's impossible to find more than one property which has same name: %s in component %s id %d"),
                    (*m_pProperties)[k]->GetBasicInfo()->m_variableName.c_str(), GetClassStr(), GetId());
        unInitializedproperties[(*m_pProperties)[k]->GetBasicInfo()->m_variableName] = (*m_pProperties)[k];
    }
    rapidxml::xml_node<>* pVarElement = pNode->first_node("VariableNode");
    std::vector<rapidxml::xml_node<>*> unUsedXMLVariableNode;
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    bool bNeedResaveFile = false;
    while (pVarElement != NULL)
    {
        EReflectPropertyType propertyType = (EReflectPropertyType)atoi(pVarElement->first_attribute("Type")->value());
        const char* szVariableName = pVarElement->first_attribute("Variable")->value();
        BEATS_ASSERT(szVariableName != NULL);
        bool bNeedMaintain = true;
        std::map<TString, CPropertyDescriptionBase*>::iterator iter = unInitializedproperties.find(szVariableName);
        // If variable name or type has changed, we will check if we have handled before.
        if (iter == unInitializedproperties.end() || iter->second->GetType() != propertyType)
        {
            TString strReplacePropertyName;
            bNeedMaintain = !pProject->GetReplacePropertyName(GetGuid(), szVariableName, strReplacePropertyName);
            if (!bNeedMaintain && 
                strReplacePropertyName.length() > 0)
            {
                bNeedResaveFile = true;
                if (strReplacePropertyName != "BEATS_PROPERTY_IGNORE")
                {
                    iter = unInitializedproperties.find(strReplacePropertyName);
                    BEATS_ASSERT(iter != unInitializedproperties.end());
                }
            }
        }

        if (iter != unInitializedproperties.end() && iter->second->GetType() == propertyType)
        {
            iter->second->LoadFromXML(pVarElement);
            unInitializedproperties.erase(iter);
        }
        else if(bNeedMaintain)
        {
            bNeedResaveFile = true;
            unUsedXMLVariableNode.push_back(pVarElement);
        }
        pVarElement = pVarElement->next_sibling("VariableNode");
    }

    if (bNeedResaveFile)
    {
        // Save the XML file to overwrite the old property data.
        BEATS_ASSERT(pNode->document()->m_pszFilePath != nullptr);
        uint32_t uFileId = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(pNode->document()->m_pszFilePath);
        BEATS_ASSERT(uFileId != 0xFFFFFFFF);
        CComponentProxyManager::GetInstance()->GetRefreshFileList().insert(uFileId);
    }
    // Run maintain logic.
    for (uint32_t i = 0; i < unUsedXMLVariableNode.size(); ++i)
    {
        EReflectPropertyType propertyType = (EReflectPropertyType)atoi(unUsedXMLVariableNode[i]->first_attribute("Type")->value());
        const char* szVariableName = unUsedXMLVariableNode[i]->first_attribute("Variable")->value();

        std::vector<CPropertyDescriptionBase*> matchTypeProperties;
        for (std::map<TString, CPropertyDescriptionBase*>::iterator iter = unInitializedproperties.begin(); iter != unInitializedproperties.end(); ++iter)
        {
            if (iter->second->GetType() == propertyType)
            {
                matchTypeProperties.push_back(iter->second);
            }
        }
        if (matchTypeProperties.size() > 0)
        {
            BEATS_ASSERT(pNode->document()->m_pszFilePath != nullptr);
            TCHAR szInform[1024];
            _stprintf(szInform, _T("Data:%s (In file\n%s\nComponent %s GUID:0x%x)is no longer valid in this version, contact developer for more information!\nTo Ignore click\"Yes\"\nReallocate click\"No\"\n"),
                szVariableName,
                pNode->document()->m_pszFilePath,
                this->GetClassStr(),
                this->GetGuid());
            int iRet = MessageBox(BEYONDENGINE_HWND, szInform, _T("Maintain data"), MB_YESNO);
            if (iRet == IDYES)
            {
                pProject->RegisterPropertyMaintainInfo(GetGuid(), szVariableName, _T("BEATS_PROPERTY_IGNORE"));
            }
            else
            {
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
                for (uint32_t j = 0; j < matchTypeProperties.size(); )
                {
                    const TString& strVariableName = matchTypeProperties[j]->GetBasicInfo()->m_variableName;
                    _stprintf(szInform, _T("Reallocate %s to %s?"), szVariableName, strVariableName.c_str());
                    TCHAR szTitle[MAX_PATH];
                    _stprintf(szTitle, _T("Reallocate data %d/%d"), j + 1, matchTypeProperties.size());
                    int iRet = MessageBox(BEYONDENGINE_HWND, szInform, szTitle, MB_YESNOCANCEL);
                    if (iRet == IDYES)
                    {
                        matchTypeProperties[j]->LoadFromXML(unUsedXMLVariableNode[i]);
                        unInitializedproperties.erase(strVariableName);
                        pProject->RegisterPropertyMaintainInfo(this->GetGuid(), szVariableName, strVariableName);
                        break;
                    }
                    else if (iRet == IDNO)
                    {
                        if (++j == matchTypeProperties.size())
                        {
                            j = 0;
                        }
                    }
                    else
                    {
                        BEATS_ASSERT(iRet == IDCANCEL);
                        break;
                    }
                }
#endif
            }
        }
    }
    for (uint32_t i = 0; i < m_pDependenciesDescription->size(); ++i)
    {
        (*m_pDependenciesDescription)[i]->LoadFromXML(pNode);
    }
}

CDependencyDescription* CComponentProxy::GetDependency(uint32_t index)
{
    BEATS_ASSERT(index < m_pDependenciesDescription->size());
    BEATS_ASSERT((*m_pDependenciesDescription)[index]->GetIndex() == index);
    return (*m_pDependenciesDescription)[index];
}

CDependencyDescription* CComponentProxy::GetDependency(const TString& strName)
{
    CDependencyDescription* pRet = nullptr;
    for (size_t i = 0; i < m_pDependenciesDescription->size(); ++i)
    {
        if ((*m_pDependenciesDescription)[i]->GetVariableName() == strName)
        {
            pRet = (*m_pDependenciesDescription)[i];
            break;
        }
    }

    return pRet;
}

const std::vector<CDependencyDescription*>* CComponentProxy::GetDependencies()
{
    return m_pDependenciesDescription;
}

void CComponentProxy::AddDependencyDescription(CDependencyDescription* pDependencyDesc )
{
    bool bExisting = false;
    if (pDependencyDesc != NULL)
    {
        for (uint32_t i = 0; i < m_pDependenciesDescription->size(); ++i)
        {
            if ((*m_pDependenciesDescription)[i] == NULL)
            {
                continue;
            }
            bExisting = _tcscmp((*m_pDependenciesDescription)[i]->GetVariableName(), pDependencyDesc->GetVariableName()) == 0;
            if (bExisting)
            {
                BEATS_ASSERT(false, _T("Component %s id %d got a reduplicated dependency declare of variable %s"), GetClassStr(), GetId(), pDependencyDesc->GetVariableName());
                break;
            }
        }
    }

    if (!bExisting)
    {
        if (pDependencyDesc != NULL)
        {
            BEATS_ASSERT(pDependencyDesc->GetOwner() == this);
        }
        m_pDependenciesDescription->push_back(pDependencyDesc);
    }
}

void CComponentProxy::AddBeConnectedDependencyDescriptionLine( CDependencyDescriptionLine* pDependencyDescLine )
{
    BEATS_ASSERT(pDependencyDescLine != NULL);
    m_pBeConnectedDependencyLines->push_back(pDependencyDescLine);
}

void CComponentProxy::RemoveBeConnectedDependencyDescriptionLine( CDependencyDescriptionLine* pDependencyDescLine )
{
    bool bRet = false;
    for (uint32_t i = 0; i < m_pBeConnectedDependencyLines->size(); ++i)
    {
        if (pDependencyDescLine == (*m_pBeConnectedDependencyLines)[i])
        {
            std::vector<CDependencyDescriptionLine*>::iterator iter = m_pBeConnectedDependencyLines->begin();
            advance(iter, i);
            m_pBeConnectedDependencyLines->erase(iter);
            bRet = true;
            break;
        }
    }
    BEATS_ASSERT(bRet);
}

const std::vector<CDependencyDescriptionLine*>* CComponentProxy::GetBeConnectedDependencyLines()
{
    return m_pBeConnectedDependencyLines;
}

std::vector<char>* CComponentProxy::GetSerializerOrderList() const
{
    return m_pSerializeOrder;
}

void CComponentProxy::AddProperty(CPropertyDescriptionBase* pProperty)
{
    if (pProperty != NULL)
    {
        BEATS_ASSERT(pProperty->GetOwner() == NULL);
        pProperty->SetOwner(this);
    }
    m_pProperties->push_back(pProperty);
}

CPropertyDescriptionBase* CComponentProxy::RemoveProperty(const TString& strName)
{
    CPropertyDescriptionBase* pProperty = NULL;
    for (auto iter = m_pProperties->begin(); iter != m_pProperties->end(); ++iter)
    {
        const TString& name = (*iter)->GetBasicInfo()->m_variableName;
        if (name == strName)
        {
            pProperty = *iter;
            m_pProperties->erase(iter);
            break;
        }
    }
    return pProperty;
}

void CComponentProxy::ClearProperty()
{
    if (m_pProperties != NULL)
    {
        for (uint32_t i = 0; i < m_pProperties->size(); ++i)
        {
            BEATS_SAFE_DELETE((*m_pProperties)[i]);
        }
        m_pProperties->clear();
    }
}

const std::vector<CPropertyDescriptionBase*>* CComponentProxy::GetPropertyPool() const
{
    return m_pProperties;
}

CPropertyDescriptionBase* CComponentProxy::GetProperty(const TCHAR* pszVariableName) const
{
    CPropertyDescriptionBase* pProperty = NULL;
    for (uint32_t i = 0; i < m_pProperties->size(); ++i)
    {
        const TString& name = (*m_pProperties)[i]->GetBasicInfo()->m_variableName;
        if(name.compare(pszVariableName) == 0)
        {
            pProperty = (*m_pProperties)[i];
            break;
        }
    }
    return pProperty;
}

void CComponentProxy::Save()
{
    for (uint32_t i = 0; i < (*m_pProperties).size(); ++i)
    {
        (*m_pProperties)[i]->Save();
    }
}

void CComponentProxy::Initialize()
{
    super::Initialize();
    for (uint32_t i = 0; i < (*m_pProperties).size(); ++i)
    {
        (*m_pProperties)[i]->Initialize();
    }
    if (m_pHostComponent != NULL && !GetTemplateFlag())
    {
        // Sync data to host component for the first time!
        // Don't call host component's initialize because we must wait all proxy's host component finish sync.
        BEATS_ASSERT(m_pHostComponent->IsInitialized() == false);
        UpdateHostComponent();
    }
}

void CComponentProxy::Uninitialize()
{
    uint32_t uComponentId = GetId();
    super::Uninitialize();
    if (uComponentId != 0xFFFFFFFF)
    {
        CComponentProxyManager::GetInstance()->UnregisterInstance(this);
        CComponentProxyManager::GetInstance()->GetIdManager()->RecycleId(uComponentId);
    }
    if (m_pProperties != NULL)
    {
        for (uint32_t i = 0; i < (*m_pProperties).size(); ++i)
        {
            (*m_pProperties)[i]->Uninitialize();
        }
    }
    // Make a copy, because the m_syncComponents will be changed in the loop.
    std::vector<CComponentInstance*> syncComponents = m_syncComponents;
    for (uint32_t i = 0; i < syncComponents.size(); ++i)
    {
        syncComponents[i]->SetSyncProxyComponent(NULL);
    }
    BEATS_ASSERT(m_syncComponents.size() == 0, _T("Not all sync components are cleared!"));
}

void CComponentProxy::AddSyncComponent(CComponentInstance* pInstance)
{
#ifdef _DEBUG
    auto iter = std::find(m_syncComponents.begin(), m_syncComponents.end(), pInstance);
    BEATS_ASSERT(iter == m_syncComponents.end(), _T("Can't add an instance twice!"));
#endif
    m_syncComponents.push_back(pInstance);
}

void CComponentProxy::RemoveSyncComponent(CComponentInstance* pInstance)
{
    auto iter = std::find(m_syncComponents.begin(), m_syncComponents.end(), pInstance);
    BEATS_ASSERT(iter != m_syncComponents.end(), _T("Can't find instance to remove!"));
    m_syncComponents.erase(iter);
}

const std::vector<CComponentInstance*>& CComponentProxy::GetSyncComponents() const
{
    return m_syncComponents;
}

void CComponentProxy::SetPtrPropertyOwner(CPropertyDescriptionBase* pProperty)
{
    m_pPtrPropertyOwner = pProperty;
}

CPropertyDescriptionBase* CComponentProxy::GetPtrPropertyOwner() const
{
    return m_pPtrPropertyOwner;
}

