#include "stdafx.h"
#include "SkeletonAnimation.h"
#include "AnimationController.h"
#include "Skeleton.h"
#include "SkeletonBone.h"

CAnimationController::CAnimationController()
    : m_fPlayingTime(0.0f)
    , m_bLoop(false)
    , m_bPlaying(false)
    , m_bBlending(false)
    , m_bAllowSkip(true)
    , m_fBlendTime(1.0f)
    , m_uLoopCount(0)
    , m_matPoolIndex(0)
    , m_pCurrentAnimation(NULL)
    , m_pPreAnimation(NULL)
    , m_pSkeleton(NULL)
{
}

CAnimationController::~CAnimationController()
{
    BEATS_SAFE_DELETE_VECTOR(m_matPool);
}

void CAnimationController::SetSkeleton(CSkeleton* pSkeleton)
{
    m_pSkeleton = pSkeleton;
}

CSkeletonAnimation* CAnimationController::GetCurrentAnimation() const
{
    return m_pCurrentAnimation;
}

void CAnimationController::PlayAnimation(CSkeletonAnimation* pAnimation, float fBlendTime, bool bLoop)
{
    if (m_pCurrentAnimation && m_pCurrentAnimation != pAnimation)
    {
        m_pPreAnimation = m_pCurrentAnimation;
        uint32_t currFrame = GetCurrFrame();
        currFrame %= m_pCurrentAnimation->GetFrameCount();
        m_pPreAnimation->SetCurFrame(currFrame);
    }

    m_pCurrentAnimation = pAnimation;
    m_pCurrentAnimation->SetSkeleton(m_pSkeleton);
    m_bLoop = bLoop;
    m_fPlayingTime = 0;
    m_uLoopCount = 0;
    m_bPlaying = true;
    m_fBlendTime = fBlendTime;
}

void CAnimationController::GoToFrame( uint32_t frame )
{
    if(m_pCurrentAnimation)
    {
        BEATS_ASSERT(m_pCurrentAnimation->GetFPS() > 0);
        m_fPlayingTime = (float)frame / m_pCurrentAnimation->GetFPS();
        m_bPlaying = false;
        m_bBlending = false;
        CalcDeltaMatrices();
    }
    m_pPreAnimation = NULL;
}

void CAnimationController::SetAllowSkip(bool bAllow)
{
    m_bAllowSkip = bAllow;
}

void CAnimationController::Pause()
{
    m_bPlaying = false;
}

void CAnimationController::Resume()
{
    if(m_pCurrentAnimation)
    {
        m_bPlaying = true;
    }
}

void CAnimationController::Stop()
{
    GoToFrame(0);
}

bool CAnimationController::IsPlaying()
{
    return m_bPlaying;
}

uint32_t CAnimationController::GetCurrFrame() const
{
    uint32_t uRet = 0xFFFFFFFF;
    if(m_pCurrentAnimation)
    {
        uRet = static_cast<uint32_t>(m_fPlayingTime * m_pCurrentAnimation->GetFPS());
    }
    return uRet;
}

float CAnimationController::GetPlayingTime() const
{
    return m_fPlayingTime;
}

void CAnimationController::Update(float fDeltaTime)
{
    if(m_bPlaying && m_pCurrentAnimation && m_pSkeleton)
    {
        if (!m_bAllowSkip)
        {
            float fMaxDeltaTime = 1.0f / m_pCurrentAnimation->GetFPS();
            if (fDeltaTime > fMaxDeltaTime)
            {
                fDeltaTime = fMaxDeltaTime;
            }
        }
        m_fPlayingTime += fDeltaTime;
        float fDuration = m_pCurrentAnimation->GetDuration();
        BEATS_ASSERT(fDuration > 0);
        if (m_fPlayingTime > fDuration)
        {
            if(m_bLoop)
            {
                //TODO: HACK: If you enable all codes below, you will find some times fmodf will return invalid result. Don't know why, so I have to do my own calculation.
                //float fTemp = 0;
                //BEATS_ASSERT(!isinf(m_fPlayingTime) && !isnan(m_fPlayingTime));
                //fTemp = fmodf(m_fPlayingTime, fDuration);
                //BEATS_ASSERT(!isinf(fTemp) && !isnan(fTemp));
                //BEATS_ASSERT(!isinf(m_fPlayingTime) && !isnan(m_fPlayingTime));
                //m_fPlayingTime = fTemp;
                //BEATS_ASSERT(!isinf(m_fPlayingTime) && !isnan(m_fPlayingTime));
                m_fPlayingTime = BEATS_FMOD(m_fPlayingTime, fDuration);
                m_uLoopCount++;
            }
            else
            {
                m_fPlayingTime = fDuration;
                m_bPlaying = false;
            }
        }
        m_bBlending = m_pPreAnimation && m_fPlayingTime < m_fBlendTime && 0 == m_uLoopCount;

        BEYONDENGINE_PERFORMDETECT_START(ePNT_AnimationManager_CalcMatrix)
        CalcDeltaMatrices();
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_AnimationManager_CalcMatrix)
    }
}

bool CAnimationController::IsBlending() const
{
    return m_bBlending;
}

void CAnimationController::SetLoop(bool bLoop)
{
    m_bLoop = bLoop;
}

uint32_t CAnimationController::GetLoopCount() const
{
    return m_uLoopCount;
}

bool CAnimationController::IsLoop() const
{
    return m_bLoop;
}

void CAnimationController::UpdateBlendWorldTM(uint32_t uIndex, const std::map<uint32_t, CMat4>& localMap)
{
    if (m_blendBonesWorldTMMap.find(uIndex) == m_blendBonesWorldTMMap.end())
    {
        BEATS_ASSERT(localMap.find(uIndex) != localMap.end());
        const CMat4& localTM = localMap.find(uIndex)->second;
        CSkeletonBone* parentBone = m_pSkeleton->GetSkeletonBoneById((uint8_t)uIndex)->GetParent();
        CMat4* pMat = GetMat();
        if (parentBone != NULL)
        {
            uint32_t parentIndex = parentBone->GetIndex();
            UpdateBlendWorldTM(parentIndex, localMap);
            BEATS_ASSERT(m_blendBonesWorldTMMap.find(parentIndex) != m_blendBonesWorldTMMap.end());
            const CMat4* parentWorldTM = m_blendBonesWorldTMMap[parentIndex];
            *pMat = (*parentWorldTM) * localTM;
        }
        else
        {
            *pMat = localTM;
        }
        m_blendBonesWorldTMMap[uIndex] = pMat;
    }
}

void CAnimationController::CalcDeltaMatrices()
{
    if(IsBlending())
    {
        m_blendDeltaMatrices.clear();
        m_blendBonesWorldTMMap.clear();
        m_matPoolIndex = 0;
        BEYONDENGINE_PERFORMDETECT_START(ePNT_AnimationManager_Blend)
        BlendAnimation(m_pPreAnimation, m_pCurrentAnimation);
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_AnimationManager_Blend)
        BEYONDENGINE_PERFORMDETECT_START(ePNT_AnimationManager_DeltaMatrix)
        uint32_t currFrame = GetCurrFrame();
        currFrame %= m_pCurrentAnimation->GetFrameCount();
        const std::map<uint8_t, CSkeletonBone*>& boneMap = m_pSkeleton->GetBoneMap();
        for (auto iter = boneMap.begin(); iter != boneMap.end(); ++iter)
        {
            CMat4* pWorldTM = NULL;
            CSkeletonBone* pSkeletonBone = iter->second;
            uint8_t skeletonType = iter->first;
            BEATS_ASSERT(m_blendBonesWorldTMMap.find(skeletonType) != m_blendBonesWorldTMMap.end());
            pWorldTM = m_blendBonesWorldTMMap[skeletonType];
            CMat4 posMatrixInverse = pSkeletonBone->GetTPosWorldTM();
            posMatrixInverse.Inverse();
            CMat4* pMat = GetMat();
            m_blendDeltaMatrices[skeletonType] = pMat;
            *pMat = (*pWorldTM) * posMatrixInverse;
        }
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_AnimationManager_DeltaMatrix)
    }
}

CMat4* CAnimationController::GetMat()
{
    CMat4* pRet = NULL;
    if (m_matPoolIndex < m_matPool.size())
    {
        pRet = m_matPool[m_matPoolIndex++];
    }
    else
    {
        BEATS_ASSERT(m_matPoolIndex == m_matPool.size());
        pRet = new CMat4;
        m_matPool.push_back(pRet);
        m_matPoolIndex = (uint32_t)m_matPool.size();
    }
    return pRet;
}

const std::map<uint8_t, CMat4*>& CAnimationController::GetBlendDeltaMatrices() const
{
    return m_blendDeltaMatrices;
}

CMat4 CAnimationController::GetBoneCurrWorldTM(uint8_t uBoneIndex) const
{
    CMat4 ret;
    if (m_pCurrentAnimation != NULL)
    {
        CSkeletonBone* pBone = m_pSkeleton->GetSkeletonBoneById(uBoneIndex);
        BEATS_ASSERT(pBone != nullptr);
        ret = pBone->GetCurrentWorldTM();
    }
    return ret;
}

//TODO:deprecated
void  CAnimationController::BlendAnimation(const CSkeletonAnimation* /*pBeforeAnimation*/, const CSkeletonAnimation* /*pAfterAnimation*/)
{
    //std::map<uint32_t, CMat4> localMap;
    //uint32_t frame = GetCurrFrame();

    //uint32_t beforeFramePlay = m_pPreAnimation->GetCurFrame();
    //uint32_t beforeFrame =  beforeFramePlay >= pBeforeAnimation->GetFrameCount() ? pBeforeAnimation->GetFrameCount() - 1 : beforeFramePlay;
    //uint32_t afterFrame =  frame >= pAfterAnimation->GetFrameCount() ? pAfterAnimation->GetFrameCount() - 1 : frame ;

    //const std::map<uint32_t, CMat4*> startBones = pBeforeAnimation->GetBoneLocalMatrixMapByFrame(beforeFrame);
    //const std::map<uint32_t, CMat4*> endBones = pAfterAnimation->GetBoneLocalMatrixMapByFrame(afterFrame);
    //BEATS_ASSERT(startBones.size() == endBones.size());
    //for (auto iter = startBones.begin(); iter != startBones.end(); ++iter)
    //{
    //    const CMat4* pMataStart = iter->second;
    //    auto endIter = endBones.find(iter->first);
    //    BEATS_ASSERT(endIter != endBones.end());
    //    const CMat4* pMatEnd = endIter->second;
    //    CMat4 matrix;
    //    if(pMataStart != pMatEnd)
    //    {
    //        Interpolation(*pMataStart, *pMatEnd, matrix);
    //    }
    //    else
    //    {
    //        matrix = *pMatEnd;
    //    }
    //    localMap[iter->first] = matrix;
    //}

    //for (auto iter = localMap.begin(); iter != localMap.end(); ++iter)
    //{
    //    UpdateBlendWorldTM(iter->first, localMap);
    //}
    //BEATS_ASSERT(m_blendBonesWorldTMMap.size() == localMap.size());
}

void CAnimationController::Interpolation(const CMat4& startMat, const CMat4& endMat,CMat4& insertMat)
{
    CVec3 pAxis;
    float radians;
    startMat.ToAxisAngle(&pAxis, &radians);

    CQuaternion start;
    start.FromAxisAngle(pAxis, radians);

    endMat.ToAxisAngle(&pAxis, &radians);

    CQuaternion end;
    end.FromAxisAngle(pAxis, radians);

    CQuaternion result;

    if(m_fPlayingTime <= m_fBlendTime )
    {
        BEATS_ASSERT(!BEATS_FLOAT_EQUAL(m_fBlendTime, 0));
        result = start.Slerp(end, m_fPlayingTime / m_fBlendTime);
        CMat4 mat;
        mat.FromQuaternion(&result);

        CVec3 posStart;
        CVec3 posEnd;
        CVec3 posStep;
        CVec3 posSub;
        CVec3 posReult;
        posStart.Fill(startMat[12], startMat[13], startMat[14]);
        posEnd.Fill(endMat[12], endMat[13], endMat[14]);
        posSub = posEnd - posStart;
        posStep = posSub * ((float)m_fPlayingTime / m_fBlendTime);
        posReult = posStart + posStep;
        mat[12] = posReult.X();
        mat[13] = posReult.Y();
        mat[14] = posReult.Z();

        insertMat = mat;
    }
}