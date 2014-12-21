#include "stdafx.h"
#ifdef EDITOR_MODE
#include "PerformDetector.h"
#include <mmsystem.h>

CPerformDetector* CPerformDetector::m_pInstance = NULL;

CPerformDetector::CPerformDetector()
: m_pCurRecord(NULL)
, m_bPause(false)
, m_bPauseRequest(false)
, m_bClearAllRequest(false)
{
    QueryPerformanceFrequency(&m_freq);
}

CPerformDetector::~CPerformDetector()
{
    DestroyResultPool();
    std::map<uint32_t, SPerformanceRecord*>::iterator iter = m_recordMap.begin();
    for (; iter != m_recordMap.end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    m_recordMap.clear();
}

void CPerformDetector::StartDetectNode( int type, uint32_t id )
{
    if (!m_bPause)
    {
        if (m_pCurRecord == NULL)
        {
            m_pCurRecord = &m_rootRecord;
        }
        //try to check if this record exists, if no, create a new one.
        std::map<uint32_t, SPerformanceRecord*>::iterator iter = m_recordMap.find(id);
        SPerformanceRecord* pRecord = NULL;
        if (iter == m_recordMap.end())
        {
            pRecord = new SPerformanceRecord(id, type);
            pRecord->typeStr = GetTypeName(type);
            m_pCurRecord->children.push_back(pRecord);//for relationship management
            pRecord->pParent = m_pCurRecord;
            m_recordMap[id] = pRecord;//for look up
        }
        else
        {
            pRecord = iter->second;
            // A new parent node appeared! reset the relationship!
            if (pRecord->pParent != m_pCurRecord)
            {
                for( std::vector<SPerformanceRecord*>::iterator iter = pRecord->pParent->children.begin(); iter != pRecord->pParent->children.end(); ++iter)
                {
                    if (*iter == pRecord)
                    {
                        pRecord->pParent->children.erase(iter);
                        break;
                    }
                }
                pRecord->pParent = m_pCurRecord;
                m_pCurRecord->children.push_back(pRecord);
            }
        }
        m_pCurRecord = pRecord;
        SPerformanceResult* pResult = GetResultFromPool();
        pResult->id = id;
        m_resultOrder.push(pResult);
        LARGE_INTEGER startCount;
        QueryPerformanceCounter(&startCount);
        pResult->startCount = startCount;
    }
}
float CPerformDetector::StopDetectNode( int type )
{
    BEYONDENGINE_UNUSED_PARAM(type);
    float elapsedTime = 0;
    if (!m_bPause)
    {
        BEATS_ASSERT(m_resultOrder.size() > 0);
        SPerformanceResult* pLastRecordResult = m_resultOrder.top();
        BEATS_ASSERT(pLastRecordResult != nullptr);
        LARGE_INTEGER nowCount;
        QueryPerformanceCounter(&nowCount);
        elapsedTime = ((float)((nowCount.QuadPart - pLastRecordResult->startCount.QuadPart) * 1000) / m_freq.QuadPart);
        m_resultOrder.pop();
        BEATS_ASSERT(m_pCurRecord != NULL && m_pCurRecord->type == type
            , _T("Stop performance detect : %s without start!"), GetTypeName(type));

        pLastRecordResult->resultMS = elapsedTime;
        UpdateRecord(pLastRecordResult);
        m_pCurRecord = m_pCurRecord->pParent;
    }
    return elapsedTime;
}

void CPerformDetector::ClearFrameResult()
{
    m_pCurRecord = NULL;
    m_resultPool.insert(m_resultPool.end(), m_resultThisFrame.begin(), m_resultThisFrame.end());
    m_resultThisFrame.clear();
}

SPerformanceResult* CPerformDetector::GetResultFromPool()
{
    SPerformanceResult* pResult = NULL;
    if (m_resultPool.size() > 0)
    {
        pResult = m_resultPool.back();
        m_resultPool.pop_back();
    }
    else
    {
        pResult = new SPerformanceResult;
    }
    m_resultThisFrame.push_back(pResult);
    return pResult;
}

void CPerformDetector::DestroyResultPool()
{
    BEATS_SAFE_DELETE_VECTOR(m_resultPool);
}

void CPerformDetector::SetTypeName( const TCHAR* typeStr[], uint32_t uCount )
{
    m_typeName.resize(0);
    for (uint32_t i = 0; i < uCount; ++i)
    {
        m_typeName.push_back(typeStr[i]);
    }    
}

const TCHAR* CPerformDetector::GetTypeName( int type )
{
    BEATS_ASSERT((uint32_t)type < m_typeName.size(), _T("Get Performance Type Error: out of bound for string name"));
    const TCHAR* result = m_typeName[type];
    return result;
}

void CPerformDetector::ResetFrameResult()
{
    BEATS_ASSERT(ePNT_Count == sizeof(pszPerformNodeStr) / sizeof(void*));
    if (!m_bPause)
    {
        std::vector<SPerformanceResult*> result;
        GetResultThisFrame(result);
        ClearFrameResult();
    }
    if (m_bPauseRequest)
    {
        m_bPauseRequest = false;
        m_bPause = !m_bPause;
    }
    if (m_bClearAllRequest)
    {
        m_bClearAllRequest = false;
        std::map<uint32_t, SPerformanceRecord*>::iterator iter = m_recordMap.begin();
        for (; iter != m_recordMap.end(); ++iter)
        {
            iter->second->Reset();
        }
    }
}

void CPerformDetector::UpdateRecord( SPerformanceResult* pResult )
{
    BEATS_ASSERT(m_pCurRecord->id == pResult->id);
    m_pCurRecord->totalValueMS += pResult->resultMS;
    if (pResult->resultMS > m_pCurRecord->maxValueMS)
    {
        m_pCurRecord->maxValueMS = pResult->resultMS;
    }
    ++m_pCurRecord->updateCount;
}

void CPerformDetector::SetPauseFlag(bool bValue)
{
    m_bPause = bValue;
}

void CPerformDetector::GetResultThisFrame( std::vector<SPerformanceResult*>& outResult )
{
    outResult = m_resultThisFrame;
}

SPerformanceRecord* CPerformDetector::GetRecord( uint32_t id )
{
    BEATS_ASSERT(id == 0 || m_recordMap.find(id) != m_recordMap.end(), _T("Can't get record with id %d"), id);
    return id == 0 ? &m_rootRecord : m_recordMap[id];
}

bool CPerformDetector::PauseSwitcher()
{
    m_bPauseRequest = true;
    return !m_bPause;
}

bool CPerformDetector::IsPaused()
{
    return m_bPause;
}

void CPerformDetector::ClearAllResult()
{
    m_bClearAllRequest= true;
}

const std::map<uint32_t, SPerformanceRecord*>& CPerformDetector::GetRecordMap() const
{
    return m_recordMap;
}

#endif