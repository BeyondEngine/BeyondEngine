#ifndef BEATS_UTILITY_FILEPATH_FILEPATHTOOL_H__INCLUDE
#define BEATS_UTILITY_FILEPATH_FILEPATHTOOL_H__INCLUDE

#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    #define ASSET_ROOT_PATH _T("assets/")
#endif
class CSerializer;
class CFilePathTool
{
    BEATS_DECLARE_SINGLETON(CFilePathTool)
public:
    bool Exists(const TCHAR* pszPath);
    bool IsAbsolute(const TCHAR* pszPath);
    bool IsDirectory(const TCHAR* pszPath);
    TString ParentPath(const TCHAR* pszPath);
    TString Extension(const TCHAR* pszPath);
    TString FileName(const TCHAR* pszPath);
    TString MakeRelative(const TCHAR* pszFrom, const TCHAR* pszTo);
    TString MakeAbsolute(const TCHAR* pszStartPath, const TCHAR* pszRelativePath);
    TString RootPath(const TCHAR* pszPath);
    bool Canonical(TCHAR* pszOutBuffer, const TCHAR* pszOriginPath);
    bool LoadFile(CSerializer* pSerializer, const TCHAR* pszFilePath, const TCHAR* pszMode, uint32_t uStartPos = 0, uint32_t uDataLength = 0);
    TString FileFullPath(const TCHAR* pszFilePath);
    TString ConvertToUnixPath(const TCHAR* pszFilePath) const;
    TString ConvertToWindowsPath(const TCHAR* pszFilePath) const;
    bool MakeDirectory(const TCHAR* pszDirectoryPath) const;

#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    void SetAssetManager(class AAssetManager* pMgr);
    class AAssetManager* GetAssetManager() const;
private:
    class AAssetManager* m_pAssetManager;
#endif
};
#endif