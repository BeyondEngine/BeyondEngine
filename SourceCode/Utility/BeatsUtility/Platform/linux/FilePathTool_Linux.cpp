#include "stdafx.h"
#include "Utility/BeatsUtility/FilePathTool.h"


bool CFilePathTool::Exists(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    FILE *pFile = _tfopen(pszPath, _T("r"));
    if(pFile)
    {
        fclose(pFile);
        return true;
    }
    return false;
}

bool CFilePathTool::IsAbsolute(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0, "File path can't be empty in CFilePathTool::IsAbsolute");
    // On Android, there are two situations for full path.
    // 1) Files in APK, e.g. assets/path/path/file.png
    // 2) Files not in APK, e.g. /data/data/org.cocos2dx.hellocpp/cache/path/path/file.png, or /sdcard/path/path/file.png.
    // So these two situations need to be checked on Android.
    TString strPath(pszPath);
    bool bRet = strPath[0] == _T('/') || strPath.find("/") == 0;
    return bRet;
}

bool CFilePathTool::IsDirectory(const TCHAR* pszPath)
{
    // TODO: Unimplement, maybe we need a JNI call.
    return false;
}

TString CFilePathTool::ParentPath(const TCHAR* pszPath)
{
    TString strPath(pszPath);
    int pos = strPath.rfind(_T('/'));
    if (pos == 0)
    {
        BEATS_ASSERT(strPath.back() == _T('/'));
        strPath.pop_back();
        pos = strPath.rfind(_T('/'));
    }
    if (pos != TString::npos)
    {
        strPath.resize(pos);
    }
    return strPath;
}

TString CFilePathTool::Extension(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    TString strPath(pszPath);
    int pos = strPath.rfind(_T('.'));
    if (pos != TString::npos)
    {
        strPath = strPath.substr(pos, strPath.length() - pos);
    }
    else
    {
        strPath.clear();
    }
    return strPath;
}

TString CFilePathTool::FileName(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    TString strPath(pszPath);
    int pos = strPath.rfind(_T('/'));
    if (pos != TString::npos)
    {
        strPath = strPath.substr(pos + 1, strPath.length() - pos);
    }
    return strPath;
}

TString CFilePathTool::MakeRelative(const TCHAR* pszFrom, const TCHAR* pszTo)
{
    // TODO: Unimplement
    BEATS_WARNING(false, _T("Call unimplement function %s"), __FUNCTION__);
    return _T("");
}

TString CFilePathTool::MakeAbsolute(const TCHAR* pszStartPath, const TCHAR* pszRelativePath)
{
    // TODO: Unimplement
    BEATS_WARNING(false, _T("Call unimplement function %s"), __FUNCTION__);
    return _T("");
}

TString CFilePathTool::RootPath( const TCHAR* pszPath )
{
    // TODO: Unimplement
    BEATS_WARNING(false, _T("Call unimplement function %s"), __FUNCTION__);
    return "/";
}

bool CFilePathTool::Canonical(TCHAR* pszOutBuffer, const TCHAR* pszOriginPath)
{
    // TODO: Unimplement
    _tcscpy(pszOutBuffer, pszOriginPath);
    BEATS_WARNING(false, _T("Call unimplement function %s"), __FUNCTION__);
    return false;
}

TString CFilePathTool::FileFullPath(const TCHAR* pszFilePath)
{
    TString strRet;
    if (IsAbsolute(pszFilePath))
    {
        strRet.assign(pszFilePath);
    }
    else
    {
        char current_absolute_path[MAX_PATH] = {};
        int cnt = readlink("/proc/self/exe", current_absolute_path, MAX_PATH);
        BEATS_ASSERT(cnt > 0 && cnt < MAX_PATH);
        BEATS_ASSERT(Exists(current_absolute_path));
        strRet = ParentPath(current_absolute_path);
        strRet.append("/").append(pszFilePath);
    }
    return strRet;
}
