#ifndef BEYOND_ENGINE_RESOURCE_RESOURCEMANAGER_H__INCLUDE
#define BEYOND_ENGINE_RESOURCE_RESOURCEMANAGER_H__INCLUDE

#include "Resource.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "ResourcePathManager.h"

class CResourceManager
{
    BEATS_DECLARE_SINGLETON(CResourceManager);
public:
    template <typename T>
    SharePtr<T> GetResource(const TString &filename, bool bAsync = false );
    bool QueryResource(const TString& fileName, SharePtr<CResource>& output) const;
    bool CleanUp();

    // Don't use it out of CResource.
    void RegisterResource(const SharePtr<CResource>& pResource);
    void UnregisterResource(const SharePtr<CResource>& pResource);
    bool LoadResource( SharePtr<CResource> pResource, const TCHAR* resourceName );
    bool LoadResource(SharePtr<CResource> pResource, bool bAsync);
    TString GetFullPath(const TString &strFileName, EResourceType type) const;

private:
    bool m_bQuit;
    bool m_bInitialized;
    typedef std::map<TString, SharePtr<CResource>> TResourceNameMap;
    TResourceNameMap m_resource;
};

template <typename T>
SharePtr<T> CResourceManager::GetResource( const TString &strFilePath, bool bAsync )
{
    SharePtr<T> pRet;
    TString strFullPath = GetFullPath(strFilePath, T::RESOURCE_TYPE);
    TResourceNameMap::iterator iter = m_resource.find(strFullPath);
    if(iter == m_resource.end())
    {
        pRet = new T;
        pRet->SetFilePath(strFullPath);
        LoadResource(pRet, bAsync );
    }
    else
    {
        pRet = iter->second;
    }
    return pRet;
}

#endif