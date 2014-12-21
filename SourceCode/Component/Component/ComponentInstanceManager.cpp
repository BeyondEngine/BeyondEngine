#include "stdafx.h"
#include "ComponentInstanceManager.h"
#include "ComponentBase.h"
#include "ComponentProxy.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Utility/BeatsUtility/IdManager.h"
#include "ComponentProjectDirectory.h"
#include "ComponentProject.h"

static const TCHAR* pszBattleFiles[] =
{
    "gameconfig.bcf",
    "heroskillprototype.bcf",
    "basicskillprototype.bcf",
    "energybuildingprototype.bcf",
    "zergsoldierprototype.bcf",
    "naturalresource.bcf",
    "heroprototype.bcf",
    "landingshipprototype.bcf",
    "soldierprototype.bcf",
    "buildingprototype.bcf",
};

CComponentInstanceManager* CComponentInstanceManager::m_pInstance = NULL;
CComponentInstanceManager::CComponentInstanceManager()
{
    m_pSerializer = new CSerializer;
}

CComponentInstanceManager::~CComponentInstanceManager()
{
    Release();
    BEATS_SAFE_DELETE(m_pSerializer);
}

CSerializer* CComponentInstanceManager::Import(const TCHAR* pszFilePath)
{
    m_pSerializer->Reset();
    m_pSerializer->Serialize(pszFilePath);
    CSerializer& serializer = *m_pSerializer;
    uint32_t uVersion = 0;
    serializer >> uVersion;
    if (uVersion != COMPONENT_SYSTEM_VERSION)
    {
        TCHAR szInfo[256];
        _stprintf(szInfo, _T("Data version is %d while our program version is %d"), uVersion, COMPONENT_SYSTEM_VERSION);
        MessageBox(BEYONDENGINE_HWND, szInfo, _T("Verion Not Match!"), MB_OK | MB_ICONERROR);
    }
    else
    {
        uint32_t uComponentTemplateCount = 0;
        serializer >> uComponentTemplateCount;
        CComponentProjectDirectory* pRootDirectory = new CComponentProjectDirectory(NULL, _T("Root"));
        m_pProject->SetRootDirectory(pRootDirectory);
        pRootDirectory->Deserialize(serializer);
        uint32_t uStartFile;
        serializer >> uStartFile;
        m_pProject->SetStartFile(uStartFile);
        // 1. Load binarize data and file structure.
        uint32_t uFileCount = 0;
        serializer >> uFileCount;
        std::map<uint32_t, std::vector<uint32_t> > conflictMap;
        for (uint32_t i = 0; i < uFileCount; ++i)
        {
            TString strFileName;
            serializer >> strFileName;
            // This is a hack way here!
            const_cast<std::vector<TString>*>(m_pProject->GetFileList())->push_back(strFileName);
            BEATS_ASSERT(conflictMap.size() == 0);

            uint32_t uStartPos = 0;
            serializer >> uStartPos;
            BEATS_ASSERT(uStartPos == (serializer.GetReadPos() - sizeof(uStartPos)), _T("File start pos not match!"));
            uint32_t uFileSize = 0;
            serializer >> uFileSize;
            m_pProject->RegisterFileLayoutInfo(i, uStartPos, uFileSize);
            uint32_t uComponentCount = 0;
            serializer >> uComponentCount;

            for (uint32_t j = 0; j < uComponentCount; ++j)
            {
                uint32_t uComponentDataSize, uGuid, uId;
                uint32_t uComponentStartPos = m_pSerializer->GetReadPos();
                serializer >> uComponentDataSize >> uGuid >> uId;
                m_pProject->RegisterComponent(i, uGuid, uId);
                serializer.SetReadPos(uComponentStartPos + uComponentDataSize);
            }

            BEATS_ASSERT(uStartPos + uFileSize <= serializer.GetWritePos(), _T("Data overflow!"));
            serializer.SetReadPos(uStartPos + uFileSize);
        }
        BEATS_ASSERT(serializer.GetReadPos() == serializer.GetWritePos(), _T("Some data are not loaded completly. loaded data size %d, all data size %d"), serializer.GetReadPos(), serializer.GetWritePos());
        // 2. Load start up file.
        SwitchFile(uStartFile);
    }
    return m_pSerializer;
}

void CComponentInstanceManager::SwitchFile(uint32_t uFileId)
{
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    std::vector<uint32_t> loadFiles, unloadFiles;
    bool bNewAddFile = false; // Useless here. Only useful when CComponentProxyManager call CalcSwitchFile.
    CalcSwitchFile(uFileId, loadFiles, unloadFiles, bNewAddFile);
    BEATS_ASSERT(!bNewAddFile, _T("This can't happen in CComponentInstanceManager, it can be true only in CComponentProxyManager"));
    bool bLoadThisFile = loadFiles.size() > 0 && loadFiles.back() == uFileId;
    if (bLoadThisFile)
    {
        m_uCurrLoadFileId = uFileId;
    }
    for (uint32_t i = 0; i < unloadFiles.size(); ++i)
    {
        CloseFile(unloadFiles[i]);
    }
    std::vector<CComponentBase*> loadedComponents;
    for (uint32_t i = 0; i < loadFiles.size(); ++i)
    {
        LoadFile(loadFiles[i], &loadedComponents);
    }
    ResolveDependency();
    for (uint32_t i = 0; i < loadedComponents.size(); ++i)
    {
        loadedComponents[i]->Initialize();
    }
    BEYONDENGINE_CHECK_HEAP;
}

void CComponentInstanceManager::LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pLoadComponents)
{
    uint32_t uFileStartPos = 0;
    uint32_t uFileDataLength = 0;
    bool bRet = m_pProject->QueryFileLayoutInfo(uFileId, uFileStartPos, uFileDataLength);
    BEATS_ASSERT(bRet, _T("Query file layout info failed! file id %d"), uFileId);
    if (bRet)
    {
        // Use a serializer proxy to avoid multi-thread load the same serializer
        CSerializer dataProxy;
        dataProxy.SetBuffer(m_pSerializer->GetBuffer(), m_pSerializer->GetWritePos(), false);
        std::vector<CComponentInstance*> loadComponents;
        dataProxy.SetReadPos(uFileStartPos);
        uint32_t uFileStartPosRead, uFileDataLengthRead;
        dataProxy >> uFileStartPosRead >> uFileDataLengthRead;
        BEATS_ASSERT(uFileStartPosRead == uFileStartPos && uFileDataLengthRead == uFileDataLength);
        uint32_t uComponentCount = 0;
        dataProxy >> uComponentCount;
        bool bOriginalLoadPhase = IsInLoadingPhase();
        BEATS_ASSERT(!bOriginalLoadPhase, "I don't think it will nest!");
        SetLoadPhaseFlag(true);
        for (uint32_t j = 0; j < uComponentCount; ++j)
        {
            uint32_t uComponentDataSize, uGuid, uId;
            uint32_t uComponentStartPos = dataProxy.GetReadPos();
            dataProxy >> uComponentDataSize >> uGuid >> uId;
            CComponentInstance* pComponent = down_cast<CComponentInstance*>(CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, false, uId, true, &dataProxy, false));
            pComponent->SetDataPos(uComponentStartPos);
            pComponent->SetDataSize(uComponentDataSize);
            BEATS_ASSERT(pComponent != NULL);
            if (pLoadComponents != nullptr)
            {
                pLoadComponents->push_back(pComponent);
            }
            loadComponents.push_back(pComponent);
            BEATS_ASSERT(uComponentStartPos + uComponentDataSize == dataProxy.GetReadPos(), _T("Component Data Not Match!\nGot an error when import data for component %x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, pComponent->GetClassStr(), uId, uComponentDataSize, dataProxy.GetReadPos() - uComponentStartPos);
            dataProxy.SetReadPos(uComponentStartPos + uComponentDataSize);
        }
        BEATS_ASSERT(dataProxy.GetReadPos() - uFileStartPos == uFileDataLength, _T("File Data NOT Match!\nGot an error when import data for file %d Required size:%d Actual size %d"), uFileId, uFileDataLength, dataProxy.GetReadPos() - uFileStartPos);
        dataProxy.SetBuffer(nullptr, 0, false);
        ResolveDependency();
        SetLoadPhaseFlag(bOriginalLoadPhase);
        // After all component instance's value is read, we call each's load function.
        for (size_t i = 0; i < loadComponents.size(); ++i)
        {
            loadComponents[i]->Load();
        }
        m_loadedFiles.push_back(uFileId);
        BEYONDENGINE_CHECK_HEAP;
    }
}

void CComponentInstanceManager::UnloadFile(uint32_t uFileId, std::vector<CComponentBase*>* pUnloadComponents)
{
    std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId);
    BEATS_WARNING(iterFile != m_loadedFiles.end(), "Close an unopened file %d, this may be right if we are exiting the program.", uFileId);
    if (iterFile != m_loadedFiles.end())
    {
        m_loadedFiles.erase(iterFile);
        const std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponentMap->find(uFileId);
        if (iter != pFileToComponentMap->end())
        {
            for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
            {
                for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
                {
                    uint32_t uComponentId = *idIter;
                    BEATS_ASSERT(uComponentId != 0xFFFFFFFF);
                    CComponentBase* pComponentBase = CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId);
                    // This may be null, because some components can be uninitialized by other component's un-initialize.
                    if (pComponentBase != NULL)
                    {
                        BEATS_ASSERT(pComponentBase->IsLoaded());
                        pComponentBase->Unload();
                        pUnloadComponents->push_back(pComponentBase);
                    }
                }
            }
        }
    }
}

void CComponentInstanceManager::CloseFile(uint32_t uFileId)
{
    std::vector<CComponentBase*> componentToDelete;
    UnloadFile(uFileId, &componentToDelete);
    BEYONDENGINE_CHECK_HEAP;
    for (size_t i = 0; i < componentToDelete.size(); ++i)
    {
        CComponentBase* pComponentBase = componentToDelete[i];
        if (pComponentBase->IsInitialized())
        {
            pComponentBase->Uninitialize();
        }
        else
        {
            uint32_t uComponentId = pComponentBase->GetId();
            if (uComponentId != 0xFFFFFFFF)
            {
                CComponentInstanceManager::GetInstance()->UnregisterInstance(pComponentBase);
                CComponentInstanceManager::GetInstance()->GetIdManager()->RecycleId(uComponentId);
            }
        }
        BEYONDENGINE_CHECK_HEAP;
    }
    for (uint32_t i = 0; i < componentToDelete.size(); ++i)
    {
        BEATS_SAFE_DELETE(componentToDelete[i]);
        BEYONDENGINE_CHECK_HEAP;
    }
}

CSerializer* CComponentInstanceManager::GetFileSerializer() const
{
    return m_pSerializer;
}

void CComponentInstanceManager::ResolveDependency()
{
    for (uint32_t i = 0; i < m_pDependencyResolver->size(); ++i)
    {
        SDependencyResolver* pDependencyResolver = (*m_pDependencyResolver)[i];
        CComponentBase* pComponentToBeLink = GetComponentInstance(pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pComponentToBeLink != NULL, _T("Resolve dependency failed, Comopnent id %d guid 0x%x doesn't exist!"), pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pDependencyResolver->pDescription == NULL);
        if (pDependencyResolver->pVariableAddress != NULL)
        {
            if (pDependencyResolver->bIsList)
            {
                pDependencyResolver->pAddFunc(pDependencyResolver->pVariableAddress, pComponentToBeLink);
            }
            else
            {
                BEATS_ASSERT(*(ptrdiff_t*)pDependencyResolver->pVariableAddress == 0, "the variable of address should be cleared.");
                *(ptrdiff_t*)pDependencyResolver->pVariableAddress = (ptrdiff_t)pComponentToBeLink;
            }
        }
    }
    BEATS_SAFE_DELETE_VECTOR(*m_pDependencyResolver);
}

bool CComponentInstanceManager::IsInClonePhase() const
{
    return m_bInClonePhase;
}

void CComponentInstanceManager::SetClonePhaseFlag(bool bInClonePhase)
{
    m_bInClonePhase = bInClonePhase;
}

bool CComponentInstanceManager::IsInLoadingPhase() const
{
    return m_bInLoadingPhase;
}

void CComponentInstanceManager::SetLoadPhaseFlag(bool bInLoadPhase)
{
    m_bInLoadingPhase = bInLoadPhase;
}

uint32_t CComponentInstanceManager::GetVersion()
{
    return COMPONENT_SYSTEM_VERSION;
}

void CComponentInstanceManager::LoadDirectoryFiles(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>& loadComponents)
{
    size_t uFileCount = pDirectory->GetFileList().size();
    for (size_t i = 0; i < uFileCount; ++i)
    {
        uint32_t uFileId = pDirectory->GetFileList().at(i);
        LoadFile(uFileId, &loadComponents);
    }
}
