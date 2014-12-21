#include "stdafx.h"
#include "ComponentProxyManager.h"
#include "ComponentInstanceManager.h"
#include "ComponentProject.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Utility/BeatsUtility/IdManager.h"
#include "ComponentGraphic.h"
#include "DependencyDescriptionLine.h"
#include "DependencyDescription.h"
#include "ComponentProxy.h"
#include "Component/ComponentPublic.h"
#include "ComponentInstance.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "ComponentProjectDirectory.h"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"

CComponentProxyManager* CComponentProxyManager::m_pInstance = NULL;

CComponentProxyManager::CComponentProxyManager()
    : m_bCreateInstanceWithProxy(true)
    , m_bExportingPhase(false)
    , m_uOperateProgress(0)
    , m_uCurrViewFileId(0xFFFFFFFF)
    , m_pCurrReflectProperty(NULL)
    , m_pCurrReflectDependency(NULL)
    , m_pCurrUpdateProxy(NULL)
    , m_pRemoveChildInfo(nullptr)
    , m_reflectOperateType(EReflectOperationType::ChangeValue)
{
    m_pPropertyCreatorMap = new std::map<uint32_t, TCreatePropertyFunc>();
    m_pComponentInheritMap = new std::map<uint32_t, std::set<uint32_t> >();
    m_pComponentBaseClassMap = new std::map<uint32_t, uint32_t>();
    m_pRemoveChildInfo = new CSerializer;
}

CComponentProxyManager::~CComponentProxyManager()
{
    BEATS_SAFE_DELETE(m_pIdManager);
    BEATS_ASSERT(m_pComponentInstanceMap->size() == 0, _T("All proxy should be deleted by instance"));
    BEATS_SAFE_DELETE(m_pComponentInstanceMap);
    BEATS_SAFE_DELETE(m_pComponentTemplateMap);
    BEATS_SAFE_DELETE(m_pDependencyResolver);
    BEATS_SAFE_DELETE(m_pPropertyCreatorMap);
    BEATS_SAFE_DELETE(m_pComponentInheritMap);
    BEATS_SAFE_DELETE(m_pComponentBaseClassMap);
    BEATS_SAFE_DELETE(m_pRemoveChildInfo);
}

void CComponentProxyManager::InitializeAllTemplate()
{
    std::map<uint32_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->begin();
    for (; iter != m_pComponentTemplateMap->end(); ++iter)
    {
        BEATS_ASSERT(iter->second != NULL);
        BEATS_ASSERT(dynamic_cast<CComponentProxy*>(iter->second) != NULL, _T("Only proxy template can be initialized!"));
        BEATS_ASSERT(iter->second->IsInitialized() == false, _T("Can't initialize component twice!"));
        iter->second->Initialize();
        BEATS_ASSERT(iter->second->IsInitialized(),
            _T("The initialize flag of component %s is not set after initialize func!"),
            iter->second->GetClassStr());
    }
}

void CComponentProxyManager::UninitializeAllTemplate()
{
    for (std::map<uint32_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->begin(); iter != m_pComponentTemplateMap->end(); ++iter)
    {
        BEATS_ASSERT(iter->second != NULL);
        BEATS_ASSERT(iter->second->IsInitialized() || iter->second->GetGuid() != 1);
        iter->second->Uninitialize();
    }
}

void CComponentProxyManager::OpenFile(const TCHAR* pFilePath, bool bCloseLoadedFile/*= false*/)
{
    m_uOperateProgress = 0;
    uint32_t uFileId = m_pProject->GetComponentFileId(pFilePath);
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    if (uFileId != 0xFFFFFFFF)
    {
        // Save cur file.
        if (m_uCurrViewFileId != uFileId)
        {
            SaveCurFile();
        }
        bool bNewAddThisFile = false;
        std::vector<uint32_t> loadFiles;
        std::vector<uint32_t> unloadFiles;
        CalcSwitchFile(uFileId, loadFiles, unloadFiles, bNewAddThisFile);
        if (bCloseLoadedFile)
        {
            // use 10% for close file.
            for (uint32_t i = 0; i < unloadFiles.size(); ++i)
            {
                m_strCurrOperateFile = m_pProject->GetComponentFileName(unloadFiles[i]);
                CloseFile(unloadFiles[i]);
                m_uOperateProgress += (uint32_t)(10.f / unloadFiles.size());
            }
        }
        m_uOperateProgress = 10;
        // use 80% for load file.
        std::vector<CComponentBase*> loadedComponents;
        for (uint32_t i = 0; i < loadFiles.size(); ++i)
        {
            m_strCurrOperateFile = m_pProject->GetComponentFileName(loadFiles[i]);
            LoadFile(loadFiles[i], &loadedComponents);
            m_uOperateProgress += (uint32_t)(80.f / loadFiles.size());
        }
        m_uOperateProgress = 90;
        bool bLoadThisFile = loadFiles.size() > 0 && loadFiles.back() == uFileId;
        if (bLoadThisFile)
        {
            if (!bNewAddThisFile)
            {
                m_uCurrLoadFileId = uFileId;
            }
        }
        if (bCloseLoadedFile && loadFiles.size() == 0) // It means we have load uFileId before, and we return to it with closing other loaded file.
        {
            m_uCurrLoadFileId = uFileId;
        }
        SetCurrentViewFileId(uFileId);
        // use 10% for initialize all loaded components.
        for (uint32_t i = 0; i < loadedComponents.size(); ++i)
        {
            static_cast<CComponentProxy*>(loadedComponents[i])->GetHostComponent()->Initialize();
            m_uOperateProgress += (uint32_t)(10.f / loadedComponents.size());
        }
        m_uOperateProgress = 100;
        m_strCurrOperateFile.clear();
        ReSaveFreshFile();
    }
}

void CComponentProxyManager::LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pComponentContainer)
{
    const TString& strFilePath = m_pProject->GetComponentFileName(uFileId);
    BEATS_ASSERT(strFilePath.length() > 0);
    if (CFilePathTool::GetInstance()->Exists(strFilePath.c_str()))
    {
        rapidxml::file<> fdoc(strFilePath.c_str());
        rapidxml::xml_document<> doc;
        try
        {
            doc.parse<rapidxml::parse_default>(fdoc.data());
            doc.m_pszFilePath = strFilePath.c_str();
        }
        catch (rapidxml::parse_error &e)
        {
            TCHAR info[MAX_PATH];
            _stprintf(info, _T("Load file :%s Failed! error :%s"), strFilePath.c_str(), e.what());
            MessageBox(BEYONDENGINE_HWND, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
        BEATS_ASSERT(std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId) == m_loadedFiles.end());
        m_loadedFiles.push_back(uFileId);

        rapidxml::xml_node<>* pRootElement = doc.first_node("Root");
        rapidxml::xml_node<>* pComponentListNode = pRootElement->first_node("Components");
        if (pComponentListNode != NULL)
        {
            bool bRestoreLoadingPhase = CComponentInstanceManager::GetInstance()->IsInLoadingPhase();
            CComponentInstanceManager::GetInstance()->SetLoadPhaseFlag(true);
            std::vector<CComponentProxy*> loadedProxyList;
            rapidxml::xml_node<>* pComponentElement = pComponentListNode->first_node("Component");
            while (pComponentElement != NULL)
            {
                const char* pGuidStr = pComponentElement->first_attribute("GUID")->value();
                char* pStopPos = NULL;
                int guid = strtoul(pGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == 0, _T("Guid value %s is not a 0x value at file %s."), pGuidStr, strFilePath.c_str());
                if (GetComponentTemplate(guid) == NULL)
                {
                    CComponentProxyManager::GetInstance()->GetRefreshFileList().insert(uFileId);
                    BEATS_ASSERT(false, _T("Can't create component with \nGUID 0x%x\nName %s\nFile id:%d Name:%s\nHave you removed this component class?"), guid, pComponentElement->first_attribute("Name")->value(), uFileId, strFilePath.c_str());
                }
                else
                {
                    rapidxml::xml_node<>* pInstanceElement = pComponentElement->first_node();
                    while (pInstanceElement != NULL)
                    {
                        int id = atoi(pInstanceElement->first_attribute("Id")->value());
                        BEATS_ASSERT(id != -1);
                        CComponentProxy* pComponentProxy = NULL;
                        if (strcmp(pInstanceElement->name(), "Instance") == 0)
                        {
                            pComponentProxy = down_cast<CComponentProxy*>(CreateComponent(guid, false, false, id, false, NULL, false));
                        }
                        pComponentProxy->LoadFromXML(pInstanceElement);
                        loadedProxyList.push_back(pComponentProxy);
                        if (pComponentContainer != nullptr)
                        {
                            pComponentContainer->push_back(pComponentProxy);
                        }
                        pInstanceElement = pInstanceElement->next_sibling();
                    }
                }
                pComponentElement = pComponentElement->next_sibling("Component");
            }
            ResolveDependency();
            CComponentInstanceManager::GetInstance()->SetLoadPhaseFlag(bRestoreLoadingPhase);
            if (!CComponentProxyManager::GetInstance()->IsExporting())
            {
                // Call component proxy's initialize means we have sync all value to host component, so we call host component's load function.
                for (size_t i = 0; i < loadedProxyList.size(); ++i)
                {
                    loadedProxyList[i]->Initialize();
                }
                for (size_t i = 0; i < loadedProxyList.size(); ++i)
                {
                    CComponentInstance* pHostComponent = static_cast<CComponentProxy*>(loadedProxyList[i])->GetHostComponent();
                    if (pHostComponent != nullptr)
                    {
                        pHostComponent->Load();
                    }
                    else
                    {
                        BEATS_ASSERT(m_bCreateInstanceWithProxy == false, "Only when m_bCreateInstanceWithProxy is set to false, we can't get the host component");
                    }
                }
            }
        }
    }
}

void CComponentProxyManager::UnloadFile(uint32_t uFileId, std::vector<CComponentBase*>* pUnloadComponents)
{
    BEATS_ASSERT(pUnloadComponents != nullptr);
    std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId);
    BEATS_WARNING(iterFile != m_loadedFiles.end(), "Close an unopened file %d, this may be right if we are exiting the program.", uFileId);
    if (iterFile != m_loadedFiles.end())
    {
        // query id from the static data: m_pProject.
        const std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponentMap->find(uFileId);
        if (iter != pFileToComponentMap->end())
        {
            for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
            {
                for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
                {
                    uint32_t uComponentId = *idIter;
                    CComponentProxy* pComponentProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                    BEATS_ASSERT(pComponentProxy != NULL && pComponentProxy->IsLoaded());
                    pComponentProxy->Unload();
                    pComponentProxy->GetHostComponent()->Unload();
                    pUnloadComponents->push_back(pComponentProxy);
                }
            }
        }
        m_loadedFiles.erase(iterFile);
    }
}

void CComponentProxyManager::LoadFileFromDirectory(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>* pComponentContainer)
{
    const std::vector<uint32_t>& fileList = pDirectory->GetFileList();
    for (uint32_t i = 0; i < fileList.size(); ++i)
    {
        LoadFile(fileList[i], pComponentContainer);
    }
}

void CComponentProxyManager::CloseFile(uint32_t uFileId)
{
    ReSaveFreshFile(); // the file we are about to close may be in the fresh file list, so we always try to save it before it is closed.
    // TODO: Lock id manager, since we will recycle id in component's un-initialize function.
    // However, we don't want that, so we lock the id manager for now.
    m_pIdManager->Lock();
    std::vector<CComponentBase*> unloadProxyList;
    UnloadFile(uFileId, &unloadProxyList);
    std::vector<CComponentBase*> componentToDelete;
    for (size_t i = 0; i < unloadProxyList.size(); ++i)
    {
        CComponentProxy* pProxy = static_cast<CComponentProxy*>(unloadProxyList[i]);
        componentToDelete.push_back(pProxy->GetHostComponent());
    }
    for (uint32_t i = 0; i < componentToDelete.size(); ++i)
    {
        CComponentBase* pComponentBase = componentToDelete[i];
        BEATS_ASSERT(!pComponentBase->IsLoaded());
        if (pComponentBase->IsInitialized())
        {
            pComponentBase->Uninitialize();
        }
    }
    for (uint32_t i = 0; i < componentToDelete.size(); ++i)
    {
        CComponentBase* pComponentBase = componentToDelete[i];
        BEATS_ASSERT(!pComponentBase->IsInitialized());
        BEATS_SAFE_DELETE(pComponentBase);
    }
    if (GetCurrentViewFileId() == uFileId)
    {
        SetCurrentViewFileId(0xFFFFFFFF);
    }
    if (GetCurrLoadFileId() == uFileId)
    {
        SetCurrLoadFileId(0xFFFFFFFF);
    }
    m_pIdManager->UnLock();
}

const uint32_t CComponentProxyManager::GetCurrentViewFileId() const
{
    return m_uCurrViewFileId;
}

void CComponentProxyManager::SetCurrentViewFileId(uint32_t uCurViewFileId)
{
    // Rebuild the m_proxyInCurScene
    m_proxyInCurScene.clear();
    const std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
    auto fileToComponentIter = pFileToComponentMap->find(uCurViewFileId);
    if (fileToComponentIter != pFileToComponentMap->end())
    {
        for (auto subIter = fileToComponentIter->second.begin(); subIter != fileToComponentIter->second.end(); ++subIter)
        {
            for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
            {
                uint32_t uComponentId = *idIter;
                BEATS_ASSERT(m_proxyInCurScene.find(uComponentId) == m_proxyInCurScene.end());
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                // If the component is deleted in code, it is possible that proxy is null.
                BEATS_ASSERT(pProxy != NULL || GetComponentTemplate(m_pProject->QueryComponentGuid(uComponentId)) == NULL);
                if (pProxy != NULL)
                {
                    m_proxyInCurScene[uComponentId] = pProxy;
                }
            }
        }
    }
    m_uCurrViewFileId = uCurViewFileId;
}

void CComponentProxyManager::Export(const TCHAR* pSavePath, std::function<bool(uint32_t, CComponentProxy*)> exportCallback)
{
    m_bExportingPhase = true;
    BEATS_ASSERT(pSavePath != NULL);
    static CSerializer serializer;
    serializer.Reset();
    serializer << COMPONENT_SYSTEM_VERSION;
    serializer << GetComponentTemplateMap()->size();
    CComponentProjectDirectory* pRootProject = m_pProject->GetRootDirectory();
    pRootProject->Serialize(serializer);
    serializer << m_pProject->GetStartFile();

    m_uOperateProgress = 0;
    uint32_t uFileCount = (uint32_t)(m_pProject->GetFileList()->size());
    serializer << uFileCount;

    for (uint32_t i = 0; i < uFileCount; ++i)
    {
        const TString strFileName = CFilePathTool::GetInstance()->FileName(m_pProject->GetComponentFileName(i).c_str());
        m_strCurrOperateFile = strFileName;
        serializer << strFileName;
        uint32_t uComponentCount = 0;
        uint32_t uWritePos = serializer.GetWritePos();
        serializer << uWritePos;// File Start pos.
        serializer << 12;// File size placeholder.
        serializer << uComponentCount; // component count place holder
        const std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponent = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponent->find(i);
        BEATS_ASSERT(iter != pFileToComponent->end(), _T("File: %s\ndoes not have a component!"), strFileName.c_str());
        if (iter != pFileToComponent->end())
        {
            std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), i);
            if (iterFile != m_loadedFiles.end())
            {
                for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
                {
                    for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
                    {
                        uint32_t uComponentId = *idIter;
                        CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                        if (pProxy)
                        {
                            if (ExportComponentProxy(i, pProxy, serializer, exportCallback))
                            {
                                m_refreshFileList.insert(*iterFile);
                            }
                            ++uComponentCount;
                        }
                        else
                        {
                            BEATS_ASSERT(false, _T("Can't find proxy with GUID 0x%x id %d, have you removed that class in code?"), m_pProject->QueryComponentGuid(uComponentId), uComponentId);
                        }
                    }
                }
            }
            else
            {
                std::vector<CComponentBase*> vecComponents;
                // Don't create instance in LoadFile when exporting.
                BEATS_ASSERT(m_bCreateInstanceWithProxy);
                m_bCreateInstanceWithProxy = false;
                m_pIdManager->Lock();
                LoadFile(i, &vecComponents);
                iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), i);
                BEATS_ASSERT(iterFile != m_loadedFiles.end(), _T("Load file index %d failed!"), i);
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    CComponentProxy* pProxy = static_cast<CComponentProxy*>(vecComponents[j]);
                    if (ExportComponentProxy(i, pProxy, serializer, exportCallback))
                    {
                        m_refreshFileList.insert(*iterFile);
                    }
                    ++uComponentCount;
                }
                ReSaveFreshFile();
                // Don't call CloseFile, because we have nothing to do with proxy's host component.
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    if (vecComponents[j]->GetId() != 0xFFFFFFFF)
                    {
                        CComponentProxyManager::GetInstance()->UnregisterInstance(vecComponents[j]);
                        CComponentProxyManager::GetInstance()->GetIdManager()->RecycleId(vecComponents[j]->GetId());
                    }
                    BEATS_SAFE_DELETE(vecComponents[j]);
                }
                m_loadedFiles.erase(iterFile);
                m_pIdManager->UnLock();
                m_bCreateInstanceWithProxy = true;// Restore.
            }
            uint32_t uCurWritePos = serializer.GetWritePos();
            uint32_t uFileDataSize = uCurWritePos - uWritePos;
            serializer.SetWritePos(uWritePos + sizeof(uint32_t));// Skip file start pos.
            serializer << uFileDataSize;
            serializer << uComponentCount;
            serializer.SetWritePos(uCurWritePos);
        }
        m_uOperateProgress = uint32_t((i + 1) * 100.f / uFileCount);
    }
    BEATS_ASSERT(m_uOperateProgress == 100);
    m_strCurrOperateFile.clear();
    serializer.Deserialize(pSavePath);
    m_bExportingPhase = false;
}

void CComponentProxyManager::QueryDerivedClass(uint32_t uBaseClassGuid, std::set<uint32_t>& result, bool bRecurse ) const
{
    result.clear();
    std::map<uint32_t, std::set<uint32_t> >::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
    if (iter != m_pComponentInheritMap->end())
    {
        result = iter->second;
        if (bRecurse)
        {
            for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
            {
                uint32_t uCurrGuid = *subIter;
                std::set<uint32_t> subResult;
                QueryDerivedClass(uCurrGuid, subResult, true);
                for (auto retIter = subResult.begin(); retIter != subResult.end(); ++retIter)
                {
                    uint32_t uRetGuid = *retIter;
                    BEATS_ASSERT(result.find(uRetGuid) == result.end());
                    result.insert(uRetGuid);
                }
            }
        }
    }
}

uint32_t CComponentProxyManager::QueryBaseClass(uint32_t uGuid) const
{
    uint32_t uRet = 0xFFFFFFFF;
    auto iter = m_pComponentBaseClassMap->find(uGuid);
    if (iter != m_pComponentBaseClassMap->end())
    {
        uRet = iter->second;
    }
    return uRet;
}

void CComponentProxyManager::RegisterClassInheritInfo( uint32_t uDerivedClassGuid, uint32_t uBaseClassGuid )
{
    std::map<uint32_t, std::set<uint32_t> >::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
    if (iter == m_pComponentInheritMap->end())
    {
        (*m_pComponentInheritMap)[uBaseClassGuid] = std::set<uint32_t>();
        iter = m_pComponentInheritMap->find(uBaseClassGuid);
    }
    BEATS_ASSERT(iter != m_pComponentInheritMap->end());
    BEATS_ASSERT(iter->second.find(uDerivedClassGuid) == iter->second.end());
    iter->second.insert(uDerivedClassGuid);
    BEATS_ASSERT(m_pComponentBaseClassMap->find(uDerivedClassGuid) == m_pComponentBaseClassMap->end());
    (*m_pComponentBaseClassMap)[uDerivedClassGuid] = uBaseClassGuid;
}

TString CComponentProxyManager::QueryComponentName(uint32_t uGuid) const
{
    TString strRet;
    std::map<uint32_t, TString>::const_iterator iter = m_abstractComponentNameMap.find(uGuid);
    if (iter != m_abstractComponentNameMap.end())
    {
        strRet = iter->second;
    }
    else
    {
        CComponentBase* pComponent = GetComponentTemplate(uGuid);
        if (pComponent != NULL)
        {
            strRet = pComponent->GetClassStr();
        }
    }
    return strRet;
}

void CComponentProxyManager::RegisterAbstractComponent(uint32_t uGuid, const TString& strName)
{
    BEATS_ASSERT(m_abstractComponentNameMap.find(uGuid) == m_abstractComponentNameMap.end());
    m_abstractComponentNameMap[uGuid] = strName;
}

void CComponentProxyManager::SaveCurFile()
{
    if (m_uCurrViewFileId != 0xFFFFFFFF)
    {
        const std::map<uint32_t, CComponentProxy*>& componentsInScene = GetComponentsInCurScene();
        std::map<uint32_t, std::vector<CComponentProxy*>> guidGroup;
        for (std::map<uint32_t, CComponentProxy*>::const_iterator iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            uint32_t uGuid = iter->second->GetGuid();
            if (guidGroup.find(uGuid) == guidGroup.end())
            {
                guidGroup[uGuid] = std::vector<CComponentProxy*>();
            }
            guidGroup[uGuid].push_back(iter->second);
        }

        const TString& strComponentFileName = m_pProject->GetComponentFileName(m_uCurrViewFileId);
        BEATS_ASSERT(!strComponentFileName.empty());
        SaveToFile(strComponentFileName.c_str(), guidGroup);

        // Recycle all id, because we will reload it in CComponentProject::ReloadFile
        for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            m_pIdManager->RecycleId(iter->first);
        }
        // We just reload the whole file to keep the id manager is working in the right way.
        m_pProject->ReloadFile(m_uCurrViewFileId);
    }
}

void CComponentProxyManager::SaveToFile(const TCHAR* pszFileName, std::map<uint32_t, std::vector<CComponentProxy*>>& components)
{
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* pDecl = doc.allocate_node(rapidxml::node_declaration);
    rapidxml::xml_attribute<>* pDecl_ver = doc.allocate_attribute("version", "1.0");
    pDecl->append_attribute(pDecl_ver);
    doc.append_node(pDecl);
    rapidxml::xml_node<>* pRootElement = doc.allocate_node(rapidxml::node_element, "Root");
    doc.append_node(pRootElement);

    rapidxml::xml_node<>* pComponentListElement = doc.allocate_node(rapidxml::node_element, "Components");
    pRootElement->append_node(pComponentListElement);
    for (std::map<uint32_t, std::vector<CComponentProxy*>>::iterator iter = components.begin(); iter != components.end(); ++iter)
    {
        rapidxml::xml_node<>* pComponentElement = doc.allocate_node(rapidxml::node_element, "Component");
        char szGUIDHexStr[32] = {0};
        sprintf(szGUIDHexStr, "0x%x", iter->first);
        pComponentElement->append_attribute(doc.allocate_attribute("GUID", doc.allocate_string(szGUIDHexStr)));
        pComponentElement->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(GetComponentTemplate(iter->first)->GetClassStr())));
        pComponentListElement->append_node(pComponentElement);
        for (uint32_t i = 0; i < iter->second.size(); ++i)
        {
            CComponentProxy* pProxy = static_cast<CComponentProxy*>(iter->second.at(i));
            pProxy->Save();
            pProxy->SaveToXML(pComponentElement, false);
        }
    }

    TString strOut;
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    rapidxml::print(std::back_inserter(strOut), doc, 0);
#endif
    std::ofstream out(pszFileName);
    out << strOut;
    out.close();
}

void CComponentProxyManager::RegisterPropertyCreator( uint32_t enumType, TCreatePropertyFunc func )
{
    BEATS_ASSERT(m_pPropertyCreatorMap->find(enumType) == m_pPropertyCreatorMap->end());
    (*m_pPropertyCreatorMap)[enumType] = func;
}

CPropertyDescriptionBase* CComponentProxyManager::CreateProperty( uint32_t propertyType, CSerializer* serializer )
{
    CPropertyDescriptionBase* pPropertyBase = (*(*m_pPropertyCreatorMap)[propertyType])(serializer);
    return pPropertyBase;
}

void CComponentProxyManager::ResolveDependency()
{
    for (uint32_t i = 0; i < m_pDependencyResolver->size(); ++i)
    {
        SDependencyResolver* pDependencyResolver = (*m_pDependencyResolver)[i];
        CComponentBase* pComponentToBeLink = GetComponentInstance(pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pComponentToBeLink != NULL, _T("Resolve dependency failed, Comopnent id %d guid 0x%x doesn't exist!"), pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pDependencyResolver->pDescription != NULL);
        BEATS_ASSERT(pDependencyResolver->pVariableAddress == NULL, "pVariableAddress should be null in proxy mode");
        BEATS_ASSERT(pComponentToBeLink != NULL, 
            _T("Component %s id %d can't resolve its dependency %s to component guid 0x%x id %d, have you deleted that component recently?"),
            pDependencyResolver->pDescription->GetOwner()->GetClassStr(), 
            pDependencyResolver->pDescription->GetOwner()->GetId(),
            pDependencyResolver->pDescription->GetDisplayName(),
            pDependencyResolver->uGuid,
            pDependencyResolver->uInstanceId);
        BEATS_ASSERT(pDependencyResolver->pDescription->GetDependencyLine(pDependencyResolver->uIndex)->GetConnectedComponent() == NULL);
        pDependencyResolver->pDescription->SetDependency(pDependencyResolver->uIndex, static_cast<CComponentProxy*>(pComponentToBeLink));
    }
    BEATS_SAFE_DELETE_VECTOR(*m_pDependencyResolver);
}

CPropertyDescriptionBase* CComponentProxyManager::GetCurrReflectProperty(EReflectOperationType* pOperateType) const
{
    if (pOperateType != nullptr)
    {
        *pOperateType = m_reflectOperateType;
    }
    // When we are in clone state or loading file state, we don't allow to reflect sync single property
    return !CComponentInstanceManager::GetInstance()->IsInClonePhase() && !CComponentInstanceManager::GetInstance()->IsInLoadingPhase() ? m_pCurrReflectProperty : nullptr;
}

void CComponentProxyManager::SetCurrReflectProperty(CPropertyDescriptionBase* pPropertyDescription, EReflectOperationType operateType)
{
    BEATS_ASSERT(pPropertyDescription == nullptr || m_pCurrReflectDependency == nullptr, "Reflect property and dependency can't both be set value");
    m_pCurrReflectProperty = pPropertyDescription;
    m_reflectOperateType = operateType;
}

CSerializer& CComponentProxyManager::GetRemoveChildInfo()
{
    BEATS_ASSERT(m_pCurrReflectProperty != nullptr && m_pCurrReflectProperty->IsContainerProperty());
    return *m_pRemoveChildInfo;
}

CDependencyDescription* CComponentProxyManager::GetCurrReflectDependency() const
{
    // When we are in clone state or loading file state, we don't allow to reflect sync single dependency
    return !CComponentInstanceManager::GetInstance()->IsInClonePhase() && !CComponentInstanceManager::GetInstance()->IsInLoadingPhase() ? m_pCurrReflectDependency : nullptr;
}

void CComponentProxyManager::SetCurrReflectDependency(CDependencyDescription* pDependency)
{
    BEATS_ASSERT(pDependency == nullptr || m_pCurrReflectProperty == nullptr, "Reflect property and dependency can't both be set value");
    m_pCurrReflectDependency = pDependency;
}

CComponentProxy* CComponentProxyManager::GetCurrUpdateProxy() const
{
    return m_pCurrUpdateProxy;
}

void CComponentProxyManager::SetCurrUpdateProxy(CComponentProxy* pProxy)
{
    m_pCurrUpdateProxy = pProxy;
}

const std::map<uint32_t, TString>& CComponentProxyManager::GetAbstractComponentNameMap() const
{
    return m_abstractComponentNameMap;
}

bool CComponentProxyManager::IsParent(uint32_t uParentGuid, uint32_t uChildGuid) const
{
    bool bRet = false;
    if(uParentGuid == uChildGuid)
    {
        bRet = true;
    }
    else
    {
        std::set<uint32_t> subClassGuids;
        QueryDerivedClass(uParentGuid, subClassGuids, true);
        if(std::find(subClassGuids.begin(), subClassGuids.end(), uChildGuid) != subClassGuids.end())
        {
            bRet = true;
        }
    }
    return bRet;
}

std::map<uint32_t, CComponentProxy*>& CComponentProxyManager::GetComponentsInCurScene() 
{
    return m_proxyInCurScene;
}

void CComponentProxyManager::OnCreateComponentInScene(CComponentProxy* pProxy)
{
    BEATS_ASSERT(m_proxyInCurScene.find(pProxy->GetId()) == m_proxyInCurScene.end());
    m_proxyInCurScene[pProxy->GetId()] = pProxy;
}

void CComponentProxyManager::OnDeleteComponentInScene(CComponentProxy* pProxy)
{
    BEATS_ASSERT(m_proxyInCurScene.find(pProxy->GetId()) != m_proxyInCurScene.end());
    CComponentInstance* pHostComponent = pProxy->GetHostComponent();
    if (pHostComponent != NULL)
    {
        BEATS_ASSERT(m_proxyInCurScene.find(pProxy->GetId()) != m_proxyInCurScene.end())
            m_proxyInCurScene.erase(pProxy->GetId());
        pHostComponent->Unload();
        pHostComponent->Uninitialize();
        BEATS_SAFE_DELETE(pHostComponent);
    }
}

bool CComponentProxyManager::IsExporting() const
{
    return m_bExportingPhase;
}

bool CComponentProxyManager::IsEnableCreateInstanceWithProxy() const
{
    return m_bCreateInstanceWithProxy;
}

void CComponentProxyManager::SetEnableCreateInstanceWithProxy(bool bFlag)
{
    m_bCreateInstanceWithProxy = bFlag;
}

std::set<uint32_t>& CComponentProxyManager::GetRefreshFileList()
{
    return m_refreshFileList;
}

uint32_t CComponentProxyManager::GetOperateProgress(TString& strCurrOperateFile) const
{
    strCurrOperateFile = m_strCurrOperateFile;
    return m_uOperateProgress;
}

void CComponentProxyManager::CheckForUnInvokedGuid(std::set<uint32_t>& uninvokeGuidList)
{
    m_bExportingPhase = true;
    uninvokeGuidList.clear();
    std::set<uint32_t> invokedGuidList;
    const std::map<uint32_t, std::vector<uint32_t> >* pTypeToComponentMap = m_pProject->GetTypeToComponentMap();
    for (auto iter = pTypeToComponentMap->begin(); iter != pTypeToComponentMap->end(); ++iter)
    {
        uint32_t uGuid = iter->first;
        invokedGuidList.insert(uGuid);
    }
    const std::map<uint32_t, std::set<uint32_t> >* pRefMap = m_pProject->GetTypeRefInComponentMap();
    for (auto iter = pRefMap->begin(); iter != pRefMap->end(); ++iter)
    {
        uint32_t uGuid = iter->first;
        invokedGuidList.insert(uGuid);
    }
    const std::map<uint32_t, CComponentBase*>* pTemplateMap = GetComponentTemplateMap();
    for (auto iter = pTemplateMap->begin(); iter != pTemplateMap->end(); ++iter)
    {
        BEATS_ASSERT(uninvokeGuidList.find(iter->first) == uninvokeGuidList.end());
        uninvokeGuidList.insert(iter->first);
    }
    for (auto iter = m_abstractComponentNameMap.begin(); iter != m_abstractComponentNameMap.end(); ++iter)
    {
        BEATS_ASSERT(uninvokeGuidList.find(iter->first) == uninvokeGuidList.end());
        uninvokeGuidList.insert(iter->first);
    }
    for (auto iter = invokedGuidList.begin(); iter != invokedGuidList.end(); ++iter)
    {
        uint32_t uGuid = *iter;
        uninvokeGuidList.erase(uGuid);
        uint32_t uBaseClassGuid = QueryBaseClass(uGuid);
        while (uBaseClassGuid != 0xFFFFFFFF)
        {
            uninvokeGuidList.erase(uBaseClassGuid);
            uBaseClassGuid = QueryBaseClass(uBaseClassGuid);
        }
    }
    m_bExportingPhase = false;
}

void CComponentProxyManager::ReSaveFreshFile()
{
    const std::vector<uint32_t>& loadedFiles = GetLoadedFiles();
    for (auto iter = m_refreshFileList.begin(); iter != m_refreshFileList.end(); ++iter)
    {
        bool bIsLoaded = std::find(loadedFiles.begin(), loadedFiles.end(), *iter) != loadedFiles.end();
        if (bIsLoaded)
        {
            const std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
            auto subIter = pFileToComponentMap->find(*iter);
            if (subIter != pFileToComponentMap->end())
            {
                std::map<uint32_t, std::vector<CComponentProxy*>> guidGroup;
                for (auto componentMapIter = subIter->second.begin(); componentMapIter != subIter->second.end(); ++componentMapIter)
                {
                    for (auto idIter = componentMapIter->second.begin(); idIter != componentMapIter->second.end(); ++idIter)
                    {
                        uint32_t uComponentId = *idIter;
                        CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                        if (pProxy != NULL) // The proxy may be null when you remove some components in code.
                        {
                            std::map<uint32_t, std::vector<CComponentProxy*>>::iterator guidGroupIter = guidGroup.find(pProxy->GetGuid());
                            if (guidGroupIter == guidGroup.end())
                            {
                                guidGroup[pProxy->GetGuid()] = std::vector<CComponentProxy*>();
                                guidGroupIter = guidGroup.find(pProxy->GetGuid());
                            }
                            guidGroupIter->second.push_back(pProxy);
                        }
                    }
                }
                const TString& strFileName = m_pProject->GetComponentFileName(*iter);
                SaveToFile(strFileName.c_str(), guidGroup);
            }
        }
    }
    m_refreshFileList.clear();
}

bool CComponentProxyManager::ExportComponentProxy(uint32_t uFileId, CComponentProxy* pProxy, CSerializer& serializer, std::function<bool(uint32_t, CComponentProxy*)> exportCallback)
{
    bool bNeedRefreshFile = false;
    if (exportCallback != nullptr)
    {
        bNeedRefreshFile = exportCallback(uFileId, pProxy);
    }
    pProxy->ExportDataToHost(serializer, eVT_SavedValue);
    return bNeedRefreshFile;
}

void CComponentProxyManager::SetCheckUselessResourceValue(bool bCheckUselessResource)
{
    m_bCheckUselessResource = bCheckUselessResource;
}

bool CComponentProxyManager::IsCheckUselessResource()
{
    return m_bCheckUselessResource;
}
