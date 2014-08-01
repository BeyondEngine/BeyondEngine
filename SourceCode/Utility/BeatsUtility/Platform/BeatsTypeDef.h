#ifndef BEATS_BEATSTYPEDEF_H__INCLUDE
#define BEATS_BEATSTYPEDEF_H__INCLUDE

#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
typedef WIN32_FIND_DATA TFileData;
#define Beats_AtomicIncrement(pVariable) ::InterlockedIncrement(pVariable)
#define Beats_AtomicDecrement(pVariable) ::InterlockedDecrement(pVariable)
#else

/// TYPE DEF
#define MAX_PATH 256
typedef unsigned int HANDLE;
typedef unsigned int HWND;
typedef long long FILETIME;
typedef int DWORD;
typedef short WORD;

struct SFileData
{
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    TCHAR  cFileName[ MAX_PATH ];
    TCHAR  cAlternateFileName[ 14 ];
#if (BEATS_PLATFORM == BEATS_PLATFORM_IOS)
    DWORD dwFileType;
    DWORD dwCreatorType;
    WORD  wFinderFlags;
#endif
};
typedef SFileData TFileData;

/// MACRO DEF
#define MessageBox(hWnd, lpText, lpCaption, uType) 0
#define IDYES 1
#define IDNO 0

#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
/// FUNCTION DEF
inline FILE* _tfopen(const TCHAR* pszFileName, const TCHAR* pszMode)
{
#ifdef _UNICODE
    char szFileNameBuffer[MAX_PATH];
    sprintf(szFileNameBuffer, "%ls", pszFileName);
    char szModeBuffer[16];
    sprintf(szModeBuffer, "%ls", pszMode);
    return fopen(szFileNameBuffer, szModeBuffer);
#else
    return fopen(pszFileName, pszMode);
#endif
}
#endif
#if (BEATS_PLATFORM == BEATS_PLATFORM_IOS)
#define Beats_AtomicIncrement(pVariable) OSAtomicIncrement32((int*)pVariable)
#define Beats_AtomicDecrement(pVariable) OSAtomicDecrement32((int*)pVariable)
#endif
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
#define Beats_AtomicIncrement(pVariable) __sync_fetch_and_add((int*)pVariable, 1)
#define Beats_AtomicDecrement(pVariable) __sync_fetch_and_sub((int*)pVariable, 1)
#endif
#endif

template <typename DerivedType, typename BaseType>
DerivedType down_cast(BaseType base)
{
#ifdef _DEBUG
    DerivedType derived = dynamic_cast<DerivedType>(base);
    BEATS_ASSERT(derived || !base, _T("down_cast failed!"));
    return derived;
#else
    return static_cast<DerivedType>(base);
#endif
}

#endif // !BEATS_BEATSTYPEDEF_H__INCLUDE
