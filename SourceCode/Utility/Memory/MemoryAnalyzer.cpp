#include "stdafx.h"
#include "MemoryAnalyzer.h"

#ifdef MEMORY_CAPTURE

CMemoryAnalyzer::CMemoryAnalyzer()
    : m_size(0)
{
}

void CMemoryAnalyzer::Register(void* address, uint32_t size, const TCHAR* tag, const TCHAR* detail)
{
    if (detail == nullptr || strlen(detail) == 0)
    {
        detail = "default";
    }
    std::lock_guard<std::mutex> autolock(m_lock);
    m_addressInfoMap[address] = AddressInfo(tag, detail, size);

    TString strTag(tag);
    std::map<TString, TagInfo>::iterator it = m_tagMap.find(strTag);
    if (it == m_tagMap.end())
    {
        m_tagMap.insert(std::pair<TString, TagInfo>(strTag, TagInfo()));
        it = m_tagMap.find(strTag);
    }
    it->second.m_size += size;
    it->second.m_detailMap[detail] += size;
    m_size += size;
}

void CMemoryAnalyzer::UnRegister(void* address)
{
    std::lock_guard<std::mutex> autolock(m_lock);
    std::map<void*, AddressInfo>::iterator addressIt = m_addressInfoMap.find(address);
    // can be removed after we use BEATS_NEW for all memory allocation.
    if (addressIt != m_addressInfoMap.end())
    {
        std::map<TString, TagInfo>::iterator it = m_tagMap.find(addressIt->second.m_tag);
        std::map<TString, uint32_t>::iterator detailIt = it->second.m_detailMap.find(addressIt->second.m_detail);
        detailIt->second -= addressIt->second.m_size;

        // don't have any allocation on this tag/detail anymore.
        if (detailIt->second == 0)
        {
            it->second.m_detailMap.erase(detailIt);
        }

        it->second.m_size -= addressIt->second.m_size;
        m_size -= addressIt->second.m_size;
        m_addressInfoMap.erase(addressIt);
    }
}

void CMemoryAnalyzer::DumpToString(TString& dump)
{
    std::lock_guard<std::mutex> autolock(m_lock);
    TCHAR szBuffer[MAX_PATH];
    _stprintf(szBuffer, "%d", m_size);
    TString strFormatNumber = CStringHelper::GetInstance()->InsertString(szBuffer, ",", true, 3, 3);
    TStringstream sstream;
    sstream << "total usage : " << strFormatNumber << std::endl;
    // sort all info by the size
    std::multimap<uint32_t, const TString*> sortMap;
    for (std::map<TString, TagInfo>::iterator it = m_tagMap.begin();it != m_tagMap.end(); ++it)
    {
        sortMap.insert(std::pair<uint32_t, const TString*>(it->second.m_size, &it->first));
    }
    for (std::multimap<uint32_t, const TString*>::reverse_iterator it = sortMap.rbegin(); it != sortMap.rend(); ++it)
    {
        const TString& strKey = *it->second;
        auto infoIter = m_tagMap.find(strKey);
        BEATS_ASSERT(infoIter != m_tagMap.end());
        // tag name
        _stprintf(szBuffer, "%d", infoIter->second.m_size);
        strFormatNumber = CStringHelper::GetInstance()->InsertString(szBuffer, ",", true, 3, 3);
        sstream << strKey << ":" << strFormatNumber << std::endl;
        std::multimap<uint32_t, const TString*> subSortMap;        
        for (std::map<TString, uint32_t>::iterator detailIt = infoIter->second.m_detailMap.begin(); detailIt != infoIter->second.m_detailMap.end(); ++detailIt)
        {
            subSortMap.insert(std::pair<uint32_t, const TString*>(detailIt->second, &detailIt->first));
        }
        for (auto subSortIter = subSortMap.rbegin(); subSortIter != subSortMap.rend(); ++subSortIter)
        {
            // detail name
            _stprintf(szBuffer, "%d", subSortIter->first);
            strFormatNumber = CStringHelper::GetInstance()->InsertString(szBuffer, ",", true, 3, 3);
            sstream << "    " << strKey << "-------------" << (*subSortIter->second) << ":" << strFormatNumber << std::endl;
        }
    }

    dump = sstream.str();
}

void CMemoryAnalyzer::DumpToFile(const TCHAR* filename)
{
    std::ofstream dumpFile(filename);
    if (dumpFile.is_open())
    {
        TString content;
        DumpToString(content);
        dumpFile << content;
        dumpFile.close();
    }
    TString strResourceCompressLog = _T("notepad ");
    strResourceCompressLog.append(filename);
    _tsystem(strResourceCompressLog.c_str());
}

#endif