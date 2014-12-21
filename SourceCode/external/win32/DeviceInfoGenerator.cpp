#include "stdafx.h"
#include "DeviceInfoGenerator.h"

BOOL CDeviceInfoGenerator::isWinVersionEqualTo(DWORD dwMajorVersion, DWORD dwMinorVersion)
{
    OSVERSIONINFOEX osvi;
    DWORDLONG dwlConditionMask = 0;

    //Initialize the OSVERSIONINFOEX structure.  
    ZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMajorVersion = dwMajorVersion;
    osvi.dwMinorVersion = dwMinorVersion;

    //Initialize the condition mask.  
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);

    return VerifyVersionInfo(
        &osvi,
        VER_MAJORVERSION | VER_MINORVERSION,
        dwlConditionMask
        );
}

BOOL CDeviceInfoGenerator::isWinVersionGreaterThan(DWORD dwMajorVersion, DWORD dwMinorVersion)
{
    OSVERSIONINFOEX osvi;
    DWORDLONG dwlConditionMask = 0;

    //Initialize the OSVERSIONINFOEX structure.  
    ZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMajorVersion = dwMajorVersion;
    osvi.dwMinorVersion = dwMinorVersion;

    //system major version > dwMajorVersion  
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER);
    if (VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask))
        return TRUE;

    //sytem major version = dwMajorVersion && minor version > dwMinorVersion  
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER);

    return VerifyVersionInfo(
        &osvi,
        VER_MAJORVERSION | VER_MINORVERSION,
        dwlConditionMask
        );
}

BOOL CDeviceInfoGenerator::isWinVersionLessThan(DWORD dwMajorVersion, DWORD dwMinorVersion)
{
    OSVERSIONINFOEX osvi;
    DWORDLONG dwlConditionMask = 0;

    //Initialize the OSVERSIONINFOEX structure.  
    ZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMajorVersion = dwMajorVersion;
    osvi.dwMinorVersion = dwMinorVersion;

    //system major version < dwMajorVersion  
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_LESS);
    if (VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask))
        return TRUE;

    //sytem major version = dwMajorVersion && minor version < dwMinorVersion  
    dwlConditionMask = 0;
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_LESS);

    return VerifyVersionInfo(
        &osvi,
        VER_MAJORVERSION | VER_MINORVERSION,
        dwlConditionMask
        );
}

TString CDeviceInfoGenerator::GenerateOsVersion()
{
    TString ret;
    /* Ancient Windows */
    if (isWinVersionLessThan(5, 1)) {
        ret = _T("Ancient Windows");
    }
    /* Windows XP */
    else if (isWinVersionEqualTo(5, 1)) {
        ret = _T("Windows_XP");
    }
    /* Windows Server 2003 */
    else if (isWinVersionEqualTo(5, 2)) {
        ret = _T("Windows_Sever_2003");
    }
    /* Windows Vista, Windows Server 2008 */
    else if (isWinVersionEqualTo(6, 0)) {
        ret = _T("Windows_Vista Windows_Sever_2008");
    }
    /* Windows 7, Windows Server 2008 R2 */
    else if (isWinVersionEqualTo(6, 1)) {
        ret = _T("Windows_7");
    }
    /* Windows 8 */
    else if (isWinVersionEqualTo(6, 2)) {
        ret = _T("Windows_8");
    }
    else if (isWinVersionGreaterThan(6, 2)) {
        ret = _T("Ancient Windows");
    }
    return ret;
}

TString CDeviceInfoGenerator::GenerateModelInfo()
{
    TString ret;
    ret = _T("PC");
    return ret;
}
