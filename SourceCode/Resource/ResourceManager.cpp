#include "stdafx.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "Task/TaskManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
#include <Psapi.h>
#elif (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#include "Framework/android/AndroidHandler.h"
#endif
#if (BEYONDENGINE_PLATFORM != PLATFORM_WIN32)
#include "Framework/PlatformHelper.h"
#endif
#define BEYONDENGINE_RESOURCE_HEADER "BeyondEngineRes"
CResourceManager* CResourceManager::m_pInstance = 0;

CResourceManager::CResourceManager()
{
    BEATS_ASSERT(!CApplication::GetInstance()->IsDestructing(), "Should not create singleton when exit the program!");
}

CResourceManager::~CResourceManager()
{
    CleanUp();
#ifdef  SHARE_PTR_TRACE
    for (TResourceNameMap::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
    {
        int iRefCount = iter->second.RefCount();
        if (iRefCount > 1)
        {
            TCHAR szBuffer1[MAX_PATH];
            _stprintf(szBuffer1, _T("File: %s still has %d reference!\nRecyle all resources before CResourceManager destroied!\nReference Pos:\n"), 
                            iter->second->GetFilePath().c_str(), 
                            iter->second.RefCount());
            std::string strRet = iter->second.GetReferencePosString();
            BEATS_ASSERT(false, _T("%s %s"), szBuffer1, strRet.c_str());
        }
    }
#endif
    BEATS_ASSERT(m_resource.size() == 0);
    for (TResourceNameMap::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
    {
        iter->second->Uninitialize();
    }
    // Because the resource can ref each other, to break the ref, I use a loop try to delete those usless one to free others' handle.
    uint32_t uCounter = 0xFFFFFFFF;
    while (uCounter > 0 && m_resource.size() > 0)
    {
        for (auto iter = m_resource.begin(); iter != m_resource.end();)
        {
            if (iter->second.RefCount() == 1)
            {
                iter = m_resource.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        if (uCounter == 0xFFFFFFFF)
        {
            uCounter = m_resource.size();
        }
        if (uCounter > 0)
        {
            --uCounter;
        }
    }
#ifdef _DEBUG
    for (TResourceNameMap::iterator iter = m_resource.begin(); iter != m_resource.end(); )
    {
        if (iter->second.RefCount() == 1)
        {
            iter = m_resource.erase(iter);
        }
        else
        {
            BEATS_ASSERT(false, "Resource:%s\nHas got %d ref when destruct!\n", iter->second->GetFilePath().c_str(), iter->second.RefCount());
#ifdef SHARE_PTR_TRACE
            BEATS_PRINT("RefPos: %s\n", iter->second.GetReferencePosString().c_str());
#endif
            ++iter;
        }
    }
#endif
}

bool CResourceManager::LoadResource(SharePtr<CResource> pResource, bool bAsync )
{
#ifdef EDITOR_MODE
    if (CComponentProxyManager::GetInstance()->IsExporting())
    {
        return false;
    }
#endif
    BEATS_ASSERT(pResource, _T("Need a fresh share pointer for output."));

    bool bRet = false;
    RegisterResource(pResource);
    if (bAsync)
    {
        BEATS_ASSERT(false, _T("Not implemented yet!"));
        // TODO: Finish this.
        //SharePtr<CTaskBase> loadTask = new CLoadComponent(pResource.Get());
        //CTaskManager::GetInstance()->AddTask(loadTask);
    }
    else
    {
#ifdef DEVELOP_VERSION
        uint32_t uStartLoadTime = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
#endif
        bool bLoadSuccess = pResource->Load();
#ifdef DEVELOP_VERSION
        pResource->m_uLoadTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000) - uStartLoadTime;
#endif
        bool bIsMainThread = std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId();
        if (bLoadSuccess)
        {
            BEATS_ASSERT(pResource->IsLoaded(), _T("Resource be created is not loaded right!"));
            if (bIsMainThread)
            {
                pResource->Initialize();
            }
            else
            {
                CTaskManager::GetInstance()->AddDelayInitializeComponent(pResource.Get());
                if (pResource->GetType() == eRT_Texture) // To free the image data in CTexture::Initialize ASAP.
                {
                    CTaskManager::GetInstance()->FlushDelayInitializeComponent();
                }
            }
            bRet = true;
        }
#ifdef DEVELOP_VERSION
        pResource->m_uInitializeTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000) - uStartLoadTime - pResource->m_uLoadTimeMS;
        pResource->m_bDelayInitialize = !bIsMainThread;
        //BEATS_PRINT("File %s loaded!\nLoadTime: %dms InitializeTime:%dms DelayInit:%s\n", pResource->GetFilePath().c_str(), pResource->m_uLoadTimeMS, pResource->m_uInitializeTimeMS, pResource->m_bDelayInitialize ? "true" : "false");
#endif
#ifdef EDITOR_MODE
        WIN32_FILE_ATTRIBUTE_DATA lpinf;
        GetFileAttributesEx(pResource->GetFilePath().c_str(), GetFileExInfoStandard, &lpinf);
        pResource->m_uLastModifyTimeLow = lpinf.ftLastWriteTime.dwLowDateTime;
        pResource->m_uLastModifyTimeHigh = lpinf.ftLastWriteTime.dwHighDateTime;
#endif
    }
    return bRet;
}

bool CResourceManager::LoadResource( SharePtr<CResource> pResource, const TCHAR* resourceName )
{
    TString resourcePath = GetFullPath( resourceName, pResource->GetType());
    pResource->SetFilePath( resourcePath );
    return LoadResource( pResource, false );
}

uint32_t CResourceManager::CleanUp()
{
    uint32_t uCleanedResourceCount = 0;
    BEATS_ASSERT(CApplication::GetInstance()->IsDestructing() || CTaskManager::GetInstance()->IsAllAyncTaskFinished(), "ResourceManager::CleanUp can only be called when no async task left!");
    BEYONDENGINE_CHECK_HEAP;
    bool bIsMainThread = std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId();
    BEATS_ASSERT(bIsMainThread, "you can only call clean up in main thread! or it will cause dangerous thread operation.");
    TResourceNameMap resourceBak = m_resource;
    if (bIsMainThread)
    {
#ifdef _DEBUG
        for (auto iter = resourceBak.begin(); iter != resourceBak.end(); ++iter)
        {
            BEATS_ASSERT(iter->second->IsInitialized());
        }
#endif
        for (auto iter = resourceBak.begin(); iter != resourceBak.end(); ++iter)
        {
            if (iter->second->IsInitialized()) // Some times the resource may be uninitialized by another resource's uninitialize, so we check if current resource is still valid.
            {
                if (iter->second->ShouldClean())
                {
                    BEYONDENGINE_CHECK_HEAP;
                    BEATS_PRINT("Clean up resource:%s\n", iter->second->GetFilePath().c_str());
                    BEATS_ASSERT(iter->second->IsInitialized());
                    iter->second->Uninitialize();
                    UnregisterResource(iter->second);
                    BEYONDENGINE_CHECK_HEAP;
                    ++uCleanedResourceCount;
                }
            }
        }
#ifdef _DEBUG
        for (auto iter = m_resource.begin(); iter != m_resource.end(); ++iter)
        {
            BEATS_ASSERT(iter->second->IsInitialized());
        }
#endif
        BEYONDENGINE_CHECK_HEAP;
    }
    BEATS_PRINT("Call Resource clean up, %d resources are cleaned!\n", uCleanedResourceCount);
    return uCleanedResourceCount;
}

void CResourceManager::RegisterResource(const SharePtr<CResource>& pResource)
{
#ifdef DEVELOP_VERSION
    BEATS_ASSERT(!m_bResourceCritical);
    uTotalFileSize += pResource->m_uFileSize;
#endif
    const TString& strFilePath = pResource->GetFilePath();
    BEATS_ASSERT(m_resource.find(strFilePath) == m_resource.end(), _T("Can't register a resource twice: %s"), strFilePath.c_str());
    m_resource[strFilePath] = pResource;
}

void CResourceManager::UnregisterResource(const SharePtr<CResource>& pResource)
{
#ifdef DEVELOP_VERSION
    BEATS_ASSERT(!m_bResourceCritical);
    BEATS_ASSERT(uTotalFileSize >= pResource->m_uFileSize);
    uTotalFileSize -= pResource->m_uFileSize;
#endif
    const TString& strFilePath = pResource->GetFilePath();
    TResourceNameMap::iterator iter = m_resource.find(strFilePath);
    BEATS_ASSERT( iter != m_resource.end(), _T("Can't Unregister a resource %s"), strFilePath.c_str());
    m_resource.erase(iter);
}

bool CResourceManager::QueryResource(const TString& fileName, SharePtr<CResource>& output) const
{
    TResourceNameMap::const_iterator iter = m_resource.find(fileName);
    bool bRet = iter != m_resource.end();
    if (bRet)
    {
        output = iter->second;
    }
    return bRet;
}

TString CResourceManager::GetFullPath(const TString &strFileName, EResourceType type) const
{
    bool bIsAbsolute = CFilePathTool::GetInstance()->IsAbsolute(strFileName.c_str());
    TString strPath = strFileName;
    if (!bIsAbsolute)
    {
        strPath = CResourceManager::GetInstance()->GetResourcePath(type);
#if (PLATFORM_WIN32 == BEYONDENGINE_PLATFORM)
        if (*strFileName.begin() != '\\' && *strFileName.begin() != '/')
        {
            strPath.append(_T("\\"));
        }
        strPath.append(strFileName);
#else
        if (*strFileName.begin() != '\\' && *strFileName.begin() != '/')
        {
            strPath.append(_T("/"));
        }
        strPath.append(strFileName);
#endif
    }
    
    return strPath;
}

const std::map<TString, SharePtr<CResource>>& CResourceManager::GetResourceMap() const
{
    return m_resource;
}

void CResourceManager::EncodeResourceData(CSerializer& inputData, CSerializer& outputData)
{
    outputData.Serialize(inputData);
}

bool CResourceManager::DecodeResourceData(CSerializer& inputData)
{
    bool bRet = false;
    BEYONDENGINE_UNUSED_PARAM(inputData);
    return bRet;
}

#ifdef DEVELOP_VERSION
void CResourceManager::PrintAllResource() const
{
    std::map<EResourceType, std::vector< SharePtr<CResource> > > resourceTypeMap;
    for (auto iter = m_resource.begin(); iter != m_resource.end(); ++iter)
    {
        resourceTypeMap[iter->second->GetType()].push_back(iter->second);
    }
    for (auto iter = resourceTypeMap.begin(); iter != resourceTypeMap.end(); ++iter)
    {
        BEATS_PRINT(_T("Type: %s x %d\n"), pszResourceTypeString[iter->first], (uint32_t)iter->second.size());
        for (uint32_t i = 0; i < iter->second.size(); ++i)
        {
            BEATS_PRINT(_T("%s\n"), iter->second[i]->GetDescription().c_str());
        }
    }
}
#endif

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined(DEVELOP_VERSION)
#define RESOURCE_REPORT_ORDER_ELAPSED_TIME //Enable this macro to sort the record by time.
void CResourceManager::GenerateResourceReport()
{
    std::map<EResourceType, std::vector< SharePtr<CResource> > > resourceTypeMap;
    for (auto iter = m_resource.begin(); iter != m_resource.end(); ++iter)
    {
        resourceTypeMap[iter->second->GetType()].push_back(iter->second);
    }
    CSerializer serializer;
    serializer << _T("Beyond Engine Resource Report:\n");
#ifdef _DEBUG
    serializer << _T("Mode: Debug\n");
#else
    serializer << _T("Mode: Release:\n");
#endif
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    TCHAR szBuffer[BEATS_PRINT_BUFFER_SIZE];
    _stprintf(szBuffer, _T("Used memory: %d kb Peak memory: %d kb\n"), pmc.WorkingSetSize / 1024, pmc.PeakWorkingSetSize / 1024);
    serializer << szBuffer;
    _stprintf(szBuffer, _T("Resource Count: %d\n{\n"), m_resource.size());
    serializer << szBuffer;
    for (auto iter = resourceTypeMap.begin(); iter != resourceTypeMap.end(); ++iter)
    {
        uint32_t uElapsedTime = 0;
        for (size_t i = 0; i < iter->second.size(); ++i)
        {
            uElapsedTime += iter->second[i]->m_uInitializeTimeMS + iter->second[i]->m_uLoadTimeMS;
        }
        _stprintf(szBuffer, _T("    %s Count: %d ElapsedTime: %dms\n"), pszResourceTypeString[iter->first], iter->second.size(), uElapsedTime);
        serializer << szBuffer;
        std::vector< SharePtr<CResource> > dataVec = iter->second;
#ifdef RESOURCE_REPORT_ORDER_ELAPSED_TIME
        std::sort(dataVec.begin(), dataVec.end(), [](SharePtr<CResource> i, SharePtr<CResource> j){
                return (i->m_uLoadTimeMS + i->m_uInitializeTimeMS > j->m_uLoadTimeMS + j->m_uInitializeTimeMS);
        });
#endif
        for (size_t i = 0; i < dataVec.size(); ++i)
        {
            serializer << dataVec[i]->GetDescription() << _T("\n");
        }
    }
    serializer << _T("}\n") << 0;
    _strtime_s(szBuffer, BEATS_PRINT_BUFFER_SIZE);
    std::vector<TString> timeResult;
    CStringHelper::GetInstance()->SplitString(szBuffer, _T(":"), timeResult);
    BEATS_ASSERT(timeResult.size() == 3);
    _strdate_s(szBuffer, BEATS_PRINT_BUFFER_SIZE);
    std::vector<TString> dateResult;
    CStringHelper::GetInstance()->SplitString(szBuffer, _T("/"), dateResult);
    BEATS_ASSERT(dateResult.size() == 3);
    TString strTimeStr = _T("C:/ResourceReport");
    strTimeStr.append(_T("_")).append(dateResult[2]);
    strTimeStr.append(_T("_")).append(dateResult[0]);
    strTimeStr.append(_T("_")).append(dateResult[1]);
    for (size_t i = 0; i < timeResult.size(); ++i)
    {
        strTimeStr.append(_T("_")).append(timeResult[i]);
    }
    strTimeStr.append(_T(".txt"));
    serializer.Deserialize(strTimeStr.c_str());
}
#endif

#ifdef EDITOR_MODE
void CResourceManager::Reload()
{
    for (auto iter = m_resource.begin(); iter != m_resource.end(); ++iter)
    {
        if (iter->second->NeedReload())
        {
            iter->second->Reload();
        }
    }
}
#endif

TString CResourceManager::GetResourcePath(EResourceType type) const
{
    TString strModulePath = CFilePathTool::GetInstance()->FileFullPath(pszResourcePathName[type]);
#if (PLATFORM_ANDROID == BEYONDENGINE_PLATFORM || PLATFORM_IOS == BEYONDENGINE_PLATFORM)
    // Always load font from the extracted directory, since free type can't load file from the assertmanager.
    // We have extracted the font files in LoadingScene::OnEnter()
    bool bGetExtractPath = CPlatformHelper::GetInstance()->IsAssetFileExtracted();
#if (PLATFORM_ANDROID == BEYONDENGINE_PLATFORM)
    bGetExtractPath = bGetExtractPath || type == eRT_Font;
#endif
    if (bGetExtractPath)
    {
        strModulePath = GetPersistentDataPath();
        strModulePath.append(_T("/")).append(pszResourcePathName[type]);
    }
#endif
    return strModulePath;
}

TString CResourceManager::GetPersistentDataPath() const
{
    TString strRet;
#if (PLATFORM_ANDROID == BEYONDENGINE_PLATFORM)
    strRet = CAndroidHandler::GetInstance()->GetAndroidPath(eAPT_FilesDirectory);
#elif (PLATFORM_WIN32 == BEYONDENGINE_PLATFORM)
    TCHAR curWorkingPath[MAX_PATH];
    ::GetModuleFileName(NULL, curWorkingPath, MAX_PATH);
    strRet = CFilePathTool::GetInstance()->ParentPath(curWorkingPath);
    strRet += _T("\\..");
    strRet = CFilePathTool::GetInstance()->ConvertToUnixPath(strRet.c_str());
#elif (PLATFORM_IOS == BEYONDENGINE_PLATFORM)
    strRet = CPlatformHelper::GetInstance()->GetPersistentDataPath();
#endif
    return strRet;
}
