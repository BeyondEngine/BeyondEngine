#ifndef BEYOND_ENGINE_PUBLICDEF_H__INCLUDE
#define BEYOND_ENGINE_PUBLICDEF_H__INCLUDE

#include "Utility/BeatsUtility/Platform/BeatsTchar.h"
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
#include <android/log.h>
#endif
//////////////////////////////////////////////////////////////////////////
//Configuration
//Software skeleton animation
//#define SW_SKEL_ANIM

//use uniform buffer object
//#define USE_UBO

#define BEYOND_ENGINE_PUBLISH_VERSION
#define BEYOND_ENGINE_VERSION d5ceeb3f4fae9b5c63cf547ff2e2d6dacef2e7b8

#ifdef EDITOR_MODE
#define ASSUME_VARIABLE_IN_EDITOR_BEGIN(var) if((var) != NULL){
#else
#define ASSUME_VARIABLE_IN_EDITOR_BEGIN(var) BEATS_ASSERT((var) != NULL);
#endif

#ifdef EDITOR_MODE
#define ASSUME_VARIABLE_IN_EDITOR_END }
#else
#define ASSUME_VARIABLE_IN_EDITOR_END
#endif

//////////////////////////////////////////////////////////////////////////
//Assert
//To call messagebox so we can afford info for the engine user.
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    #ifdef _UNICODE
        #define MessageBox MessageBoxW
    #else
        #define MessageBox MessageBoxA
    #endif
#endif
static TCHAR szBeatsDialogBuffer[10240] = {0};
#ifdef _DEBUG

    #if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
        #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            static bool bIgnoreThisAssert = false;\
            if (!bIgnoreThisAssert)\
            {\
                BEATS_WARNING(condition, __VA_ARGS__)\
                _stprintf_s(szBeatsDialogBuffer, __VA_ARGS__, _T(""));\
                TCHAR errorInfo[2048];\
                _stprintf_s(errorInfo, _T("\nErrorID:%d at %s %d\n"), GetLastError(), _T(__FILE__), __LINE__);\
                _tcscat_s(szBeatsDialogBuffer, errorInfo);\
                _tcscat_s(szBeatsDialogBuffer, _T("Condition: "));\
                _tcscat_s(szBeatsDialogBuffer, _T(#condition));\
                int iRet = MessageBox(NULL, szBeatsDialogBuffer, _T("Beats assert"), MB_ABORTRETRYIGNORE | MB_ICONERROR);\
                if(iRet == IDABORT)\
                {\
                    __asm{int 3};\
                }\
                else if(iRet == IDIGNORE)\
                {\
                    bIgnoreThisAssert = true;\
                }\
            }\
        }
    #elif(BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
        #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            _stprintf(szBeatsDialogBuffer, ##__VA_ARGS__, _T(""));\
            TCHAR szPos[MAX_PATH];\
            _stprintf(szPos, _T("\nAt FILE:%s Line:%d"), _T(__FILE__), __LINE__);\
            __android_log_assert(#condition, "Android_Assert", _T("%s %s"), szBeatsDialogBuffer, szPos);\
        }
    #else
        #define BEATS_ASSERT(condition,...)\
        if (!(condition))\
        {\
            asm{int 3}\
        }
    #endif
#else
#define BEATS_ASSERT(condition, ...)
#endif

//////////////////////////////////////////////////////////////////////////
//Warning
#ifdef _DEBUG
#define BEATS_WARNING(condition, ...)\
    if (!(condition))\
    {\
        BEATS_PRINT(__VA_ARGS__);\
        BEATS_PRINT(_T("\tAt line :%d in file: %s\n"), __LINE__, _T(__FILE__));\
    }
#else
#define BEATS_WARNING(condition, ...)
#endif

//////////////////////////////////////////////////////////////////////////
//DEBUG_PRINT
#ifdef _DEBUG
    #if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
        #define BEATS_PRINT(...)\
        {\
            _stprintf_s(szBeatsDialogBuffer, ##__VA_ARGS__, _T(""));\
            OutputDebugString(szBeatsDialogBuffer);\
        }
    #elif (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
        #define BEATS_PRINT(...)\
        {\
            _stprintf(szBeatsDialogBuffer, ##__VA_ARGS__, _T(""));\
            __android_log_print(ANDROID_LOG_INFO,_T("BEATS_PRINT"), _T("%s"), szBeatsDialogBuffer);\
        }
    #else
        #define BEATS_PRINT(...)\
        {\
            _stprintf_s(szBeatsDialogBuffer, ##__VA_ARGS__, _T(""));\
            _tprintf(szBeatsDialogBuffer);\
        }
    #endif
#else
    #define BEATS_PRINT(...)
#endif


//////////////////////////////////////////////////////////////////////////
// SingletonDef
#define BEATS_DECLARE_SINGLETON(className)\
private:\
    className();\
    ~className();\
    static className* m_pInstance;\
public:\
    static className* GetInstance()\
    {\
    if (m_pInstance == NULL)\
    m_pInstance = new className;\
    return m_pInstance;\
    }\
    static void Destroy()\
    {\
        if (m_pInstance != NULL)\
        {\
            BEATS_SAFE_DELETE(m_pInstance);\
        }\
    }\
    static bool HasInstance()\
    {\
        return m_pInstance != NULL;\
    }\
private:\

//////////////////////////////////////////////////////////////////////////
// Delete
#define BEATS_SAFE_DELETE(p) \
    if ((p) != NULL)\
{\
    delete (p);\
    (p) = NULL;\
}

#define BEATS_SAFE_DELETE_ARRAY(p) if (p!=NULL){delete[] p; p = NULL;}

#define BEATS_SAFE_DELETE_VECTOR(p)\
    for(size_t i = 0; i < (p).size(); ++i)\
    { BEATS_SAFE_DELETE((p)[i]);}\
    (p).clear();

#define BEATS_SAFE_DELETE_MAP(p, type)\
    for(type::iterator iter = (p).begin(); iter != (p).end(); ++iter)\
    { BEATS_SAFE_DELETE((iter)->second);}\
    (p).clear();

#define BEATS_SAFE_RELEASE(p)\
    if (p != NULL)\
    {\
        p->Release();\
    }

//////////////////////////////////////////////////////////////////////////
// Utility
#define BEATS_CLIP_VALUE(var, minValue, maxValue)\
    {\
    if ((var) < (minValue)) {(var)=(minValue);}\
    else if ((var) > (maxValue)) {(var)=(maxValue);}\
    }
#define BEATS_FLOAT_EPSILON 1.192092896e-07F        //FLT_EPSILON
#define BEATS_FLOAT_EQUAL(floatValue, value) (fabs((value) - (floatValue)) < BEATS_FLOAT_EPSILON)

//////////////////////////////////////////////////////////////////////////
/// Unicode Marco
#ifdef _UNICODE
#define BEATS_STR_UPER _wcsupr_s
#define BEATS_STR_LOWER _wcslwr_s

#else
#define BEATS_STR_UPER _strupr_s
#define BEATS_STR_LOWER _strlwr_s

#endif 

//////////////////////////////////////////////////////////////////////////
///Assimbly
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    #define BEATS_ASSI_GET_EIP__(intVariable, LineNum) _asm call label##LineNum _asm label##LineNum: _asm pop intVariable __asm{}
    #define BEATS_ASSI_GET_EIP_(intVariable, LineNum) BEATS_ASSI_GET_EIP__(intVariable, LineNum)
    #define BEATS_ASSI_GET_EIP(intVariable) BEATS_ASSI_GET_EIP_(intVariable, __LINE__)

    #define BEATS_ASSI_GET_EBP(intVariable) __asm{mov intVariable, Ebp}

#else

    #define BEATS_ASSI_GET_EIP__(intVariable, LineNum) asm call label##LineNum asm label##LineNum: asm pop intVariable asm{}
    #define BEATS_ASSI_GET_EIP_(intVariable, LineNum) BEATS_ASSI_GET_EIP__(intVariable, LineNum)
    #define BEATS_ASSI_GET_EIP(intVariable) BEATS_ASSI_GET_EIP_(intVariable, __LINE__)

    #define BEATS_ASSI_GET_EBP(intVariable) asm{mov intVariable, Ebp}
#endif

#define BEYONDENGINE_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define BEYONDENGINE_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define BEYONDENGINE_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(i) ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? BEYONDENGINE_SWAP32(i) : (i) )
#define BEYONDENGINE_SWAP_INT16_LITTLE_TO_HOST(i) ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? BEYONDENGINE_SWAP16(i) : (i) )
#define BEYONDENGINE_SWAP_INT32_BIG_TO_HOST(i)    ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? (i) : BEYONDENGINE_SWAP32(i) )
#define BEYONDENGINE_SWAP_INT16_BIG_TO_HOST(i)    ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? (i):  BEYONDENGINE_SWAP16(i) )

#endif