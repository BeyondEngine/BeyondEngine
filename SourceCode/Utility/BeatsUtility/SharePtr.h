#ifndef BEATS_UTILITY_SHAREPTR_SHAREPTR_H__INCLUDE
#define BEATS_UTILITY_SHAREPTR_SHAREPTR_H__INCLUDE

#include <mutex>

#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    #ifdef _DEBUG
    #include "dbghelp.h"
    //#define SHARE_PTR_TRACE
    #endif

    #ifdef SHARE_PTR_TRACE
    #include "Utility/BeatsUtility/StringHelper.h"
    #include <set>
    #endif

#elif(BEATS_PLATFORM == BEATS_PLATFORM_IOS)
    #include <libkern/OSAtomic.h>
#endif

template<typename ClassType>
class SharePtr
{
    // This is for implicit conversion about bool. Such as bool tmp = SharePtr<ClassType>;
    // Don't try to override operator bool, because it will also implicit to be treated as an integer.
private:
    struct Dummy{};
    typedef void (SharePtr::*TDummyFunc)(Dummy*);
    void DummyFunc(Dummy*){}
public:
    operator TDummyFunc() const
    {
        return m_pObject ? &SharePtr::DummyFunc : NULL;
    }

public:
    SharePtr<ClassType>()
        : m_pRefCount(NULL)
        , m_pObject(NULL)
#ifdef SHARE_PTR_TRACE
        , m_uRefrencePos(0)
        , m_pRefrencePosSet(NULL)
#endif
    {
    }

    SharePtr<ClassType>(ClassType* pObject)
        : m_pRefCount (NULL)
        , m_pObject(pObject)
#ifdef SHARE_PTR_TRACE
        , m_uRefrencePos(0)
        , m_pRefrencePosSet(NULL)
#endif
    {
        if (pObject != NULL)
        {
#ifdef SHARE_PTR_TRACE
            m_pRefrencePosSet = new std::multiset<size_t>;
            size_t eBPValue = 0;
            BEATS_ASSI_GET_EBP(eBPValue);
            m_uRefrencePos = (*(size_t*)((size_t*)(eBPValue) + 1));//current eip,ignore it.
            size_t lastEBPValue = *(size_t*)(eBPValue);
            size_t ignoreFrameCount = 1;
            bool bMeaningfulFile = false;
            while (ignoreFrameCount != 0 || !bMeaningfulFile)
            {
                if (ignoreFrameCount > 0)
                {
                    -- ignoreFrameCount;
                }
                if(ignoreFrameCount == 0)
                {
                    DWORD displacement = 0;
                    IMAGEHLP_LINE info;
                    ZeroMemory(&info, sizeof(info));
                    info.SizeOfStruct = sizeof(info);
                    bool bGetAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), m_uRefrencePos, &displacement, &info) == TRUE;
                    if (bGetAddrSuccess)
                    {
                        const char* pszPathExtension = PathFindExtensionA(info.FileName);
                        bMeaningfulFile = strlen(pszPathExtension) > 0;
                    }
                    else
                    {
                        bMeaningfulFile = true;
                    }
                }
                if (!bMeaningfulFile)
                {
                    lastEBPValue = *(size_t*)(eBPValue);
                    // If the module enable FPO optimize, the address may be invalid.
                    eBPValue = lastEBPValue;
                    size_t* pPtr = (size_t*)((size_t*)(eBPValue) + 1);
                    if (lastEBPValue == 0 || IsBadCodePtr((FARPROC)pPtr))
                    {
                        break;
                    }
                    m_uRefrencePos = (*pPtr);
                }
            }
            m_pRefrencePosSet->insert(m_uRefrencePos);

#endif
            m_pRefCount = new long(1);
        }
    }

    //Copy constructor.
    SharePtr<ClassType>(const SharePtr<ClassType>& value)
        : m_pRefCount(NULL)
        , m_pObject(NULL)
#ifdef SHARE_PTR_TRACE
        , m_uRefrencePos(0)
        , m_pRefrencePosSet(NULL)
#endif
    {
        *this = value;
    }

    template<class ClassType2>
    SharePtr<ClassType>(const SharePtr<ClassType2>& value)
        : m_pRefCount(NULL)
        , m_pObject(NULL)
#ifdef SHARE_PTR_TRACE
        , m_uRefrencePos(0)
        , m_pRefrencePosSet(NULL)
#endif
    {
        *this = value;
    }

    ~SharePtr<ClassType>()
    {
        Destroy();
    }

    const SharePtr<ClassType>&  operator = (const SharePtr<ClassType>& value)
    {
        if (value == NULL)
        {
            Destroy();
            m_pRefCount = NULL;
            m_pObject = NULL;
        }
        else
        {
            CopyImpl(value);
        }
        return *this;
    }

    template<class ClassType2>
    const SharePtr<ClassType>&  operator = (const SharePtr<ClassType2>& value)
    {
        return CopyImpl(value);
    }

    ClassType* operator ->()
    {
        return m_pObject;
    }

    ClassType* operator ->() const
    {
        return m_pObject;
    }

    ClassType& operator *()
    {
        return *m_pObject;
    }

    template<class ClassType2>
    bool operator == (const SharePtr<ClassType2>& rhs) const
    {
        return m_pObject == rhs.Get();
    }

    template<class ClassType2>
    bool operator != (const SharePtr<ClassType2>& rhs) const
    {
        return m_pObject != rhs.Get();
    }

    void Create(ClassType* pObject)
    {
        bool bCanCreate = m_pObject == NULL && m_pRefCount == NULL && pObject != NULL;
        BEATS_ASSERT(bCanCreate, _T("Can't create a share ptr twice!"));
        if (bCanCreate)
        {
            m_pObject = pObject;
            m_pRefCount = new long(1);
#ifdef SHARE_PTR_TRACE
            m_pRefrencePosSet = new std::multiset<size_t>;
#endif
        }
    }

    ClassType* Get() const
    {
        return m_pObject;
    }

    int RefCount() const
    {
        return *m_pRefCount;
    }

    long* RefCountPtr() const
    {
        return m_pRefCount;
    }

    void Destroy(bool bSimulate = false)
    {
        bool bCanDestroy = m_pRefCount != NULL && m_pObject != NULL && *m_pRefCount > 0;
        if (bCanDestroy)
        {
            BEATS_ASSERT(*m_pRefCount > 0, _T("Ref count is invalid for share pointer destroy"));
            Beats_AtomicDecrement(m_pRefCount);
#ifdef SHARE_PTR_TRACE
            m_lockmutex.lock();
            std::multiset<size_t>::iterator iter = m_pRefrencePosSet->find(m_uRefrencePos);
            BEATS_ASSERT(iter != m_pRefrencePosSet->end());
            m_pRefrencePosSet->erase(iter);
            m_lockmutex.unlock();
#endif 

            if (*m_pRefCount == 0)
            {
                BEATS_SAFE_DELETE(m_pRefCount);
                if (!bSimulate)
                {
                    BEATS_SAFE_DELETE(m_pObject);
                }
                else
                {
                    m_pObject = NULL;
                }
#ifdef SHARE_PTR_TRACE
                BEATS_SAFE_DELETE(m_pRefrencePosSet);
#endif
            }
        }
    }

private:
    template<class ClassType2>
    const SharePtr<ClassType>&  CopyImpl (const SharePtr<ClassType2>& value)
    {
        if (*this != value)
        {
            if (m_pObject != NULL && m_pRefCount != NULL && *m_pRefCount > 0)
            {
                Destroy();
            }
#ifdef SHARE_PTR_TRACE
            m_pObject = dynamic_cast<ClassType*>(value.Get());
            if (m_pObject == NULL)
            {
                TCHAR szBuffer[MAX_PATH];
                CStringHelper::GetInstance()->ConvertToTCHAR(typeid(value).name(), szBuffer, MAX_PATH);
                BEATS_ASSERT(false, _T("Dynamic cast type failed in SharePtr!\nFunction:\n %s\nwith type of\n %s"), _T(__FUNCTION__), szBuffer);
            }
#else
            m_pObject = static_cast<ClassType*>(value.Get());
#endif
            m_pRefCount = value.RefCountPtr();
            Beats_AtomicIncrement(m_pRefCount);

#ifdef SHARE_PTR_TRACE
            SetRefPos(value.m_pRefrencePosSet);
#endif
        }
        return *this;
    }

private:
    long* m_pRefCount;
    ClassType* m_pObject;

#ifdef  SHARE_PTR_TRACE
public:
    size_t m_uRefrencePos;
    std::multiset<size_t>* m_pRefrencePosSet;
    static std::mutex m_lockmutex;

    void SetRefPos(std::multiset<size_t> *refPosSet)
    {
        m_lockmutex.lock();
        m_pRefrencePosSet = refPosSet;

        size_t eBPValue = 0;
        BEATS_ASSI_GET_EBP(eBPValue);
        m_uRefrencePos = (*(size_t*)((size_t*)(eBPValue) + 1));//current eip,ignore it.
        size_t lastEBPValue = *(size_t*)(eBPValue);
        size_t ignoreFrameCount = 2;
        bool bMeaningfulFile = false;
        while (ignoreFrameCount != 0 || !bMeaningfulFile)
        {
            if (ignoreFrameCount > 0)
            {
                -- ignoreFrameCount;
            }
            if(ignoreFrameCount == 0)
            {
                DWORD displacement = 0;
                IMAGEHLP_LINE info;
                ZeroMemory(&info, sizeof(info));
                info.SizeOfStruct = sizeof(info);
                bool bGetAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), m_uRefrencePos, &displacement, &info) == TRUE;
                if (bGetAddrSuccess)
                {
                    const char* pszPathExtension = PathFindExtensionA(info.FileName);                        
                    bMeaningfulFile = strlen(pszPathExtension) > 0 && 
                        _stricmp(PathFindFileNameA(info.FileName), "shareptr.h") != 0 &&
                        _stricmp(PathFindFileNameA(info.FileName), "resourcemanager.h") != 0;
                }
                else
                {
                    bMeaningfulFile = true;
                }
            }
            if (!bMeaningfulFile)
            {
                lastEBPValue = *(size_t*)(eBPValue);
                // If the module enable FPO optimize, the address may be invalid.
                eBPValue = lastEBPValue;
                size_t* pPtr = (size_t*)((size_t*)(eBPValue) + 1);
                if (lastEBPValue == 0 || IsBadCodePtr((FARPROC)pPtr))
                {
                    break;
                }
                m_uRefrencePos = (*pPtr);
            }
        }
        m_pRefrencePosSet->insert(m_uRefrencePos);
        m_lockmutex.unlock();
    }

    void GetReferencePosString(std::string& strOut)
    {
        m_lockmutex.lock();
        char szBuffer[MAX_PATH];
        size_t uIndex = 0;
        for (std::multiset<size_t>::iterator iter = m_pRefrencePosSet->begin(); iter != m_pRefrencePosSet->end(); ++iter)
        {
            DWORD displacement = 0;
            IMAGEHLP_LINE info;
            ZeroMemory(&info, sizeof(info));
            info.SizeOfStruct = sizeof(info);
            bool bGetAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), *iter, &displacement, &info) == TRUE;
            BEATS_ASSERT(bGetAddrSuccess, _T("Get code pos failed!"));

            if (bGetAddrSuccess)
            {
                sprintf_s(szBuffer, "%d.%s Line:%d\n\n", uIndex++, info.FileName, info.LineNumber);
                strOut.append(szBuffer);
            }
        }
        m_lockmutex.unlock();
    }
#endif
};

#ifdef SHARE_PTR_TRACE
template<class ClassType>
std::mutex SharePtr<ClassType>::m_lockmutex;
#endif
#endif