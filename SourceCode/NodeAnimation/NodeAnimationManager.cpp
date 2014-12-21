#include "stdafx.h"
#include "NodeAnimationManager.h"
#include "NodeAnimation.h"
#include "NodeAnimationData.h"
#include "Event/EventDispatcher.h"
#include "Scene/Node.h"

CNodeAnimationManager* CNodeAnimationManager::m_pInstance = NULL;

CNodeAnimationManager::CNodeAnimationManager()
{
    BEATS_ASSERT(!CApplication::GetInstance()->IsDestructing(), "Should not create singleton when exit the program!");
}

CNodeAnimationManager::~CNodeAnimationManager()
{
    for (std::set<CNodeAnimation*>::iterator iter = m_cacheList.begin(); iter != m_cacheList.end(); ++iter)
    {
        CNodeAnimation* pAnimation = *iter;
        BEATS_SAFE_DELETE(pAnimation);
    }
    while (m_animationDataMap.size() > 0)
    {
        CNodeAnimationData* pData = m_animationDataMap.begin()->second;
        BEATS_SAFE_DELETE_COMPONENT(pData);
    }
}

CNodeAnimation* CNodeAnimationManager::RequestNodeAnimation(ENodeAnimationPlayType playType/* = eNAPT_ONCE*/, uint32_t uFPS/* = 60*/, CNode* pOwner/* = nullptr*/, bool bAutoDestroy/* = false*/)
{
    CNodeAnimation* pRet = nullptr;
    if (m_cacheList.size() > 0)
    {
        std::unique_lock<std::mutex> locker(m_animationRequestMutex);
        pRet = *m_cacheList.begin();
        BEATS_ASSERT(pRet->m_bDeleteFlag);
        pRet->m_bDeleteFlag = false;
        BEATS_ASSERT(pRet->GetOwner() == nullptr && pRet->GetData() == nullptr && pRet->m_bDeleteFlag == false && pRet->m_bAutoDestroy == false && !pRet->IsPlaying())
        m_cacheList.erase(pRet);
    }
    else
    {
        pRet = new CNodeAnimation;
    }
    pRet->SetPlayType(playType);
    pRet->SetFPS(uFPS);
    pRet->SetOwner(pOwner);
    pRet->SetAutoDestroy(bAutoDestroy);
    return pRet;
}

void CNodeAnimationManager::DeleteNodeAnimation(CNodeAnimation* pNodeAnimation)
{
#ifndef DISABLE_NODE_ANIMATION
    if (pNodeAnimation)
    {
        if (pNodeAnimation->IsPlaying())
        {
            pNodeAnimation->Stop();
        }
        m_animationSetMutex.lock();
        m_animationSet.erase(pNodeAnimation);
        m_animationSetMutex.unlock();
        pNodeAnimation->Reset();
        {
            pNodeAnimation->m_bDeleteFlag = true;
            std::unique_lock<std::mutex> locker(m_animationRequestMutex);
            BEATS_ASSERT(m_cacheList.find(pNodeAnimation) == m_cacheList.end());
            m_cacheList.insert(pNodeAnimation);
        }
    }
#endif
}

void CNodeAnimationManager::RegisterNodeAnimation(CNodeAnimation* pAnimation)
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(pAnimation != NULL && pAnimation->GetOwner() != NULL && pAnimation->IsPlaying() && !pAnimation->m_bDeleteFlag);
#ifdef DEVELOP_VERSION
    if (!m_bEnable)
    {
        pAnimation->SetCurrentFrame(pAnimation->GetData()->GetFrameCount() - 1);
        pAnimation->TriggerEvent(eET_EVENT_NODE_ANIMATION_STOP);
        return;
    }
#endif
    m_animationSetMutex.lock();
    m_animationSet.insert(pAnimation);
    m_animationSetMutex.unlock();
#endif
}

void CNodeAnimationManager::RemoveNode(CNode* pNode)
{
#ifndef DISABLE_NODE_ANIMATION
    m_animationSetMutex.lock();
    std::set<CNodeAnimation*> animationBak = m_animationSet;
    m_animationSetMutex.unlock();
    for (auto iter = animationBak.begin(); iter != animationBak.end(); ++iter)
    {
        CNodeAnimation* pAnimation = (*iter);
        if (pAnimation->GetOwner() == pNode)
        {
            if (pAnimation->IsAutoDestroy())
            {
                DeleteNodeAnimation(pAnimation);
            }
            else
            {
                if (pAnimation->IsPlaying())
                {
                    pAnimation->Stop();
                }
                pAnimation->Reset();
                m_animationSetMutex.lock();
                m_animationSet.erase(pAnimation);
                m_animationSetMutex.unlock();
            }
        }
    }
#endif
}

void CNodeAnimationManager::ResetNode(CNode* pNode)
{
#ifndef DISABLE_NODE_ANIMATION
    m_animationSetMutex.lock();
    for (auto iter = m_animationSet.begin(); iter != m_animationSet.end(); ++iter)
    {
        CNodeAnimation* pAnimation = (*iter);
        if (pAnimation->GetOwner() == pNode)
        {
            pAnimation->ResetNode();
        }
    }
    m_animationSetMutex.unlock();
#endif
}

void CNodeAnimationManager::RegisterNodeAnimationData(CNodeAnimationData* pData)
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(!pData->GetName().empty(), _T("AnimationData must have a name!"));
    BEATS_ASSERT(pData != NULL, _T("AnimationData must not be null!"));
    BEATS_ASSERT(m_animationDataMap.find(pData->GetName()) == m_animationDataMap.end(), _T("Can't register animation data %s, this name already exists!\n Id: %d %d"), pData->GetName().c_str(), pData->GetId(), m_animationDataMap[pData->GetName()]->GetId());
    m_animationDataMap[pData->GetName()] = pData;
#endif
}

void CNodeAnimationManager::UnregisterNodeAnimationData(CNodeAnimationData* pData)
{
    BEATS_ASSERT(pData != NULL, _T("AnimationData must not be null!"));
    BEATS_ASSERT(!pData->GetName().empty(), _T("AnimationData must have a name!"));
    BEATS_ASSERT(m_animationDataMap.find(pData->GetName()) != m_animationDataMap.end(), _T("Can't unregister animation data %s, this name doesn't exist!"), pData->GetName().c_str());
    m_animationDataMap.erase(pData->GetName());
}

CNodeAnimationData* CNodeAnimationManager::GetNodeAnimationData(const TString& strName) const
{
    CNodeAnimationData* pRet = NULL;
    auto iter = m_animationDataMap.find(strName);
    if (iter != m_animationDataMap.end())
    {
        pRet = iter->second;
    }
    return pRet;
}

void CNodeAnimationManager::Update(float ddt)
{
#ifndef DISABLE_NODE_ANIMATION
#ifdef DEVELOP_VERSION
    if (!m_bEnable)
    {
        return;
    }
#endif
    m_animationSetMutex.lock();
    std::set<CNodeAnimation*> animationBak = m_animationSet;
    m_animationSetMutex.unlock();
    for (auto iter = animationBak.begin(); iter != animationBak.end(); ++iter)
    {
        CNodeAnimation* pAnimation = *iter;
        if (pAnimation->IsPlaying() && !pAnimation->m_bDeleteFlag)
        {
            pAnimation->Update(ddt);
        }
        // Don't use else instead the if.
        if (!pAnimation->IsPlaying())
        {
            // TODO: I think this event should be moved in the stop function.
            pAnimation->TriggerEvent(eET_EVENT_NODE_ANIMATION_STOP);
            if (!pAnimation->m_bDeleteFlag)
            {
                if (pAnimation->IsAutoDestroy())
                {
                    DeleteNodeAnimation(pAnimation);
                }
                else
                {
                    m_animationSetMutex.lock();
                    BEATS_ASSERT(m_animationSet.find(pAnimation) != m_animationSet.end());
                    m_animationSet.erase(pAnimation);
                    m_animationSetMutex.unlock();
                }
            }
        }
    }
#endif
}

CNodeAnimation* CNodeAnimationManager::GetNodeAnimation(CNode* pNode, const TString& strName)
{
    CNodeAnimation* pRet = NULL;
    BEATS_ASSERT(pNode != nullptr);
    m_animationSetMutex.lock();
    for (auto iter = m_animationSet.begin(); iter != m_animationSet.end(); ++iter)
    {
        CNodeAnimation* pNodeAnimation = *iter;
        if (pNodeAnimation->GetOwner() == pNode)
        {
            if (strName.empty() || pNodeAnimation->GetData()->GetName() == strName)
            {
                pRet = pNodeAnimation;
                break;
            }
        }
    }
    m_animationSetMutex.unlock();
    return pRet;
}

std::vector<CNodeAnimation*> CNodeAnimationManager::GetNodeAnimations(CNode* pNode)
{
    std::vector<CNodeAnimation*> list;
    BEATS_ASSERT(pNode != nullptr);
    m_animationSetMutex.lock();
    for (auto iter = m_animationSet.begin(); iter != m_animationSet.end(); ++iter)
    {
        CNodeAnimation* pNodeAnimation = *iter;
        if (pNodeAnimation->GetOwner() == pNode)
        {
            list.push_back(pNodeAnimation);
        }
    }
    m_animationSetMutex.unlock();
    return list;
}

void CNodeAnimationManager::PlayNodeAnimation(CNode* pNode, const TString& strAnimationName, uint32_t uStartFramePos /*= 0*/, ENodeAnimationPlayType type /*= eNAPT_ONCE*/, bool bResetNodeWhenStop /*= false*/)
{
#ifndef DISABLE_NODE_ANIMATION
#ifdef DEVELOP_VERSION
    if (!m_bEnable)
    {
        return;
    }
#endif
    CNodeAnimation* pAnimation = GetNodeAnimation(pNode, strAnimationName);
    if (pAnimation)
    {
        pAnimation->ResetNode();
    }
    else
    {
        CNodeAnimationData* pData = GetNodeAnimationData(strAnimationName);
        BEATS_ASSERT(pData != NULL);
        pAnimation = RequestNodeAnimation(type, 60, pNode, true);
        pAnimation->SetData(pData);
    }
    pAnimation->SetResetNodeWhenStop(bResetNodeWhenStop);
    pAnimation->Play(uStartFramePos);
#endif
}

void CNodeAnimationManager::StopNodeAnimation(CNode* pNode, const TString& strAnimationName, bool bResetNode /*= false*/)
{
#ifndef DISABLE_NODE_ANIMATION
    CNodeAnimation* pAnimation = GetNodeAnimation(pNode, strAnimationName);
    if (pAnimation != NULL)
    {
        if (bResetNode)
        {
            pAnimation->ResetNode();
        }
        pAnimation->Stop();
    }
#endif
}

void CNodeAnimationManager::StopNodeAnimation(const TString& strName)
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(!strName.empty());
    m_animationSetMutex.lock();
    std::set<CNodeAnimation*> bak = m_animationSet;
    for (auto iter = bak.begin(); iter != bak.end(); ++iter)
    {
        BEATS_ASSERT((*iter)->GetData());
        // TODO: figure out why sometimes the data will be empty(delete flag is true)
        if ((*iter)->GetData()->GetName() == strName)
        {
            DeleteNodeAnimation(*iter);
        }
    }
    m_animationSetMutex.unlock();
#endif
}