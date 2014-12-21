#ifndef BEYOND_ENGINE_UTILITY_MEMORY_MEMORY_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MEMORY_MEMORY_H__INCLUDE

#include "MemoryAnalyzer.h"

namespace Details
{
    template<typename T, bool IsSingletonTag>
    struct Deallocator{};

    template<typename T>
    struct Deallocator< T, true >
    {
        static void Deallocate(T* ptr)
        {
            ptr->Destroy();
        }
    };

    template<typename T>
    struct Deallocator< T, false >
    {
        static void Deallocate(T* ptr)
        {
            delete ptr;
        }
    };
}

template<typename T>
struct IsSingleton
{
private:
    typedef uint16_t Yes;
    typedef uint32_t No;

    template<typename T2>
    static Yes Check(typename T2::IsSingletonTag*);

    template<typename T2>
    static No Check(...);

public:
    enum
    {
        Answer = (sizeof(Check<T>(0)) == sizeof(Yes))
    };
};


template<typename T>
inline T* Allocate(const TCHAR* file, uint32_t line, const TCHAR* tag, const TCHAR* detail)
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    T* ptr = new (_NORMAL_BLOCK, file, line) T;
#else
    T* ptr = new T;
    BEYONDENGINE_UNUSED_PARAM(file);
    BEYONDENGINE_UNUSED_PARAM(line);
#endif
    BEYONDENGINE_UNUSED_PARAM(tag);
    BEYONDENGINE_UNUSED_PARAM(detail);
#ifdef MEMORY_CAPTURE
    if (tag)
    {
        CMemoryAnalyzer::GetInstance()->Register(ptr, sizeof(T), tag, detail);
    }
#endif
    return ptr;
}

template<typename T>
inline T* AllocateArray(const TCHAR* file, uint32_t line, uint32_t count, const TCHAR* tag, const TCHAR* detail)
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    T* ptr = new (_NORMAL_BLOCK, file, line) T[count];
#else
    T* ptr = new T[count];
    BEYONDENGINE_UNUSED_PARAM(file);
    BEYONDENGINE_UNUSED_PARAM(line);
#endif
    BEYONDENGINE_UNUSED_PARAM(detail);
    BEYONDENGINE_UNUSED_PARAM(tag);
#ifdef MEMORY_CAPTURE
    if (tag)
    {
        CMemoryAnalyzer::GetInstance()->Register(ptr, sizeof(T) * count, tag, detail);
    }
#endif
    return ptr;
}

template<typename T>
inline void Deallocate(T* ptr)
{
#ifdef MEMORY_CAPTURE
    CMemoryAnalyzer::GetInstance()->UnRegister(ptr);
#endif
    Details::Deallocator<T, IsSingleton<T>::Answer>::Deallocate(ptr);
}

template<typename T>
inline void DeallocateArray(T* ptr)
{
#ifdef MEMORY_CAPTURE
    CMemoryAnalyzer::GetInstance()->UnRegister(ptr);
#endif
    delete[] ptr;
}

#define BEATS_NEW(objType, tag, detail) \
    Allocate<objType>(__FILE__, __LINE__, tag, detail)

#define BEATS_DELETE(pObj) \
    Deallocate(pObj)

#define BEATS_NEW_ARRAY(objType, count, tag, detail) \
    AllocateArray<objType>(__FILE__, __LINE__, count, tag, detail)

#define BEATS_DELETE_ARRAY(arrayObj) \
    DeallocateArray(arrayObj)

//////////////////////////////////////////////////////////////////////////
// Delete
#define BEATS_SAFE_DELETE(p) \
    if ((p) != NULL)\
        {\
    BEATS_DELETE(p);\
    (p) = NULL;\
        }

#define BEATS_SAFE_DELETE_COMPONENT(p) \
if ((p) != NULL)\
{\
    if (p->IsInitialized())\
            {\
        p->Uninitialize(); \
            }\
    BEATS_DELETE(p); \
    (p) = NULL; \
}

#define BEATS_SAFE_DELETE_ARRAY(p) if (p!=NULL){ BEATS_DELETE_ARRAY(p); p = NULL;}

#define BEATS_SAFE_DELETE_VECTOR(p)\
    for(uint32_t i = 0; i < (p).size(); ++i)\
            { BEATS_SAFE_DELETE((p)[i]);}\
    (p).clear();

#define BEATS_SAFE_DELETE_MAP(p)\
    for(auto iter = (p).begin(); iter != (p).end(); ++iter)\
            { BEATS_SAFE_DELETE((iter)->second);}\
    (p).clear();

#define BEATS_SAFE_RELEASE(p)\
    if (p != NULL)\
            {\
        p->Release();\
            }


#endif
