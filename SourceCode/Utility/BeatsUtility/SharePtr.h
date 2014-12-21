#ifndef BEATS_UTILITY_SHAREPTR_SHAREPTR_H__INCLUDE
#define BEATS_UTILITY_SHAREPTR_SHAREPTR_H__INCLUDE

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    #include "dbghelp.h"
    //#define SHARE_PTR_TRACE

    #ifdef SHARE_PTR_TRACE
    #include "Utility/BeatsUtility/StringHelper.h"
    #include "Utility/BeatsUtility/FilePathTool.h"
    #endif

#elif(BEYONDENGINE_PLATFORM == PLATFORM_IOS)
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
        : m_pRefCount(NULL)
        , m_pObject(pObject)
#ifdef SHARE_PTR_TRACE
        , m_uRefrencePos(0)
        , m_pRefrencePosSet(NULL)
#endif
    {
        if (pObject != NULL)
        {
#ifdef SHARE_PTR_TRACE
            m_pRefrencePosSet = new std::multiset<uint32_t>;
            uint32_t eBPValue = 0;
            BEATS_ASSI_GET_EBP(eBPValue);
            m_uRefrencePos = (*(uint32_t*)((uint32_t*)(eBPValue)+1));//current eip,ignore it.
            uint32_t lastEBPValue = *(uint32_t*)(eBPValue);
            uint32_t ignoreFrameCount = 1;
            bool bMeaningfulFile = false;
            while (ignoreFrameCount != 0 || !bMeaningfulFile)
            {
                if (ignoreFrameCount > 0)
                {
                    --ignoreFrameCount;
                }
                if (ignoreFrameCount == 0)
                {
                    DWORD displacement = 0;
                    IMAGEHLP_LINE info;
                    ZeroMemory(&info, sizeof(info));
                    info.SizeOfStruct = sizeof(info);
                    bool bGetAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), m_uRefrencePos, &displacement, &info) == TRUE;
                    if (bGetAddrSuccess)
                    {
                        bMeaningfulFile = CFilePathTool::GetInstance()->Extension(info.FileName).length() > 0;
                    }
                    else
                    {
                        bMeaningfulFile = true;
                    }
                }
                if (!bMeaningfulFile)
                {
                    lastEBPValue = *(uint32_t*)(eBPValue);
                    // If the module enable FPO optimize, the address may be invalid.
                    eBPValue = lastEBPValue;
                    uint32_t* pPtr = (uint32_t*)((uint32_t*)(eBPValue)+1);
                    if (lastEBPValue == 0 || IsBadCodePtr((FARPROC)pPtr))
                    {
                        break;
                    }
                    m_uRefrencePos = (*pPtr);
                }
            }
            m_pRefrencePosSet->insert(m_uRefrencePos);
#endif
            m_pRefCount = new uint32_t(1);
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

    SharePtr<ClassType>&  operator = (const SharePtr<ClassType>& value)
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
    SharePtr<ClassType>&  operator = (const SharePtr<ClassType2>& value)
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
            m_pRefCount = new uint32_t(1);
#ifdef SHARE_PTR_TRACE
            m_pRefrencePosSet = new std::multiset<uint32_t>;
#endif
        }
    }

    ClassType* Get() const
    {
        return m_pObject;
    }

    uint32_t RefCount() const
    {
        return *m_pRefCount;
    }

    uint32_t* RefCountPtr() const
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
            sharePtrLockMutex.lock();
            std::multiset<uint32_t>::iterator iter = m_pRefrencePosSet->find(m_uRefrencePos);
            BEATS_ASSERT(iter != m_pRefrencePosSet->end());
            m_pRefrencePosSet->erase(iter);
            sharePtrLockMutex.unlock();
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
    SharePtr<ClassType>&  CopyImpl(const SharePtr<ClassType2>& value)
    {
        if (*this != value)
        {
            if (m_pObject != NULL && m_pRefCount != NULL && *m_pRefCount > 0)
            {
                Destroy();
            }
#ifdef SHARE_PTR_TRACE
            m_pObject = dynamic_cast<ClassType*>(value.Get());
            BEATS_ASSERT(m_pObject != NULL, _T("Dynamic cast type failed in SharePtr!\nFunction:\n %s\nwith type of\n %s"), _T(__FUNCTION__), typeid(value).name());
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
    uint32_t* m_pRefCount;
    ClassType* m_pObject;

#ifdef  SHARE_PTR_TRACE
public:
    uint32_t m_uRefrencePos;
    std::multiset<uint32_t>* m_pRefrencePosSet;

    void SetRefPos(std::multiset<uint32_t> *refPosSet)
    {
        sharePtrLockMutex.lock();
        m_pRefrencePosSet = refPosSet;

        uint32_t eBPValue = 0;
        BEATS_ASSI_GET_EBP(eBPValue);
        m_uRefrencePos = (*(uint32_t*)((uint32_t*)(eBPValue)+1));//current eip,ignore it.
        uint32_t lastEBPValue = *(uint32_t*)(eBPValue);
        uint32_t ignoreFrameCount = 2;
        bool bMeaningfulFile = false;
        while (ignoreFrameCount != 0 || !bMeaningfulFile)
        {
            if (ignoreFrameCount > 0)
            {
                --ignoreFrameCount;
            }
            if (ignoreFrameCount == 0)
            {
                DWORD displacement = 0;
                IMAGEHLP_LINE info;
                ZeroMemory(&info, sizeof(info));
                info.SizeOfStruct = sizeof(info);
                bool bGetAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), m_uRefrencePos, &displacement, &info) == TRUE;
                if (bGetAddrSuccess)
                {
                    TString strPathExtension = CFilePathTool::GetInstance()->Extension(info.FileName);
                    TString strFileName = CFilePathTool::GetInstance()->FileName(info.FileName);
                    bMeaningfulFile = strPathExtension.length() > 0 &&
                        _stricmp(strFileName.c_str(), "shareptr.h") != 0 &&
                        _stricmp(strFileName.c_str(), "resourcemanager.h") != 0;
                }
                else
                {
                    bMeaningfulFile = true;
                }
            }
            if (!bMeaningfulFile)
            {
                lastEBPValue = *(uint32_t*)(eBPValue);
                // If the module enable FPO optimize, the address may be invalid.
                eBPValue = lastEBPValue;
                uint32_t* pPtr = (uint32_t*)((uint32_t*)(eBPValue)+1);
                if (lastEBPValue == 0 || IsBadCodePtr((FARPROC)pPtr))
                {
                    break;
                }
                m_uRefrencePos = (*pPtr);
            }
        }
        m_pRefrencePosSet->insert(m_uRefrencePos);
        sharePtrLockMutex.unlock();
    }

    TString GetReferencePosString() const
    {
        TString strRet;
        sharePtrLockMutex.lock();
        char szBuffer[MAX_PATH];
        uint32_t uIndex = 0;
        for (std::multiset<uint32_t>::iterator iter = m_pRefrencePosSet->begin(); iter != m_pRefrencePosSet->end(); ++iter)
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
                strRet.append(szBuffer);
            }
        }
        sharePtrLockMutex.unlock();
        return strRet;
    }
#endif
};

#ifdef SHARE_PTR_TRACE
extern std::mutex sharePtrLockMutex;
#endif
#endif