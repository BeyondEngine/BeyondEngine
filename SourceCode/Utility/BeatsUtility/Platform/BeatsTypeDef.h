#ifndef BEATS_BEATSTYPEDEF_H__INCLUDE
#define BEATS_BEATSTYPEDEF_H__INCLUDE

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
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
    DWORD dwFileAttributes = 0;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh = 0;
    DWORD nFileSizeLow = 0;
    DWORD dwReserved0 = 0;
    DWORD dwReserved1 = 0;
    TCHAR  cFileName[ MAX_PATH ];
    TCHAR  cAlternateFileName[ 14 ];
#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    DWORD dwFileType = 0;
    DWORD dwCreatorType = 0;
    WORD  wFinderFlags = 0;
#endif
};
typedef SFileData TFileData;

/// MACRO DEF
#define MessageBox(hWnd, lpText, lpCaption, uType) 0
#define IDYES 1
#define IDNO 0

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

#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
#define Beats_AtomicIncrement(pVariable) OSAtomicIncrement32((int*)pVariable)
#define Beats_AtomicDecrement(pVariable) OSAtomicDecrement32((int*)pVariable)
#endif
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#define Beats_AtomicIncrement(pVariable) __sync_fetch_and_add((int*)pVariable, 1)
#define Beats_AtomicDecrement(pVariable) __sync_fetch_and_sub((int*)pVariable, 1)
#endif
#if (BEYONDENGINE_PLATFORM == PLATFORM_LINUX)
#define Beats_AtomicIncrement(pVariable) __sync_fetch_and_add((int*)pVariable, 1)
#define Beats_AtomicDecrement(pVariable) __sync_fetch_and_sub((int*)pVariable, 1)
#endif

typedef struct tagBITMAPINFOHEADER{
    uint32_t    biSize;
    uint32_t    biWidth;
    uint32_t    biHeight;
    uint16_t    biPlanes;
    uint16_t    biBitCount;
    uint32_t    biCompression;
    uint32_t    biSizeImage;
    uint32_t    biXPelsPerMeter;
    uint32_t    biYPelsPerMeter;
    uint32_t    biClrUsed;
    uint32_t    biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
    uint16_t    bfType;
    uint32_t    bfSize;
    uint16_t    bfReserved1;
    uint16_t    bfReserved2;
    uint32_t    bfOffBits;
} BITMAPFILEHEADER;

#define BI_RGB        0L

#endif

template <typename DerivedType, typename BaseType>
DerivedType down_cast(BaseType base)
{
#ifdef _DEBUG
    DerivedType derived = nullptr;
    if (base != nullptr)
    {
        derived = dynamic_cast<DerivedType>(base);
        BEATS_ASSERT(derived != nullptr, _T("down_cast failed!"));
    }
    return derived;
#else
    return static_cast<DerivedType>(base);
#endif
}

#endif // !BEATS_BEATSTYPEDEF_H__INCLUDE
