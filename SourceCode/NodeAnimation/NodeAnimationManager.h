#ifndef BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONMANAGER_H__INCLUDE
#define BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONMANAGER_H__INCLUDE

class CNodeAnimation;
class CNode;
class CNodeAnimationData;
class CNodeAnimationManager
{
    BEATS_DECLARE_SINGLETON(CNodeAnimationManager);
public:
    void RegisterNodeAnimation(CNodeAnimation *pAnimation);
    void RemoveNodeAnimation(CNode* pNode, CNodeAnimation* pNodeAnimation = NULL);

    void RegisterNodeAnimationData(CNodeAnimationData* pData);
    void UnregisterNodeAnimationData(CNodeAnimationData* pData);
    CNodeAnimationData* GetNodeAnimationData(const TString& strName) const;
    void Update(float ddt);

private:
    std::map<CNode*, std::vector<CNodeAnimation*>> m_animations;
    std::map<TString, CNodeAnimationData*> m_animationDataMap;
};

#endif