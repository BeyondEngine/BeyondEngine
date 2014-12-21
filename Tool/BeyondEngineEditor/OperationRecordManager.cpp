#include "stdafx.h"
#include "OperationRecordManager.h"

COperationRecordManager* COperationRecordManager::m_pInstance = nullptr;

COperationRecordManager::COperationRecordManager()
{

}

COperationRecordManager::~COperationRecordManager()
{
    ClearRecord();
    for (auto iter = m_recordCache.begin(); iter != m_recordCache.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            BEATS_SAFE_DELETE(*subIter);
        }
    }
    m_recordCache.clear();
}

void COperationRecordManager::AppendRecord(COperationRecord* pRecord)
{
    if (m_recordList.size() > 0)
    {
        if (m_recordList.size() > 100)
        {
            std::vector<COperationRecord*> newRecordList;
            for (size_t i = 0; i < m_recordList.size(); ++i)
            {
                if (i < 50)
                {
                    EOperationRecordType type = m_recordList[i]->GetType();
                    m_recordCache[type].push_back(m_recordList[i]);
                }
                else
                {
                    newRecordList.push_back(m_recordList[i]);
                }
            }
            m_recordList.swap(newRecordList);
            m_nCurrIndex -= 50;
            if (m_nCurrIndex < -1)
            {
                m_nCurrIndex = -1;
            }
        }
        if (m_nCurrIndex < (int)m_recordList.size() - 1)
        {
            for (int i = m_nCurrIndex + 1; i < (int)m_recordList.size(); ++i)
            {
                EOperationRecordType type = m_recordList[i]->GetType();
                m_recordCache[type].push_back(m_recordList[i]);
            }
            m_recordList.resize(m_nCurrIndex + 1);
        }
    }
    m_recordList.push_back(pRecord);
    m_nCurrIndex = m_recordList.size() - 1;
}

bool COperationRecordManager::DoRecord()
{
    bool bRet = m_nCurrIndex < (int)m_recordList.size() - 1;
    if (bRet)
    {
        ++m_nCurrIndex;
        m_recordList[m_nCurrIndex]->Do();
    }
    return bRet;
}

bool COperationRecordManager::UndoRecord()
{
    bool bRet = m_nCurrIndex >= 0;
    if (bRet)
    {
        BEATS_ASSERT(m_nCurrIndex < (int)m_recordList.size());
        m_recordList[m_nCurrIndex]->Undo();
        --m_nCurrIndex;
    }
    return bRet;
}

void COperationRecordManager::ClearRecord()
{
    for (uint32_t i = 0; i < m_recordList.size(); ++i)
    {
        EOperationRecordType type = m_recordList[i]->GetType();
        m_recordCache[type].push_back(m_recordList[i]);
    }
    m_recordList.clear();
    m_nCurrIndex = -1;
}