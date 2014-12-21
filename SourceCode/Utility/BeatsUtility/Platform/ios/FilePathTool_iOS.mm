#include "stdafx.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#import <Foundation/Foundation.h>
#include "Utility/BeatsUtility/StringHelper.h"
#include "Framework/PlatformHelper.h"

bool CFilePathTool::Exists(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath && pszPath[0]);
    TString unixPath = ConvertToUnixPath(pszPath);
    pszPath = unixPath.c_str();
    return [[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:pszPath]];
}

bool CFilePathTool::IsAbsolute(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath && pszPath[0]);
    return pszPath[0] == _T('/');
}

bool CFilePathTool::IsDirectory(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath && pszPath[0]);
    BOOL bIsDir;
    BOOL bExists = [[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:pszPath] isDirectory:&bIsDir];
    return bExists && bIsDir;
}

TString CFilePathTool::ParentPath(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath && pszPath[0]);
    TString strPath(pszPath);
    int pos = strPath.rfind(_T('/'));
    if (pos == 0)
    {
        BEATS_ASSERT(strPath.back() == _T('/'));
        strPath.pop_back();
        pos = strPath.rfind(_T('/'));
    }    if (pos != TString::npos)
    {
        strPath.resize(pos);
    }
    return strPath;
}

TString CFilePathTool::Extension(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath && pszPath[0]);
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
    BEATS_ASSERT(pszPath && pszPath[0]);
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
    BEATS_ASSERT(false, _T("MakeRelative not implemented!"));
    return _T("");
}

TString CFilePathTool::MakeAbsolute(const TCHAR* pszStartPath, const TCHAR* pszRelativePath)
{
    // TODO: Unimplement
    BEATS_ASSERT(false, _T("MakeAbsolute not implemented!"));
    return _T("");
}

TString CFilePathTool::RootPath( const TCHAR* pszPath )
{
    // TODO: Unimplement
    BEATS_ASSERT(false, _T("RootPath not implemented!"));
    return _T("");
}

bool CFilePathTool::Canonical(TCHAR* pszOutBuffer, const TCHAR* pszOriginPath)
{
    strcpy(pszOutBuffer, pszOriginPath);
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
        strRet = CPlatformHelper::GetInstance()->GetResourceRootPath();
        strRet.append("/").append(pszFilePath);
    }
    return strRet;
}