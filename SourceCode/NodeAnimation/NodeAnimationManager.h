#ifndef BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONMANAGER_H__INCLUDE
#define BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONMANAGER_H__INCLUDE

#include "NodeAnimation.h"

class CNode;
class CNodeAnimationData;
class CNodeAnimationManager
{
    BEATS_DECLARE_SINGLETON(CNodeAnimationManager);
public:
    CNodeAnimation* RequestNodeAnimation(ENodeAnimationPlayType playType = eNAPT_ONCE, uint32_t uFPS = 60, CNode* pOwner = nullptr, bool bAutoDestroy = false);
    void DeleteNodeAnimation(CNodeAnimation* pNodeAnimation);

    void RegisterNodeAnimation(CNodeAnimation *pAnimation);
    void RemoveNode(CNode* pNode);
    void ResetNode(CNode* pNode);
    CNodeAnimation* GetNodeAnimation(CNode* pNode, const TString& strName);
    std::vector<CNodeAnimation*> GetNodeAnimations(CNode* pNode);
    void RegisterNodeAnimationData(CNodeAnimationData* pData);
    void UnregisterNodeAnimationData(CNodeAnimationData* pData);
    CNodeAnimationData* GetNodeAnimationData(const TString& strName) const;
    void Update(float ddt);
    void PlayNodeAnimation(CNode* pNode, const TString& strAnimationName, uint32_t uStartFramePos = 0, ENodeAnimationPlayType type = eNAPT_ONCE, bool bResetNodeWhenStop = false);
    void StopNodeAnimation(CNode* pNode, const TString& strAnimationName, bool bResetNode = false);
    void StopNodeAnimation(const TString& strName);

#ifdef DEVELOP_VERSION
    bool m_bEnable = true;
#endif
private:
    std::mutex m_animationSetMutex;
    std::set<CNodeAnimation*> m_animationSet;
    std::map<TString, CNodeAnimationData*> m_animationDataMap;
    std::mutex m_animationRequestMutex;
    std::set<CNodeAnimation*> m_cacheList;
};

#endif