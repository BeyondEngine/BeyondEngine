#ifndef BEYOND_ENGINE_RESOURCE_RESOURCEMANAGER_H__INCLUDE
#define BEYOND_ENGINE_RESOURCE_RESOURCEMANAGER_H__INCLUDE

#include "ResourcePublic.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Utility/BeatsUtility/SharePtr.h"
class CResource;
class CResourceManager
{
    BEATS_DECLARE_SINGLETON(CResourceManager);
public:
    template <typename T>
    SharePtr<T> GetResource(const TString &filename, bool bAsync = false );
    bool QueryResource(const TString& fileName, SharePtr<CResource>& output) const;
    uint32_t CleanUp();

    // Don't use it out of CResource.
    void RegisterResource(const SharePtr<CResource>& pResource);
    void UnregisterResource(const SharePtr<CResource>& pResource);
    bool LoadResource( SharePtr<CResource> pResource, const TCHAR* resourceName );
    bool LoadResource(SharePtr<CResource> pResource, bool bAsync);
    TString GetFullPath(const TString &strFileName, EResourceType type) const;
    const std::map<TString, SharePtr<CResource>>& GetResourceMap() const;
    void EncodeResourceData(CSerializer& inputData, CSerializer& outputData);
    bool DecodeResourceData(CSerializer& inputData);
    TString GetPersistentDataPath() const;
    TString GetResourcePath(EResourceType type) const;
#ifdef DEVELOP_VERSION
    void PrintAllResource() const;
    uint32_t uTotalFileSize = 0;
    bool m_bResourceCritical = false;
#endif
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined(DEVELOP_VERSION)
    void GenerateResourceReport();
#endif
#ifdef EDITOR_MODE
    void Reload();
#endif
    static const uint32_t MIN_RESOURCE_REF_COUNT = 3;
private:
    typedef std::map<TString, SharePtr<CResource>> TResourceNameMap;
    TResourceNameMap m_resource;
};

#ifdef DISABLE_RENDER
#include "Render/Texture.h"
#endif

template <typename T>
SharePtr<T> CResourceManager::GetResource( const TString &strFilePath, bool bAsync )
{
    SharePtr<T> pRet;
    TString strFullPath = strFilePath;
#ifdef DISABLE_RENDER
    if (T::REFLECT_GUID == CTexture::REFLECT_GUID)
    {
        strFullPath = "name.png";
    }
#endif
    bool bIsAbsolute = CFilePathTool::GetInstance()->IsAbsolute(strFullPath.c_str());
#ifndef EDITOR_MODE
    BEATS_ASSERT(!bIsAbsolute, "we should not pass absolute path to resource manager!");
#endif // EDITOR_MODE

    if (!bIsAbsolute)
    {
        strFullPath = GetFullPath(CStringHelper::GetInstance()->ToLower(strFullPath), T::RESOURCE_TYPE);
    }
#if (BEYONDENGINE_PLATFORM != PLATFORM_WIN32)
    BEATS_ASSERT(strFullPath.find('\\') == TString::npos, "file path: %s contains \"\\\"!");
#endif
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