#include "stdafx.h"
#include "ComponentManagerBase.h"
#include "Utility/BeatsUtility/IdManager.h"
#include "ComponentProject.h"
#include "ComponentProjectDirectory.h"

void DefaultAddDependencyFunc(void* pContainer, void* pDependency)
{
    std::vector<void*>* pVector = (std::vector<void*>*)pContainer;
    BEATS_ASSERT(std::find(pVector->begin(), pVector->end(), pDependency) == pVector->end(), "component repeated in the same dependency list");
    pVector->push_back((void*)pDependency);
}

CComponentManagerBase::CComponentManagerBase()
    : m_uCurrLoadFileId(0xFFFFFFFF)
{
#ifndef EDITOR_MODE
    BEATS_ASSERT(!CApplication::GetInstance()->IsDestructing(), "Should not create singleton when exit the program!");

#endif // EDITOR_MODE
    m_pIdManager = new CIdManager;
    m_pProject = new CComponentProject;
    m_pComponentTemplateMap = new std::map<uint32_t, CComponentBase*>;
    m_pComponentInstanceMap = new std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>;
    m_pDependencyResolver = new std::vector<SDependencyResolver*>;
}

CComponentManagerBase::~CComponentManagerBase()
{

}

void CComponentManagerBase::Release()
{
    BEATS_SAFE_DELETE(m_pIdManager);
    BEATS_SAFE_DELETE(m_pProject);
    BEATS_ASSERT(m_pComponentInstanceMap->size() == 0);
    BEATS_SAFE_DELETE(m_pComponentInstanceMap);
    BEATS_SAFE_DELETE_MAP(*m_pComponentTemplateMap);
    BEATS_SAFE_DELETE(m_pComponentTemplateMap);
    BEATS_SAFE_DELETE(m_pDependencyResolver);
}

bool CComponentManagerBase::RegisterTemplate( CComponentBase* pComponent )
{
    std::map<uint32_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->find(pComponent->GetGuid());
    BEATS_ASSERT(iter == m_pComponentTemplateMap->end(), _T("component %s guid 0x%x is already registered!"), pComponent->GetClassStr(), pComponent->GetGuid());
    if (iter == m_pComponentTemplateMap->end())
    {
        (*m_pComponentTemplateMap)[pComponent->GetGuid()] = pComponent;
    }
    return iter == m_pComponentTemplateMap->end();
}

bool CComponentManagerBase::UnregisterTemplate(CComponentBase* pComponent)
{
    std::map<uint32_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->find(pComponent->GetGuid());
    bool bRet = iter != m_pComponentTemplateMap->end();
    BEATS_ASSERT(bRet, _T("component %s guid 0x%x is not registered!"), pComponent->GetClassStr(), pComponent->GetGuid());
    if (bRet)
    {
        m_pComponentTemplateMap->erase(iter);
    }
    return bRet;
}

bool CComponentManagerBase::RegisterInstance(CComponentBase* pNewInstance)
{
    uint32_t guid = pNewInstance->GetGuid();
    std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(guid);
    if (iter == m_pComponentInstanceMap->end())
    {
        (*m_pComponentInstanceMap)[guid] = new std::map<uint32_t, CComponentBase*>;
        iter = m_pComponentInstanceMap->find(guid);
    }
    BEATS_ASSERT(iter->second->find(pNewInstance->GetId()) == iter->second->end(), _T("Create an existing component! GUID:0x%x, InstanceId: %d"), guid, pNewInstance->GetId());
    (*iter->second)[pNewInstance->GetId()] = pNewInstance;
    return true;
}

bool CComponentManagerBase::UnregisterInstance(CComponentBase* pNewInstance)
{
    uint32_t guid = pNewInstance->GetGuid();
    std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(guid);
    BEATS_ASSERT(iter != m_pComponentInstanceMap->end(), _T("Unregister instance component %s guid 0x%x id %d failed!"), 
        pNewInstance->GetClassStr(), pNewInstance->GetGuid(), pNewInstance->GetId());
    std::map<uint32_t, CComponentBase*>::iterator subIter = iter->second->find(pNewInstance->GetId());
    BEATS_ASSERT(subIter != iter->second->end(), _T("Unregister instance component %s guid 0x%x id %d failed!"), 
        pNewInstance->GetClassStr(), pNewInstance->GetGuid(), pNewInstance->GetId());
    iter->second->erase(subIter);
    if (iter->second->size() == 0)
    {
        BEATS_SAFE_DELETE(iter->second);
        m_pComponentInstanceMap->erase(iter);
    }
    return true;
}

CComponentBase* CComponentManagerBase::GetComponentTemplate( uint32_t guid ) const
{
    CComponentBase* pResult = NULL;
    std::map<uint32_t, CComponentBase*>::const_iterator iter = m_pComponentTemplateMap->find(guid);
    if (iter != m_pComponentTemplateMap->end())
    {
        pResult = iter->second;
    }
    return pResult;
}

CComponentBase* CComponentManagerBase::CreateComponent( uint32_t guid, bool bCloneFromTemplate, bool bManualManage/* = false*/, uint32_t specifiedInstanceId /*=0xffffffff*/, bool bCheckRequestId/* = true*/, CSerializer* pData /*=NULL*/, bool bCallInitFunc/* = true*/)
{
    CComponentBase* pNewInstance = NULL;
    CComponentBase* pTemplate = GetComponentTemplate(guid);
    BEATS_ASSERT(pTemplate != NULL, _T("Create an unknown component, Guid:0x%x id: %d"), guid, specifiedInstanceId);
    pNewInstance = CreateComponentByRef(pTemplate, bCloneFromTemplate, bManualManage, specifiedInstanceId, bCheckRequestId, pData, bCallInitFunc);
    return pNewInstance;
}

CComponentBase* CComponentManagerBase::CreateComponentByRef( CComponentBase* pComponentRef, bool bCloneValue, bool bManualManage /*= false*/, uint32_t specifiedInstanceId /*= 0xFFFFFFFF*/, bool bCheckRequestId /*= true*/, CSerializer* pData /*= NULL*/, bool bCallInitFunc/* = true*/)
{
    BEATS_ASSERT(pComponentRef != NULL, _T("Create an unknown component by NULL ref"));
    if (!bManualManage)
    {
        if (specifiedInstanceId != 0xFFFFFFFF)
        {
            m_pIdManager->ReserveId(specifiedInstanceId, bCheckRequestId);
        }
        else
        {
            specifiedInstanceId = m_pIdManager->GenerateId();
        }
    }
    else
    {
        BEATS_ASSERT(specifiedInstanceId == 0xFFFFFFFF, _T("If you want to manage component instance by yourself, don't require instance id!"));
    }

    // TODO: bCloneValue only makes sense when this is a CComponentProxy.
    CComponentBase* pNewInstance = static_cast<CComponentBase*>(pComponentRef->Clone(bCloneValue, pData, specifiedInstanceId, bCallInitFunc));
    if (!bManualManage)
    {
        RegisterInstance(pNewInstance);
    }
    return pNewInstance;
}


const std::map<uint32_t, CComponentBase*>* CComponentManagerBase::GetComponentTemplateMap()
{
    return m_pComponentTemplateMap;
}

const std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>* CComponentManagerBase::GetComponentInstanceMap()
{
    return m_pComponentInstanceMap;
}

CComponentBase* CComponentManagerBase::GetComponentInstance( uint32_t uId , uint32_t uGUID/* = 0xffffffff*/)
{
    CComponentBase* pResult = NULL;
    if (uGUID != 0xFFFFFFFF)
    {
        std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(uGUID);
        if (iter != m_pComponentInstanceMap->end())
        {
            std::map<uint32_t, CComponentBase*>::iterator subIter = iter->second->find(uId);
            if (subIter != iter->second->end())
            {
                pResult = subIter->second;
            }
        }
    }
    else
    {
        std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin();
        for (; iter != m_pComponentInstanceMap->end(); ++iter)
        {
            pResult = GetComponentInstance(uId, iter->first);
            if (pResult != NULL)
            {
                break;
            }
        }
    }
    return pResult;
}

const std::vector<uint32_t>& CComponentManagerBase::GetLoadedFiles() const
{
    return m_loadedFiles;
}

CIdManager* CComponentManagerBase::GetIdManager() const
{
    return m_pIdManager;
}

CComponentProject* CComponentManagerBase::GetProject() const
{
    return m_pProject;
}

void CComponentManagerBase::Reset(bool bDeleteTemplate)
{
    std::vector<uint32_t> bak = m_loadedFiles;
    for (int32_t i = bak.size() - 1; i >= 0; --i)
    {
        CloseFile(bak[i]);
    }
    m_uCurrLoadFileId = 0xFFFFFFFF;
    BEATS_ASSERT(m_loadedFiles.size() == 0);
    BEATS_ASSERT(m_pComponentInstanceMap->size() == 0);
    if (bDeleteTemplate)
    {
        for (auto iter = m_pComponentTemplateMap->begin(); iter != m_pComponentTemplateMap->end(); ++iter)
        {
            BEATS_SAFE_DELETE(iter->second);
        }
        m_pComponentTemplateMap->clear();
    }
    m_pIdManager->Reset();
    m_pProject->CloseProject();
    BEYONDENGINE_CHECK_HEAP;
}

uint32_t CComponentManagerBase::GetCurrLoadFileId()
{
    return m_uCurrLoadFileId;
}

void CComponentManagerBase::SetCurrLoadFileId(uint32_t uId)
{
    m_uCurrLoadFileId = uId;
}

void CComponentManagerBase::AddDependencyResolver( CDependencyDescription* pDescription, uint32_t uIndex, uint32_t uGuid, uint32_t uInstanceId , void* pVariableAddress, bool bIsList, TAddDependencyFunc pFunc /*= NULL*/)
{
    if (!CComponentInstanceManager::GetInstance()->IsInClonePhase() || CComponentInstanceManager::GetInstance()->IsInLoadingPhase())
    {
        SDependencyResolver* pDependencyResovler = new SDependencyResolver;
        pDependencyResovler->pDescription = pDescription;
        pDependencyResovler->uIndex = uIndex;
        pDependencyResovler->uGuid = uGuid;
        pDependencyResovler->uInstanceId = uInstanceId;
#ifdef _DEBUG
        // pVariableAddress is always null for CComponentProxyManager
        if (pVariableAddress)
        {
            if (bIsList)
            {
                std::vector<void*>* pVector = (std::vector<void*>*)pVariableAddress;
                // pVariableAddress is always null for CComponentProxyManager
                BEATS_ASSERT(pVector->size() == 0, "the variable of address should be cleared.");
            }
            else
            {
                BEATS_ASSERT(*(ptrdiff_t*)pVariableAddress == 0, "the variable of address should be cleared.");
            }
        }
#endif
        pDependencyResovler->pVariableAddress = pVariableAddress;
        pDependencyResovler->bIsList = bIsList;
        pDependencyResovler->pAddFunc = pFunc == NULL ? DefaultAddDependencyFunc : pFunc;
        m_pDependencyResolver->push_back(pDependencyResovler);
    }
}

void CComponentManagerBase::CalcSwitchFile(uint32_t uFileId, std::vector<uint32_t>& loadFiles, std::vector<uint32_t>& unloadFiles, bool& bNewAddFile)
{
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    bool bLoadThisFile = true;
    // 1. File is in the parent directory (loaded before): just change the content of m_proxyInCurScene
    if (std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId) != m_loadedFiles.end())
    {
        // Change content will be done at last. So do nothing here.
        bLoadThisFile = false;
        // Close any other loaded + unnecessary file.
        CComponentProjectDirectory* pDirectory = m_pProject->FindProjectDirectoryById(uFileId);
        CComponentProjectDirectory* pCurDirectory = m_pProject->FindProjectDirectoryById(m_uCurrLoadFileId);

        BEATS_ASSERT(pDirectory != NULL && pCurDirectory != NULL);
        if (pDirectory != pCurDirectory)
        {
            TString strLogicPath = pDirectory->MakeRelativeLogicPath(pCurDirectory);
            if (*strLogicPath.rbegin() == '/')
            {
                strLogicPath.resize(strLogicPath.size() - 1);
            }
            std::vector<TString> logicPaths;
            CStringHelper::GetInstance()->SplitString(strLogicPath.c_str(), _T("/"), logicPaths);
            BEATS_ASSERT(logicPaths.size() > 0);
            if (logicPaths.back().compare(_T("..")) == 0)
            {
                unloadFiles.push_back(m_uCurrLoadFileId);
                CComponentProjectDirectory* pCurLoopDirectory = pCurDirectory->GetParent();
                logicPaths.pop_back();
                while (logicPaths.size() > 0)
                {
                    if (logicPaths.back().compare(_T("..")) == 0)
                    {
                        const std::vector<uint32_t>& fileList = pCurLoopDirectory->GetFileList();
                        for (uint32_t i = 0; i < fileList.size(); ++i)
                        {
                            BEATS_ASSERT(std::find(m_loadedFiles.begin(), m_loadedFiles.end(), fileList[i]) != m_loadedFiles.end());
                            unloadFiles.push_back(fileList[i]);
                        }
                        pCurLoopDirectory = pCurLoopDirectory->GetParent();
                        logicPaths.pop_back();
                    }
                }
                BEATS_ASSERT(pDirectory == pCurLoopDirectory);
            }
            const std::vector<uint32_t>& fileList = pDirectory->GetFileList();
            for (uint32_t i = 0; i < fileList.size(); ++i)
            {
                BEATS_ASSERT(std::find(m_loadedFiles.begin(), m_loadedFiles.end(), fileList[i]) != m_loadedFiles.end());
                if (fileList[i] != uFileId)
                {
                    unloadFiles.push_back(fileList[i]);
                }
            }
        }
    }
    else
    {
        CComponentProjectDirectory* pDirectory = m_pProject->FindProjectDirectoryById(uFileId);
        BEATS_ASSERT(pDirectory != NULL);
        TString strLogicPath;
        // new open a file.
        if (m_uCurrLoadFileId == 0xFFFFFFFF)
        {
            std::vector<CComponentProjectDirectory*> directories;
            CComponentProjectDirectory* pCurDirectory = pDirectory->GetParent();
            while (pCurDirectory != NULL)
            {
                directories.push_back(pCurDirectory);
                pCurDirectory = pCurDirectory->GetParent();
            }
            while (directories.size() > 0)
            {
                size_t uFileCount = directories.back()->GetFileList().size();
                for (size_t i = 0; i < uFileCount; ++i)
                {
                    uint32_t uFileId = directories.back()->GetFileList().at(i);
                    loadFiles.push_back(uFileId);
                }
                directories.pop_back();
            }
        }
        else
        {
            CComponentProjectDirectory* pCurDirectory = m_pProject->FindProjectDirectoryById(m_uCurrLoadFileId);
            BEATS_ASSERT(pCurDirectory != NULL);
            strLogicPath = pDirectory->MakeRelativeLogicPath(pCurDirectory);

            // 2. File is at the same directory: close current file and open it.
            if (strLogicPath.empty())
            {
                unloadFiles.push_back(m_uCurrLoadFileId);
            }
            else
            {
                if (*strLogicPath.rbegin() == '/')
                {
                    strLogicPath.resize(strLogicPath.size() - 1);
                }
                std::vector<TString> logicPaths;
                CStringHelper::GetInstance()->SplitString(strLogicPath.c_str(), _T("/"), logicPaths);
                BEATS_ASSERT(logicPaths.size() > 0);
                // 3. File is in the son directory: don't close current, load rest files of directory and go on.
                if (logicPaths[0].compare(_T("..")) != 0)
                {
                    // Load rest files of the same directory.
                    const std::vector<uint32_t>& fileList = pCurDirectory->GetFileList();
                    for (uint32_t i = 0; i < fileList.size(); ++i)
                    {
                        if (fileList[i] != m_uCurrLoadFileId)
                        {
                            loadFiles.push_back(fileList[i]);
                        }
                    }
                    // Load sub-directory to target, but don't load the last sub-directory, because we only need one file in it, the target file.
                    CComponentProjectDirectory* pCurLoopDirectory = pCurDirectory;
                    for (int i = 0; i < (int)logicPaths.size() - 1; ++i)
                    {
                        pCurLoopDirectory = pCurLoopDirectory->FindChild(logicPaths[i].c_str());
                        BEATS_ASSERT(pCurLoopDirectory != NULL);
                        size_t uFileCount = pCurLoopDirectory->GetFileList().size();
                        for (size_t i = 0; i < uFileCount; ++i)
                        {
                            uint32_t uFileId = pCurLoopDirectory->GetFileList().at(i);
                            loadFiles.push_back(uFileId);
                        }
                    }
                }
                else// 4. File is in other different directory: close current and change directory.
                {
                    // This means the file should be already loaded (since it's in the parent directory)
                    // But it is not in the m_loadedFiles, so this file must be a new added one
                    // So we do nothing as if it is already loaded. This condition is occurred only in Editor mode.
                    bNewAddFile = logicPaths.back().compare(_T("..")) == 0;
                    if (!bNewAddFile)
                    {
                        unloadFiles.push_back(m_uCurrLoadFileId);
                        CComponentProjectDirectory* pCurLoopDirectory = pCurDirectory->GetParent();
                        BEATS_ASSERT(pCurLoopDirectory != NULL);
                        for (int i = 1; i < (int)logicPaths.size() - 1; ++i)
                        {
                            if (logicPaths[i].compare(_T("..")) == 0)
                            {
                                const std::vector<uint32_t>& fileList = pCurLoopDirectory->GetFileList();
                                for (uint32_t i = 0; i < fileList.size(); ++i)
                                {
                                    unloadFiles.push_back(fileList[i]);
                                }
                                pCurLoopDirectory = pCurLoopDirectory->GetParent();
                                BEATS_ASSERT(pCurLoopDirectory != NULL);
                            }
                            else
                            {
                                pCurLoopDirectory = pCurLoopDirectory->FindChild(logicPaths[i].c_str());
                                BEATS_ASSERT(pCurLoopDirectory != NULL);
                                size_t uFileCount = pCurLoopDirectory->GetFileList().size();
                                for (size_t i = 0; i < uFileCount; ++i)
                                {
                                    uint32_t uFileId = pCurLoopDirectory->GetFileList().at(i);
                                    loadFiles.push_back(uFileId);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (bLoadThisFile)
    {
        loadFiles.push_back(uFileId);
    }
}