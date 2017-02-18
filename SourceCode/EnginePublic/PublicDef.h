#ifndef BEYOND_ENGINE_PUBLICDEF_H__INCLUDE
#define BEYOND_ENGINE_PUBLICDEF_H__INCLUDE

//STL headers
#include <stdint.h>
#include <assert.h>
#include <string>

#include <list>
#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#include <algorithm>
#include <memory>
#include <sstream>
#include <iostream>
#include <fstream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "Utility/BeatsUtility/Platform/BeatsTchar.h"

#define BEYONDENGINE_UNUSED_PARAM(unusedparam) (void)unusedparam

#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#include <android/log.h>
#elif (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
extern HWND BEYONDENGINE_HWND;
#elif (BEYONDENGINE_PLATFORM == PLATFORM_LINUX)
#include <sys/time.h>
#elif (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
#include <malloc/malloc.h>
#endif
#pragma warning(disable:4127)
//////////////////////////////////////////////////////////////////////////
//Configuration
#define DEVELOP_VERSION
#define BEYOND_ENGINE_PUBLISH_VERSION
#ifdef EDITOR_MODE
#define DISABLE_NETWORK
#define DEVELOP_VERSION
#else
//#define USE_VLD
//#define CHECK_HEAP
//#define DISABLE_MULTI_THREAD
//#define MEMORY_CAPTURE
//#define DISABLE_NETWORK
//#define DISABLE_UI
//#define DISABLE_NODE_ANIMATION
#endif

#include "Utility/Memory/Memory.h"

#ifdef EDITOR_MODE
#define ASSUME_VARIABLE_IN_EDITOR_BEGIN(var, ...) \
BEATS_ASSERT(var, ##__VA_ARGS__);\
if(var){
#else
#define ASSUME_VARIABLE_IN_EDITOR_BEGIN(var, ...) BEATS_ASSERT(var, ##__VA_ARGS__);{
#endif

#define ASSUME_VARIABLE_IN_EDITOR_END }

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
#define TO_UTF8(str) CStringHelper::GetInstance()->StringToUtf8(str)
#else
#define TO_UTF8(str)
#endif
//////////////////////////////////////////////////////////////////////////
//Assert
//To call messagebox so we can afford info for the engine user.
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    #ifdef _UNICODE
        #define MessageBox MessageBoxW
    #else
        #define MessageBox MessageBoxA
    #endif
#endif
#ifndef BEATS_LOG
#define BEATS_LOG(type, logString, ...)
#endif
static const uint16_t BEATS_PRINT_BUFFER_SIZE = 2048;
#if defined(DEVELOP_VERSION) || defined(_DEBUG)
    #if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    #if defined(_DEBUG)
        #define BEATS_ASSERT_IMPL\
        int iRet = MessageBox(BEYONDENGINE_HWND, szInfoBuffer, _T("Beats assert"), MB_ABORTRETRYIGNORE | MB_ICONERROR); \
        if (iRet == IDABORT)\
        {\
            __debugbreak(); \
        }\
        else if (iRet == IDIGNORE)\
        {\
            bIgnoreThisAssert = true; \
        }
    #else
        #define BEATS_ASSERT_IMPL bIgnoreThisAssert = true;
    #endif
    #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            static bool bIgnoreThisAssert = false;\
            if (!bIgnoreThisAssert)\
            {\
                TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = { 0 }; \
                _stprintf(szInfoBuffer, ##__VA_ARGS__, ""); \
                TCHAR szLocationBuffer[512] = { 0 }; \
                _stprintf(szLocationBuffer, _T("\nErrorID:%d at %s %d %s\nCondition: %s\n"), GetLastError(), _T(__FILE__), __LINE__, __FUNCTION__, _T(#condition)); \
                _tcscat_s(szInfoBuffer, BEATS_PRINT_BUFFER_SIZE, szLocationBuffer); \
                OutputDebugString(szInfoBuffer); \
                BEATS_LOG(ELogType::eLT_Error, szInfoBuffer, 0, nullptr); \
                BEATS_ASSERT_IMPL\
            }\
        }
    #elif(BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
        #if defined(_DEBUG)
            #define BEATS_ASSERT_IMPL(title, data) __android_log_assert(title, "BeyondEngine", "%s", data);
        #else
            #define BEATS_ASSERT_IMPL(title, data) __android_log_print(ANDROID_LOG_ERROR, _T("BeyondEngine"), _T("%s"), szInfoBuffer);
        #endif
        #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = {0}; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, "");\
            TCHAR szLocationBuffer[512] = {0};\
            _stprintf(szLocationBuffer, _T("\nat %s %d %s\nCondition: %s\n"), __FILE__, __LINE__, __FUNCTION__, _T(#condition)); \
            _tcscat(szInfoBuffer, szLocationBuffer);\
            BEATS_LOG(ELogType::eLT_Error, szInfoBuffer, 0, nullptr); \
            BEATS_ASSERT_IMPL(#condition, szInfoBuffer);\
        }
    #elif(BEYONDENGINE_PLATFORM == PLATFORM_IOS)
        #if defined(_DEBUG)
        #define BEATS_ASSERT_IMPL(data) assert(false && data)
        #else
        #define BEATS_ASSERT_IMPL(data)
        #endif
        #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = {0}; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, "");\
            TCHAR szLocationBuffer[512] = {0};\
            _stprintf(szLocationBuffer, _T("\nat %s %d %s\nCondition: %s\n"), __FILE__, __LINE__, __FUNCTION__, _T(#condition)); \
            _tcscat(szInfoBuffer, szLocationBuffer);\
            _tprintf(szInfoBuffer);\
            BEATS_LOG(ELogType::eLT_Error, szInfoBuffer, 0, nullptr); \
            BEATS_ASSERT_IMPL(#condition);\
        }
    #elif(BEYONDENGINE_PLATFORM == PLATFORM_LINUX)
        #if defined(_DEBUG)
        #define BEATS_ASSERT_IMPL(data) assert(false && data)
        #else
        #define BEATS_ASSERT_IMPL(data)
        #endif
        #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = {0}; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, "");\
            TCHAR szLocationBuffer[512] = {0};\
            _stprintf(szLocationBuffer, _T("\nat %s %d %s\nCondition: %s\n"), __FILE__, __LINE__, __FUNCTION__, _T(#condition)); \
            _tcscat(szInfoBuffer, szLocationBuffer);\
            _tprintf("%s", szInfoBuffer);\
            BEATS_ASSERT_IMPL(#condition);\
        }
    #else
        #define BEATS_ASSERT(condition, ...) {assert(condition);}
    #endif
#else
    #define BEATS_ASSERT(condition, ...)
#endif

//////////////////////////////////////////////////////////////////////////
//Warning
#if defined(DEVELOP_VERSION) || defined(_DEBUG)
#define BEATS_WARNING(condition, ...)\
    if (!(condition))\
    {\
        BEATS_PRINT(__VA_ARGS__);\
        BEATS_PRINT(_T("    At line :%d in file: %s\n"), __LINE__, _T(__FILE__));\
    }
#else
#define BEATS_WARNING(condition, ...)
#endif

//////////////////////////////////////////////////////////////////////////
//DEVELOP_PRINT
#if defined(DEVELOP_VERSION) || defined(_DEBUG)
    #if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
        #define BEATS_PRINT_IMPL(...)\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = { 0 }; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, _T("")); \
            OutputDebugString(szInfoBuffer);\
            printf(szInfoBuffer);

    #elif (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
        #define BEATS_PRINT_IMPL(...)\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = {0}; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, _T(""));\
            __android_log_print(ANDROID_LOG_INFO,_T("BeyondEngine"), _T("%s"), szInfoBuffer);
    #elif (BEYONDENGINE_PLATFORM == PLATFORM_LINUX)
        #define BEATS_PRINT_IMPL(...)\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = {0}; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__);\
            struct timeval tv;\
            gettimeofday(&tv, NULL);\
            struct tm* pTimeInfo = localtime(&tv.tv_sec);\
            TCHAR timeBuffer[128] = {0}; \
            _stprintf(timeBuffer, "%d-%d-%d %d:%d:%d.%d", 1900+pTimeInfo->tm_year, pTimeInfo->tm_mon+1, pTimeInfo->tm_mday, pTimeInfo->tm_hour, pTimeInfo->tm_min, pTimeInfo->tm_sec, int32_t(tv.tv_usec/1000));\
            _tprintf("[%s] %s", timeBuffer, szInfoBuffer);
    #else
        #define BEATS_PRINT_IMPL(...)\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE] = {0}; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, _T(""));\
            _tprintf(szInfoBuffer);
    #endif
    #define BEATS_PRINT(...)\
    {\
        BEATS_PRINT_IMPL(__VA_ARGS__);\
        BEATS_LOG(ELogType::eLT_Info, szInfoBuffer, 0, nullptr); \
    }

    #define BEATS_PRINT_EX(catalog, color, ...)\
    {\
        BEATS_PRINT_IMPL(__VA_ARGS__); \
        BEATS_LOG(ELogType::eLT_Info, szInfoBuffer, color, catalog); \
    }
#else
    #define BEATS_PRINT(...)
    #define BEATS_PRINT_EX(...)
#endif


//////////////////////////////////////////////////////////////////////////
// SingletonDef
#ifdef DEVELOP_VERSION
#define REGISTER_SINGLETON_NAME(className) g_registeredSingleton.push_back(className)
extern std::vector<TString> g_registeredSingleton;
#else
#define REGISTER_SINGLETON_NAME(className)
#endif
#define BEATS_DECLARE_SINGLETON(className)\
public:\
    struct IsSingletonTag{};\
private:\
    className();\
    ~className();\
    static className* m_pInstance;\
public:\
    static className* GetInstance()\
    {\
        if (m_pInstance == NULL)\
        {\
            REGISTER_SINGLETON_NAME(#className);\
            m_pInstance = new className; \
        }\
        return m_pInstance;\
    }\
    static void Destroy()\
    {\
        if (m_pInstance != NULL)\
        {\
            delete m_pInstance;\
            m_pInstance = nullptr;\
        }\
    }\
    static bool HasInstance()\
    {\
        return m_pInstance != NULL;\
    }\
private:

#ifdef DEVELOP_VERSION
#define DESTROY_SINGLETON(singleton)\
{\
    if (singleton::HasInstance()) \
    {\
        auto iter = std::find(g_registeredSingleton.begin(), g_registeredSingleton.end(), #singleton); \
        BEATS_ASSERT(iter != g_registeredSingleton.end(), "Singleton %s is not registered!", #singleton); \
        if (iter != g_registeredSingleton.end())\
        {\
            g_registeredSingleton.erase(iter); \
            BEATS_PRINT("singleton %s destoried!\n", #singleton);\
        }\
        singleton::Destroy(); \
    }\
}
#else
#define DESTROY_SINGLETON(singleton) singleton::Destroy()
#endif

//////////////////////////////////////////////////////////////////////////
// Utility
#define BEATS_CLIP_VALUE(var, minValue, maxValue)\
    {\
    if ((var) < (minValue)) {(var)=(minValue);}\
    else if ((var) > (maxValue)) {(var)=(maxValue);}\
    }
#define BEATS_FLOAT_EPSILON 1.192092896e-07F        //FLT_EPSILON
#define BEATS_FLOAT_EQUAL(value, floatValue) (fabs((value) - (floatValue)) < BEATS_FLOAT_EPSILON)
#define BEATS_FLOAT_GREATER(value, floatValue) (((value) - (floatValue)) >  BEATS_FLOAT_EPSILON)
#define BEATS_FLOAT_GREATER_EQUAL(value, floatValue) (BEATS_FLOAT_EQUAL(value, floatValue) || BEATS_FLOAT_GREATER(value, floatValue))
#define BEATS_FLOAT_LESS(value, floatValue) (((value) - (floatValue)) < -BEATS_FLOAT_EPSILON)
#define BEATS_FLOAT_LESS_EQUAL(value, floatValue) (BEATS_FLOAT_EQUAL(value, floatValue) || BEATS_FLOAT_LESS(value, floatValue))

#define BEATS_FLOAT_EQUAL_EPSILON(value, floatValue, epsilon) (fabs((value) - (floatValue)) < epsilon)
#define BEATS_FLOAT_GREATER_EPSILON(value, floatValue, epsilon) (((value) - (floatValue)) >  epsilon)
#define BEATS_FLOAT_GREATER_EQUAL_EPSILON(value, floatValue, epsilon) (BEATS_FLOAT_EQUAL_EPSILON(value, floatValue, epsilon) || BEATS_FLOAT_GREATER_EPSILON(value, floatValue, epsilon))
#define BEATS_FLOAT_LESS_EPSILON(value, floatValue, epsilon) (((value) - (floatValue)) < -epsilon)
#define BEATS_FLOAT_LESS_EQUAL_EPSILON(value, floatValue, epsilon) (BEATS_FLOAT_EQUAL_EPSILON(value, floatValue, epsilon) || BEATS_FLOAT_LESS_EPSILON(value, floatValue, epsilon))

#define BEATS_RAND_RANGE(minValue, maxValue) (((float)rand() / RAND_MAX) * (maxValue - minValue) + minValue)

//////////////////////////////////////////////////////////////////////////
/// Unicode Marco
#ifdef _UNICODE
#define BEATS_STR_UPER _wcsupr_s
#define BEATS_STR_LOWER _wcslwr_s

#else
#define BEATS_STR_UPER _strupr_s
#define BEATS_STR_LOWER _strlwr_s
#endif
    
#if BEYONDENGINE_PLATFORM == PLATFORM_IOS
#endif

inline bool BeyondEngineCheckHeap()
{
    bool bRet = true;
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    bRet = _CrtCheckMemory() != FALSE;
#elif BEYONDENGINE_PLATFORM == PLATFORM_IOS
    bRet = malloc_zone_check(nullptr) != 0;
#endif
    return bRet;
}

//////////////////////////////////////////////////////////////////////////
///Assimbly
#if defined(_DEBUG) && defined(CHECK_HEAP)
#define BEYONDENGINE_CHECK_HEAP BEATS_ASSERT(BeyondEngineCheckHeap(), "_CrtCheckMemory Failed!\nFile:%s Line:%d", __FILE__, __LINE__)
#else
#define BEYONDENGINE_CHECK_HEAP
#endif
    
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    #define BEATS_ASSI_GET_EIP__(intVariable, LineNum) _asm call label##LineNum _asm label##LineNum: _asm pop intVariable __asm{}
    #define BEATS_ASSI_GET_EIP_(intVariable, LineNum) BEATS_ASSI_GET_EIP__(intVariable, LineNum)
    #define BEATS_ASSI_GET_EIP(intVariable) BEATS_ASSI_GET_EIP_(intVariable, __LINE__)
    #define BEATS_ASSI_GET_EBP(intVariable) __asm{mov intVariable, Ebp}
    
#else
    #define BEATS_ASSI_GET_EIP(intVariable)
    #define BEATS_ASSI_GET_EBP(intVariable)
#endif

#define BEYONDENGINE_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100)
#define BEYONDENGINE_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define BEYONDENGINE_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)
#define BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(i) ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? BEYONDENGINE_SWAP32(i) : (i) )
#define BEYONDENGINE_SWAP_INT16_LITTLE_TO_HOST(i) ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? BEYONDENGINE_SWAP16(i) : (i) )
#define BEYONDENGINE_SWAP_INT32_BIG_TO_HOST(i)    ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? (i) : BEYONDENGINE_SWAP32(i) )
#define BEYONDENGINE_SWAP_INT16_BIG_TO_HOST(i)    ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? (i):  BEYONDENGINE_SWAP16(i) )


///////////////////////////////////////////////////////////////////////////////
// BEYONDENGINE_ENDIAN
#define BEYONDENGINE_LITTLEENDIAN  0   //!< Little endian machine
#define BEYONDENGINE_BIGENDIAN     1   //!< Big endian machine

// Endianness of the machine.
//
// GCC 4.6 provided macro for detecting endianness of the target machine. But other
// compilers may not have this. User can define BEYONDENGINE_ENDIAN to either
// BEYONDENGINE_LITTLEENDIAN or BEYONDENGINE_BIGENDIAN.
// Default detection implemented with reference to
//  https://gcc.gnu.org/onlinedocs/gcc-4.6.0/cpp/Common-Predefined-Macros.html
//  http://www.boost.org/doc/libs/1_42_0/boost/detail/endian.hpp

#ifndef BEYONDENGINE_ENDIAN
// Detect with GCC 4.6's macro
#  ifdef __BYTE_ORDER__
#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#      define BEYONDENGINE_ENDIAN   BEYONDENGINE_LITTLEENDIAN
#    elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#      define BEYONDENGINE_ENDIAN   BEYONDENGINE_BIGENDIAN
#    else
#      error Unknown machine endianess detected. User needs to define BEYONDENGINE_ENDIAN.
#    endif // __BYTE_ORDER__
// Detect with GLIBC's endian.h
#  elif defined(__GLIBC__)
#    include <endian.h>
#    if (__BYTE_ORDER == __LITTLE_ENDIAN)
#      define BEYONDENGINE_ENDIAN   BEYONDENGINE_LITTLEENDIAN
#    elif (__BYTE_ORDER == __BIG_ENDIAN)
#      define BEYONDENGINE_ENDIAN   BEYONDENGINE_BIGENDIAN
#    else
#      error Unknown machine endianess detected. User needs to define BEYONDENGINE_ENDIAN.
#   endif // __GLIBC__
// Detect with _LITTLE_ENDIAN and _BIG_ENDIAN macro
#  elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
#    define BEYONDENGINE_ENDIAN BEYONDENGINE_LITTLEENDIAN
#  elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)
#    define BEYONDENGINE_ENDIAN BEYONDENGINE_BIGENDIAN
// Detect with architecture macros
#  elif defined(__sparc) || defined(__sparc__) || defined(_POWER) || defined(__powerpc__) || defined(__ppc__) || defined(__hpux) || defined(__hppa) || defined(_MIPSEB) || defined(_POWER) || defined(__s390__)
#    define BEYONDENGINE_ENDIAN BEYONDENGINE_BIGENDIAN
#  elif defined(__i386__) || defined(__alpha__) || defined(__ia64) || defined(__ia64__) || defined(_M_IX86) || defined(_M_IA64) || defined(_M_ALPHA) || defined(__amd64) || defined(__amd64__) || defined(_M_AMD64) || defined(__x86_64) || defined(__x86_64__) || defined(_M_X64) || defined(__bfin__)
#    define BEYONDENGINE_ENDIAN BEYONDENGINE_LITTLEENDIAN
#  elif defined(_MSC_VER) && defined(_M_ARM)
#    define BEYONDENGINE_ENDIAN BEYONDENGINE_LITTLEENDIAN
#  else
#    error Unknown machine endianess detected. User needs to define BEYONDENGINE_ENDIAN.
#  endif
#endif // BEYONDENGINE_ENDIAN

#endif // BEYOND_ENGINE_PUBLICDEF_H__INCLUDE
