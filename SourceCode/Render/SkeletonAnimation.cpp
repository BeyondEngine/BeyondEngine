#include "stdafx.h"
#include "SkeletonAnimation.h"
#include "SkeletonBone.h"
#include "Skeleton.h"

CSkeletonAnimation::CSkeletonAnimation(CSkeleton* pSkeleton)
    : m_uFrameCount(0)
    , m_uCurFrame(0)
    , m_pSkeleton(pSkeleton)
{

}

CSkeletonAnimation::~CSkeletonAnimation()
{
}

float CSkeletonAnimation::GetDuration() const
{
    return (float)m_uFrameCount / m_pSkeleton->GetFPS();
}

std::map<uint8_t, CMat4> CSkeletonAnimation::GetDeltaTM(float playingTime)
{
    playingTime = BEATS_FMOD(playingTime, GetDuration());
    std::map<uint8_t, CMat4> ret;
    const std::map<uint8_t, CSkeletonBone* >& boneMap = m_pSkeleton->GetBoneMap();
    for (auto boneIter = boneMap.begin(); boneIter != boneMap.end(); ++boneIter)
    {
        CSkeletonBone* pSkeletonBone = boneIter->second;
        BEATS_ASSERT(boneIter->first == pSkeletonBone->GetIndex());
        const CMat4& worldTM = GetWorldTMByTime(pSkeletonBone, playingTime);
        CMat4 tPosMatrixInverse = pSkeletonBone->GetTPosWorldTM();
        tPosMatrixInverse.Inverse();
        ret[pSkeletonBone->GetIndex()] = worldTM * tPosMatrixInverse;
    }
    return ret;
}

CAABBBox CSkeletonAnimation::GetAABBBox(float fPlayTime)
{
    CAABBBox ret;
    fPlayTime = BEATS_FMOD(fPlayTime, GetDuration());
    const std::map<uint8_t, CSkeletonBone* >& boneMap = m_pSkeleton->GetBoneMap();
    for (auto boneIter = boneMap.begin(); boneIter != boneMap.end(); ++boneIter)
    {
        CSkeletonBone* pSkeletonBone = boneIter->second;
        BEATS_ASSERT(boneIter->first == pSkeletonBone->GetIndex());
        const CMat4& worldTM = GetWorldTMByTime(pSkeletonBone, fPlayTime);
        ret.BuildWithPos(worldTM.GetTranslate());
    }
    return ret;
}

uint8_t CSkeletonAnimation::GetFPS() const
{
    return m_pSkeleton->GetFPS();
}

bool CSkeletonAnimation::Load(CSerializer& serializer)
{
    bool bRet = true;
    serializer >> m_uFrameCount;
    serializer >> m_strAniName;
    serializer >> m_uStartFrame;
    m_boneLocationMap.clear();
    for (auto iter = m_pSkeleton->GetBoneMap().begin(); iter != m_pSkeleton->GetBoneMap().end(); ++iter)
    {
        uint32_t uKeySize = 0;
        serializer >> uKeySize;
        if (uKeySize > 0)
        {
            std::map<uint16_t, CLocation>& keyFrameMap = m_boneLocationMap[iter->first];
            for (uint32_t i = 0; i < uKeySize; ++i)
            {
                uint16_t keyValue = 0;
                serializer >> keyValue;
                CLocation& location = keyFrameMap[keyValue];

                serializer >> location.m_scale;
                serializer >> location.m_rotation;
                serializer >> location.m_pos;
            }
        }
    }
    return bRet;
}

uint16_t CSkeletonAnimation::GetFrameCount() const
{
    return m_uFrameCount;
}

void CSkeletonAnimation::SetCurFrame(uint32_t curFrame)
{
   m_uCurFrame = curFrame;
}

uint32_t CSkeletonAnimation::GetCurFrame() const
{
    return m_uCurFrame;
}

void CSkeletonAnimation::SetSkeleton(CSkeleton* pSkeleton)
{
    if (m_pSkeleton != pSkeleton)
    {
        m_pSkeleton = pSkeleton;
    }
}

const CMat4& CSkeletonAnimation::GetWorldTMByTime(CSkeletonBone* pBone, float fPlayingTime)
{
    if (BEATS_FLOAT_EQUAL(fPlayingTime, m_fCurrentCacheTime))
    {
        auto iter = m_cacheWorldTM.find(pBone->GetIndex());
        if (iter != m_cacheWorldTM.end())
        {
            return iter->second;
        }
    }
    else
    {
        m_fCurrentCacheTime = fPlayingTime;
        m_cacheWorldTM.clear();
    }
    CMat4& ret = m_cacheWorldTM[pBone->GetIndex()];
    auto iter = m_boneLocationMap.find(pBone->GetIndex());
    if (iter == m_boneLocationMap.end() || iter->second.size() == 0)
    {
        ret = pBone->GetTPosWorldTM();
    }
    else
    {
        float fElapsedFrame = fPlayingTime * m_pSkeleton->GetFPS();
        if (fElapsedFrame > 1.0f) //Convert from frame count to frame index.
        {
            fElapsedFrame -= 1.0f;
        }
        float fCurrFrame = fElapsedFrame + m_uStartFrame;
        CLocation* pPreLocation = nullptr;
        uint16_t uPreFrame = 0;
        CLocation locationRet = iter->second.rbegin()->second; // By default we get the last location as result. Since if current frame is greater than the last key frame.
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            if (BEATS_FLOAT_LESS_EQUAL_EPSILON(fCurrFrame, subIter->first, 0.01f))
            {
                if (pPreLocation == nullptr)
                {
                    locationRet = subIter->second;
                }
                else
                {
                    float fProgress = ((float)(fCurrFrame - uPreFrame)) / (subIter->first - uPreFrame);
                    BEATS_CLIP_VALUE(fProgress, 0, 1);
                    locationRet = pPreLocation->Lerp(subIter->second, fProgress);
                }
                break;
            }
            else
            {
                pPreLocation = &subIter->second;
                uPreFrame = subIter->first;
            }
        }
        ret = CMat4(locationRet);
        if (pBone->GetParent() != nullptr)
        {
            ret = GetWorldTMByTime(pBone->GetParent(), fPlayingTime) * ret;
        }
    }
    return ret;
}

const TString& CSkeletonAnimation::GetAniName() const
{
    return m_strAniName;
}
