#ifndef BEYOND_ENGINE_RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#include "MathExt/Quaternion.h"
#include "MathExt/Mat4.h"

class CSkeletonAnimation;
class CSkeleton;

class CAnimationController
{
public:
    CAnimationController();
    ~CAnimationController();

    void SetSkeleton(CSkeleton* pSkeleton);
    CSkeletonAnimation* GetCurrentAnimation() const;
    void PlayAnimation(CSkeletonAnimation* pAnimation, float fBlendTime, bool bLoop);
    void GoToFrame(uint32_t frame);
    void SetAllowSkip(bool bAllow);
    void Pause();
    void Resume();
    void Stop();
    bool IsPlaying();
    uint32_t GetCurrFrame() const;
    float GetPlayingTime() const;
    void Update(float fDeltaTime);
    const std::map<uint8_t, CMat4*>& GetBlendDeltaMatrices() const;
    CMat4 GetBoneCurrWorldTM(uint8_t uBoneIndex) const;
    void BlendAnimation(const CSkeletonAnimation* pBeforeAnimation, const CSkeletonAnimation* pAfterAnimation);
    bool IsBlending() const;
    void SetLoop(bool bLoop);
    uint32_t GetLoopCount() const;
    bool IsLoop() const;

private:
    void Interpolation(const CMat4& startMat, const CMat4& endMat,CMat4& insertMat);
    void UpdateBlendWorldTM(uint32_t type, const std::map<uint32_t, CMat4>& localMap);
    void CalcDeltaMatrices();
    CMat4* GetMat();

private:
    bool m_bLoop;
    bool m_bPlaying;
    bool m_bBlending;
    bool m_bAllowSkip; // indicate if the delta time is too much, should it skip the frame.
    uint32_t m_uLoopCount;
    float m_fPlayingTime;
    float m_fBlendTime;
    CSkeletonAnimation* m_pCurrentAnimation;
    CSkeletonAnimation* m_pPreAnimation;
    CSkeleton* m_pSkeleton;

    // it's the delta matrices from world TM of T-pos to current world TM.
    std::map<uint8_t, CMat4*> m_blendDeltaMatrices;

    // When we are blending animation, get bone's world TM from this map, otherwise, fetch the data in CSkeletonAnimation
    std::map<uint32_t, CMat4*> m_blendBonesWorldTMMap;

    std::vector<CMat4*> m_matPool;
    uint32_t m_matPoolIndex;

};

#endif