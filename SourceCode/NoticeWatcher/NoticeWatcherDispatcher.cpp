#include "stdafx.h"
#include "NoticeWatcherDispatcher.h"
#include "WatchNode.h"

CNoticeWatcherDispatcher* CNoticeWatcherDispatcher::m_pInstance = nullptr;
CNoticeWatcherDispatcher::CNoticeWatcherDispatcher()
{

}

CNoticeWatcherDispatcher::~CNoticeWatcherDispatcher()
{

}

void CNoticeWatcherDispatcher::RegisterWatchNode(CWatchNode* pWatchNode, const std::vector<TString>& strNoticeTags)
{
    for (const auto& strNoticeTag : strNoticeTags)
    {
        RegisterWatchNode(pWatchNode, strNoticeTag);
    }
}

void CNoticeWatcherDispatcher::RegisterWatchNode(CWatchNode* pWatchNode, const TString& strNoticeTag)
{
    auto& pool = m_noticeWatcherPool[strNoticeTag];
    if (std::find(pool.begin(), pool.end(), pWatchNode) == pool.end())
    {
        pool.push_back(pWatchNode);
        pWatchNode->Watched();
        //check the notice be dispatched
        auto iter = m_currentNoticeStack.find(strNoticeTag);
        if (iter != m_currentNoticeStack.end())
        {
            pWatchNode->ReceiveNotice(iter->second);
        }
    }
}

void CNoticeWatcherDispatcher::UnRegisterWatchNode(CWatchNode* pWatchNode)
{
    if (pWatchNode)
    {
        for (auto& pools : m_noticeWatcherPool)
        {
            for (auto iter = pools.second.begin(); iter != pools.second.end(); ++iter)
            {
                if (*iter == pWatchNode)
                {
                    pWatchNode->CompleteNotice(pWatchNode->GetWatchCount());
                    pools.second.erase(iter);
                    break;
                }
            }
        }
    }
}

void CNoticeWatcherDispatcher::DispatchNotice(const TString& strNoticeTag, uint32_t uDispatchCount, bool bIsOnlyNotice)
{
    bool canDispatch = true;
    if (bIsOnlyNotice && GetTagCount(strNoticeTag) > 0)
    {
        canDispatch = false;
    }

    if (canDispatch)
    {
        auto iter = m_currentNoticeStack.find(strNoticeTag);
        if (iter == m_currentNoticeStack.end())
        {
            m_currentNoticeStack[strNoticeTag] = uDispatchCount;
        }
        else
        {
            iter->second += uDispatchCount;
        }
        auto notice = m_noticeWatcherPool.find(strNoticeTag);
        if (notice != m_noticeWatcherPool.end())
        {
            for (auto watchNode : notice->second)
            {
                watchNode->ReceiveNotice(uDispatchCount);
            }
        }
    }
}

void CNoticeWatcherDispatcher::NoticeComplete(const TString& strNoticeTag)
{
    auto iter = m_currentNoticeStack.find(strNoticeTag);
    if (iter != m_currentNoticeStack.end())
    {
        iter->second--;
        if (iter->second == 0)
        {
            m_currentNoticeStack.erase(iter);
        }
        auto notice = m_noticeWatcherPool.find(strNoticeTag);
        if (notice != m_noticeWatcherPool.end())
        {
            for (auto watchNode : notice->second)
            {
                watchNode->CompleteNotice();
            }
        }
    }
}

void CNoticeWatcherDispatcher::ResetNoticeCount(const TString& strNoticeTag)
{
    auto iter = m_currentNoticeStack.find(strNoticeTag);
    if (iter != m_currentNoticeStack.end())
    {
        auto notice = m_noticeWatcherPool.find(strNoticeTag);
        if (notice != m_noticeWatcherPool.end())
        {
            for (auto watchNode : notice->second)
            {
                watchNode->CompleteNotice(iter->second);
            }
        }
        iter->second = 0;
        m_currentNoticeStack.erase(strNoticeTag);
    }
}

void CNoticeWatcherDispatcher::ResetAll()
{
    auto it = m_currentNoticeStack.begin();
    for (; it != m_currentNoticeStack.end(); it++)
    {
        auto notice = m_noticeWatcherPool.find(it->first);
        if (notice != m_noticeWatcherPool.end())
        {
            for (auto watchNode : notice->second)
            {
                watchNode->CompleteNotice(it->second);
            }
        }
    }
    m_currentNoticeStack.clear();
    m_noticeWatcherPool.clear();
}

uint32_t CNoticeWatcherDispatcher::GetTagCount(const TString& strNoticeTag)
{
    uint32_t count = 0;
    auto iter = m_currentNoticeStack.find(strNoticeTag);
    if (iter != m_currentNoticeStack.end())
    {
        count = iter->second;
    }
    return count;
}

std::map<TString, uint32_t> CNoticeWatcherDispatcher::GetNodeWatchTarget(CWatchNode* pNode)
{
    std::map<TString, uint32_t>nameList;
    BEATS_ASSERT(pNode);
    auto notice = m_noticeWatcherPool.begin();
    for (; notice != m_noticeWatcherPool.end(); notice++)
    {
        uint32_t count = 0;
        auto countIt = m_currentNoticeStack.find(notice->first);
        if (countIt != m_currentNoticeStack.end() && countIt->second > 0)
        {
            count = countIt->second;
        }
        for (auto watchNode : notice->second)
        {
            if (watchNode == pNode)
            {
                nameList[notice->first] = count;
            }
        }
    }
    return nameList;
}

void CNoticeWatcherDispatcher::PrintAllWatchCount()
{
    for (auto it = m_currentNoticeStack.begin(); it != m_currentNoticeStack.end(); it++)
    {
        if (it->second > 0)
        {
            BEATS_PRINT("watch target = %s, count = %d \n", it->first.c_str(), it->second);
        }
    }
}
