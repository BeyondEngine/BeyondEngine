#ifndef BEATS_UTILITY_UTILITYMANAGER_H__INCLUDE
#define BEATS_UTILITY_UTILITYMANAGER_H__INCLUDE

struct SDirectory
{
    SDirectory(SDirectory* pParent, const TCHAR* pszPath)
        : m_pParent(pParent)
    {
        if (pszPath != NULL)
        {
            m_szPath.assign(pszPath);
        }
        m_pFileList = new std::vector<TFileData*>;
        m_pDirectories = new std::vector<SDirectory*>;
    }
    ~SDirectory()
    {
        for (size_t i = 0; i < m_pFileList->size(); ++i)
        {
            BEATS_SAFE_DELETE((*m_pFileList)[i]);
        }
        BEATS_SAFE_DELETE(m_pFileList);
        for (size_t i = 0; i < m_pDirectories->size(); ++i)
        {
            BEATS_SAFE_DELETE((*m_pDirectories)[i]);
        }
        BEATS_SAFE_DELETE(m_pDirectories);
    }

    SDirectory* GetChild(const TString& childPath) const
    {
        SDirectory* pRet = NULL;
        for (size_t i = 0; i < m_pDirectories->size(); ++i)
        {
            if (m_pDirectories->at(i)->m_szPath.compare(childPath) == 0)
            {
                pRet = m_pDirectories->at(i);
                break;
            }
        }
        return pRet;
    }

    void SetData(const TFileData& rData)
    {
        memcpy(&m_data, &rData, sizeof(TFileData));
    }

    SDirectory* m_pParent;
    std::vector<TFileData*>* m_pFileList;
    std::vector<SDirectory*>* m_pDirectories;
    TFileData m_data;
    TString m_szPath;
};

struct SLogicDriverInfo
{
    SLogicDriverInfo()
        : m_type(0)
        , m_diskSize(0)
        , m_diskFreeSize(0)
    {

    }
    ~SLogicDriverInfo()
    {

    }
    char m_type;
    unsigned long long m_diskSize;
    unsigned long long m_diskFreeSize;
    TString m_diskName;
};

struct SDiskInfo
{
    SDiskInfo& operator = (const SDiskInfo& rRef)
    {
        if (this != &rRef)
        {
            m_logicDriverInfo.clear();
            for (size_t i = 0; i < rRef.m_logicDriverInfo.size(); ++i)
            {
                m_logicDriverInfo.push_back(rRef.m_logicDriverInfo[i]);
            }
        }

        return *this;
    }
    std::vector<SLogicDriverInfo> m_logicDriverInfo;
};
//////////////////////////////////////////////////////////////////////////
class CFileFilter;
class CSerializer;
class CMD5;

class CUtilityManager
{
    BEATS_DECLARE_SINGLETON(CUtilityManager)
public:
    void Init();
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    bool AcquireSingleFilePath(bool saveOrOpen, HWND hwnd, TString& result, const TCHAR* Tiltle, const TCHAR* filter, const TCHAR* pszInitialPath);
    bool AcquireMuiltyFilePath(bool bAllowDirectory, HWND hwnd, std::vector<TString>& result, const TCHAR* Tiltle, const TCHAR* filter, const TCHAR* pszInitialPath);
    bool AcquireDirectory(HWND hwnd, TString& strDirectoryPath, const TCHAR* pszTitle);
    // File Directory
    bool FillDirectory(SDirectory& fileList, bool bFillSubDirectory = true, CFileFilter* pFileFilter = NULL, unsigned long long* pFileSize = NULL);
    unsigned long long BuildDirectoryToList(SDirectory* pDirectory, std::vector<TFileData*>& listToAppend);
    void SerializeDirectory(SDirectory* pDirectory, CSerializer& serializer);
    void DeserializeDirectory(SDirectory* pDirectory, CSerializer& serializer, long long* pTotalDataSize = NULL, size_t* pFileCount = NULL);

    // MD5
    bool CalcMD5(CMD5& md5, SDirectory& fileList);

#endif
    bool WriteDataToFile(FILE* pFile, void* pData, size_t uDataLength, size_t uRetryCount = 20);
    bool ReadDataFromFile(FILE* pFile, void* pData, size_t uDataLength, size_t uRetryCount = 20);

    // System Module
    bool GetProcessModule(size_t uProcessId, std::vector<TString>& modulePath);
    const TString& GetModuleFileName();
    void SetModuleFileName(const TCHAR* pszFileName);
    
private:
    TString m_strFileModuleName;
};


#endif