#ifndef BEYOND_ENGINE_SKELETONANIMATION_H__INCLUDE
#define BEYOND_ENGINE_SKELETONANIMATION_H__INCLUDE
#include "MathExt/Location.h"
#include "MathExt/AABBBox.h"
class CSkeleton;
class CSkeletonBone;
class CSerializer;
class CSkeletonAnimation
{
public:
    CSkeletonAnimation(CSkeleton* pSkeleton);
    virtual ~CSkeletonAnimation();
    
    bool Load(CSerializer& serializer);

    float GetDuration() const;
    std::map<uint8_t, CMat4> GetDeltaTM(float playingTime);
    CAABBBox GetAABBBox(float fPlayTime);

    uint8_t GetFPS() const;
    uint16_t GetFrameCount() const;
    void SetCurFrame(uint32_t curFrame);
    uint32_t GetCurFrame() const;
    void SetSkeleton(CSkeleton* pSkeleton);

    const TString& GetAniName() const;

private:
    const CMat4& GetWorldTMByTime(CSkeletonBone* pBone, float fPlayingTime);

private:
    uint16_t m_uFrameCount;
    uint16_t m_uStartFrame = 0;
    uint32_t m_uCurFrame;
    CSkeleton* m_pSkeleton = nullptr;
    TString m_strAniName;
    std::map<uint8_t, std::map<uint16_t, CLocation> > m_boneLocationMap; // key: boneIndex; key: frame index, value:location.
    float m_fCurrentCacheTime = FLT_MAX;
    std::map<uint8_t, CMat4> m_cacheWorldTM;
};

#endif