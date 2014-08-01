#include "stdafx.h"
#include "NodeAnimationManager.h"
#include "NodeAnimation.h"
#include "NodeAnimationData.h"

CNodeAnimationManager* CNodeAnimationManager::m_pInstance = NULL;

CNodeAnimationManager::CNodeAnimationManager()
{

}

CNodeAnimationManager::~CNodeAnimationManager()
{

}

void CNodeAnimationManager::RegisterNodeAnimation(CNodeAnimation* pAnimation)
{
    CNode* pNode = pAnimation->GetOwner();
    BEATS_ASSERT(pNode != NULL);
    auto iter = m_animations.find(pNode);
    if (iter == m_animations.end())
    {
        m_animations[pNode] = std::vector<CNodeAnimation*>();
    }
#ifdef _DEBUG
    else
    {
        bool bExists = false;
        for (size_t i = 0; i < iter->second.size(); ++i)
        {
            if (iter->second.at(i) == pAnimation)
            {
                bExists = true;
                break;
            }
        }
        BEATS_ASSERT(!bExists, _T("Can't register the same animation twice!"));
    }
#endif // DEBUG
    m_animations[pNode].push_back(pAnimation);
}

void CNodeAnimationManager::RemoveNodeAnimation(CNode* pNode, CNodeAnimation* pNodeAnimation)
{
    BEATS_ASSERT(pNode != NULL);
    auto iter = m_animations.find(pNode);
    if (iter != m_animations.end())
    {
        for (size_t i = 0; i < iter->second.size(); ++i)
        {
            if (pNodeAnimation != NULL && iter->second[i] == pNodeAnimation)
            {
                iter->second[i] = iter->second.back();
                iter->second.pop_back();
                break;
            }
        }
        if (iter->second.size() == 0 || pNodeAnimation == NULL)
        {
            m_animations.erase(iter);
        }
    }
}

void CNodeAnimationManager::RegisterNodeAnimationData(CNodeAnimationData* pData)
{
    BEATS_ASSERT(!pData->GetName().empty(), _T("AnimationData must have a name!"));
    BEATS_ASSERT(pData != NULL, _T("AnimationData must not be null!"));
    BEATS_ASSERT(m_animationDataMap.find(pData->GetName()) == m_animationDataMap.end(), _T("Can't register animation data %s, this name already exists!"), pData->GetName().c_str());
    m_animationDataMap[pData->GetName()] = pData;
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
    BEATS_ASSERT(iter != m_animationDataMap.end());
    if (iter != m_animationDataMap.end())
    {
        pRet = iter->second;
    }
    return pRet;
}

void CNodeAnimationManager::Update(float ddt)
{
    for (auto iter = m_animations.begin(); iter != m_animations.end(); )
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end();)
        {
            CNodeAnimation* pAnimation = *subIter;
            bool bAnimationFinished = !pAnimation->IsPlaying();
            if (!bAnimationFinished)
            {
                pAnimation->Update(ddt);
                bAnimationFinished = !pAnimation->IsPlaying();
            }
            if (bAnimationFinished)
            {
                subIter = iter->second.erase(subIter);
            }
            else
            {
                ++subIter;
            }
        }
        if (iter->second.size() == 0)
        {
            iter = m_animations.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}
