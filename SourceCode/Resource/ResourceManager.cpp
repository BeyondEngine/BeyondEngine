#include "stdafx.h"
#include "ResourceManager.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Task/TaskManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

CResourceManager* CResourceManager::m_pInstance = 0;

CResourceManager::CResourceManager()
    : m_bQuit(false)
    , m_bInitialized(false)
{

}

CResourceManager::~CResourceManager()
{
    m_bQuit = true;

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
            std::string strRet;
            iter->second.GetReferencePosString(strRet);
            TCHAR szBuffer2[10240];
            CStringHelper::GetInstance()->ConvertToTCHAR(strRet.c_str(), szBuffer2, 10240);
            BEATS_ASSERT(false, _T("%s %s"), szBuffer1, szBuffer2);
        }
    }
#endif

    for (TResourceNameMap::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
    {
        iter->second->Uninitialize();
    }
}

bool CResourceManager::LoadResource(SharePtr<CResource> pResource, bool bAsync )
{
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
        pResource->Load();
        BEATS_ASSERT(pResource->IsLoaded(), _T("Resource be created is not loaded right!"));
        if (std::this_thread::get_id() == CTaskManager::GetInstance()->GetTaskThreadId())
        {
            CTaskManager::GetInstance()->AddDelayInitializeComponent(pResource.Get());
        }
        else
        {
            pResource->Initialize();
            BEATS_ASSERT(pResource->IsInitialized(), _T("Resource be created is not initialized right!"));
        }
        bRet = true;
    }

    return bRet;
}

bool CResourceManager::LoadResource( SharePtr<CResource> pResource, const TCHAR* resourceName )
{
    TString resourcePath = GetFullPath( resourceName ,pResource->GetType());
    pResource->SetFilePath( resourcePath );
    return LoadResource( pResource, false );
}

bool CResourceManager::CleanUp()
{
    //TODO: clean those resource are not referenced.
    return true;
}

void CResourceManager::RegisterResource(const SharePtr<CResource>& pResource)
{
    const TString& strFilePath = pResource->GetFilePath();
    BEATS_ASSERT(m_resource.find(strFilePath) == m_resource.end(), _T("Can't register a resource twice: %s"), strFilePath.c_str());
    m_resource[strFilePath] = pResource;
}

void CResourceManager::UnregisterResource(const SharePtr<CResource>& pResource)
{
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
        CResourcePathManager::EResourcePathType pathType = CResourcePathManager::GetInstance()->GetResourcePathType(type);
        strPath = CResourcePathManager::GetInstance()->GetResourcePath(pathType);
        strPath.append(_T("/")).append(strFileName);
    }
    return strPath;
}