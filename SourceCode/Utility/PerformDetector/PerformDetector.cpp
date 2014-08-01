#include "stdafx.h"
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
#include "PerformDetector.h"
#include <mmsystem.h>

CPerformDetector* CPerformDetector::m_pInstance = NULL;

CPerformDetector::CPerformDetector()
: m_resultPoolIndex(0)
, m_lastResultPoolIndex(0)
, m_pCurRecord(NULL)
, m_bPause(false)
, m_bPauseRequest(false)
, m_bClearAllRequest(false)
{
    QueryPerformanceFrequency(&m_freq);
    IncreaseResultPool();
}

CPerformDetector::~CPerformDetector()
{
    DestroyResultPool();
    std::map<size_t, SPerformanceRecord*>::iterator iter = m_recordMap.begin();
    for (; iter != m_recordMap.end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    m_recordMap.clear();
}

void CPerformDetector::StartDetectNode( int type, size_t id )
{
    if (!m_bPause)
    {
        if (m_pCurRecord == NULL)
        {
            m_pCurRecord = &m_rootRecord;
        }
        //try to check if this record exists, if no, create a new one.
        std::map<size_t, SPerformanceRecord*>::iterator iter = m_recordMap.find(id);
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

        LARGE_INTEGER startCount;
        QueryPerformanceCounter(&startCount);
        SPerformanceResult* pResult = GetResultFromPool();
        pResult->startCount = startCount;
        pResult->id = id;
        m_resultOrder.push(pResult);
    }
}
float CPerformDetector::StopDetectNode( int type )
{
    float elapsedTime = 0;
    if (!m_bPause)
    {
        SPerformanceResult* pLastRecordResult = m_resultOrder.top();
        m_resultOrder.pop();
        BEATS_ASSERT(m_pCurRecord != NULL && m_pCurRecord->type == type
            , _T("Stop performance detect : %s without start!"), GetTypeName(type));
        LARGE_INTEGER nowCount;
        QueryPerformanceCounter(&nowCount);
        elapsedTime = ((float)((nowCount.QuadPart - pLastRecordResult->startCount.QuadPart) * 1000) / m_freq.QuadPart);

        pLastRecordResult->result = elapsedTime;
        UpdateRecord(pLastRecordResult);
        m_pCurRecord = m_pCurRecord->pParent;
    }
    return elapsedTime;
}

void CPerformDetector::ClearFrameResult()
{
    m_pCurRecord = NULL;
    m_lastResultPoolIndex = m_resultPoolIndex;
}

SPerformanceResult* CPerformDetector::GetResultFromPool()
{
    if (m_resultPoolIndex >= m_resultPool.size())
    {
        m_resultPoolIndex = 0;
    }
    SPerformanceResult* pResult = m_resultPool[m_resultPoolIndex++];
    return pResult;
}

void CPerformDetector::IncreaseResultPool()
{
    const int defaultResultPoolSize = 3000;
    m_resultPool.reserve(m_resultPool.size() + defaultResultPoolSize);
    for (int i = 0; i < defaultResultPoolSize; ++i)
    {
        m_resultPool.push_back(new SPerformanceResult());
    }
}

void CPerformDetector::DestroyResultPool()
{
    BEATS_SAFE_DELETE_VECTOR(m_resultPool);
}

void CPerformDetector::SetTypeName( const TCHAR* typeStr[], size_t uCount )
{
    m_typeName.resize(0);
    for (size_t i = 0; i < uCount; ++i)
    {
        m_typeName.push_back(typeStr[i]);
    }    
}

const TCHAR* CPerformDetector::GetTypeName( int type )
{
    BEATS_ASSERT((size_t)type < m_typeName.size(), _T("Get Performance Type Error: out of bound for string name"));
    const TCHAR* result = m_typeName[type];
    return result;
}

void CPerformDetector::ResetFrameResult()
{
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
        std::map<size_t, SPerformanceRecord*>::iterator iter = m_recordMap.begin();
        for (; iter != m_recordMap.end(); ++iter)
        {
            iter->second->Reset();
        }
    }
}

void CPerformDetector::UpdateRecord( SPerformanceResult* pResult )
{
    m_pCurRecord->totalValue += pResult->result;
    if (pResult->result > m_pCurRecord->maxValue)
    {
        m_pCurRecord->maxValue = pResult->result;
    }
    ++m_pCurRecord->updateCount;
}

void CPerformDetector::GetResultThisFrame( std::vector<SPerformanceResult*>& outResult )
{
    outResult.resize(0);
    if (m_lastResultPoolIndex > m_resultPoolIndex)
    {
        for (size_t i = m_lastResultPoolIndex; i < m_resultPool.size(); ++i)
        {
            outResult.push_back(m_resultPool[i]);
        }
        for (size_t i = 0; i < m_resultPoolIndex; ++i)
        {
            outResult.push_back(m_resultPool[i]);
        }
    }
    else
    {
        for (size_t i = m_lastResultPoolIndex; i < m_resultPoolIndex; ++i)
        {
            outResult.push_back(m_resultPool[i]);
        }
    }
}

SPerformanceRecord* CPerformDetector::GetRecord( size_t id )
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
#endif