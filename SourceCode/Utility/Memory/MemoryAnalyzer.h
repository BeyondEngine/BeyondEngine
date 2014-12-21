#ifndef BEYOND_ENGINE_UTILITY_MEMORY_MEMORYANALYER_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MEMORY_MEMORYANALYER_H__INCLUDE

#ifdef MEMORY_CAPTURE

struct TagInfo
{
    TagInfo() : m_size(0) {}
    uint32_t                        m_size;
    std::map<TString, uint32_t>    m_detailMap;
};

struct AddressInfo
{
    AddressInfo() : m_size(0) {}
    AddressInfo(const TCHAR* tag, const TCHAR* detail, uint32_t size)
        : m_tag(tag), m_detail(detail), m_size(size) {}
    TString        m_tag;
    TString        m_detail;
    uint32_t    m_size;
};

class CMemoryAnalyzer
{
public:
    static CMemoryAnalyzer* GetInstance()
    {
        static CMemoryAnalyzer instance;
        return &instance;
    }
    CMemoryAnalyzer();

    void Register(void* address, uint32_t size, const TCHAR* tag, const TCHAR* detail);
    void UnRegister(void* address);

    void DumpToString(TString& dump);
    void DumpToFile(const TCHAR* filename);
private:
    std::mutex                      m_lock;
    std::map<TString, TagInfo>      m_tagMap;
    std::map<void*, AddressInfo>    m_addressInfoMap;
    uint32_t                        m_size;
};


#endif // DEBUG


#endif // BEYOND_ENGINE_UTILITY_MEMORY_MEMORYANALYER_H__INCLUDE
