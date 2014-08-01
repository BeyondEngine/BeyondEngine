#include "stdafx.h"
#include "Animation3D.h"
#include "AnimationController.h"
#include "Skeleton.h"
#include "SkeletonBone.h"
#include "Resource/ResourcePublic.h"
#include <math.h>
#include "RenderManager.h"

CAnimationController::CAnimationController()
    : m_fPlayingTime(0.0f)
    , m_bLoop(false)
    , m_bPlaying(false)
    , m_fBlendTime(1.0f)
    , m_uLoopCount(0)
{
}

CAnimationController::~CAnimationController()
{
}

void CAnimationController::SetSkeleton(SharePtr<CSkeleton> skeleton)
{
    m_pSkeleton = skeleton;
    m_initBoneWorldTM.clear();
}

SharePtr<CAnimation3D> CAnimationController::GetCurrentAnimation()
{
    return m_pCurrentAnimation;
}

void CAnimationController::PlayAnimation(const SharePtr<CAnimation3D>& pAnimation, float fBlendTime, bool bLoop)
{
    if(m_pCurrentAnimation)
    {
        m_pPreAnimation = m_pCurrentAnimation;
        size_t currFrame = static_cast<size_t>(m_fPlayingTime * m_pCurrentAnimation->GetFPS());
        currFrame %= m_pCurrentAnimation->GetFrameCount();
        m_pPreAnimation->SetCurFrame(currFrame);
    }

    m_bonesMatOfFrameMap.clear();
    
    m_pCurrentAnimation = pAnimation;
    m_bLoop = bLoop;
    m_fPlayingTime = 0;
    m_uLoopCount = 0;
    m_bPlaying = true;

    CalcInitBoneWorldTM();
}

void CAnimationController::GoToFrame( size_t frame )
{
    if(m_pCurrentAnimation)
    {
        m_fPlayingTime = (float)frame / m_pCurrentAnimation->GetFPS();
        m_bPlaying = false;
        CalcDeltaMatrices();
    }
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
    m_fPlayingTime = 0;
    CalcDeltaMatrices();
    m_bPlaying = false;
}

bool CAnimationController::IsPlaying()
{
    return m_bPlaying;
}

size_t CAnimationController::GetCurrFrame() const
{
    size_t uRet = 0xFFFFFFFF;
    if(m_pCurrentAnimation)
    {
        uRet = static_cast<size_t>(m_fPlayingTime * m_pCurrentAnimation->GetFPS());
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
        m_fPlayingTime += fDeltaTime;
        if (m_fPlayingTime > m_pCurrentAnimation->Duration())
        {
            if(m_bLoop)
            {
                m_fPlayingTime = fmod(m_fPlayingTime, m_pCurrentAnimation->Duration());
                m_uLoopCount++;
            }
            else
            {
                m_fPlayingTime = m_pCurrentAnimation->Duration();
            }
        }
        BEYONDENGINE_PERFORMDETECT_START(ePNT_AnimationManager_CalcMatrix)
        CalcDeltaMatrices();
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_AnimationManager_CalcMatrix)
    }
}

bool  CAnimationController::CheckBlend()
{
    return m_pPreAnimation && m_fPlayingTime <= m_fBlendTime && 0 == m_uLoopCount ;
}

void  CAnimationController::CalcInitBoneWorldTM()
{
    BEATS_ASSERT(m_pCurrentAnimation);
    BEATS_ASSERT(m_pSkeleton);
    const std::vector<ESkeletonBoneType>& bones = m_pCurrentAnimation->GetBones();
    if(m_initBoneWorldTM.empty())
    {
        for (size_t i = 0; i < bones.size(); ++i)
        {
            SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(bones[i]);
            kmMat4 initTransform;
            kmMat4Identity(&initTransform);
            GetBoneInitWorldTM(initTransform,pSkeletonBone);
            m_initBoneWorldTM[pSkeletonBone->GetBoneType()] = initTransform;
        }
    }
}

const kmMat4*  CAnimationController::GetBoneTM(ESkeletonBoneType boneType)
{
    size_t indexBone = m_pCurrentAnimation->GetBoneIndexFromType(boneType);
    return  &m_blendBonesMat[indexBone];
}

void CAnimationController::CalcDeltaMatrices()
{
    m_deltaMatrices.clear();
    m_curBoneWorldTM.clear();
    if(CheckBlend())
    {
        BEYONDENGINE_PERFORMDETECT_START(ePNT_AnimationManager_Blend)
        BlendAnimation(m_pPreAnimation,m_pCurrentAnimation);
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_AnimationManager_Blend)
    }

    const std::vector<ESkeletonBoneType>& bones = m_pCurrentAnimation->GetBones();

    BEYONDENGINE_PERFORMDETECT_START(ePNT_AnimationManager_DeltaMatrix)
    BoneMatrix bonesMat;
    BoneMatrix cacheBonesMat;
    size_t currFrame = static_cast<size_t>(m_fPlayingTime * m_pCurrentAnimation->GetFPS());
    currFrame %= m_pCurrentAnimation->GetFrameCount();
    std::map<size_t,BoneMatrix>::const_iterator iter = m_bonesMatOfFrameMap.find(currFrame);
    if( iter != m_bonesMatOfFrameMap.end())
    {
        cacheBonesMat = iter->second;
        BEATS_ASSERT(bones.size() == cacheBonesMat.size());
    }
    for (size_t i = 0; i < bones.size(); ++i)
    {
        SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(bones[i]);
         if( CheckBlend() )
         {
             BEATS_ASSERT(m_blendBonesMat.size() > 0);
             ESkeletonBoneType boneType = bones[i];
             SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(boneType);
             ESkeletonBoneType parentType = pSkeletonBone->GetParentType();

             kmMat4 curTransform;
             kmMat4Assign(&curTransform, &m_blendBonesMat[i]);
             while (eSBT_Null != parentType)
             {
                 const kmMat4* mat = GetBoneTM(parentType);
                 SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(parentType);
                 parentType = pSkeletonBone->GetParentType();
                 kmMat4Multiply(&curTransform, mat, &curTransform);
             }

             m_curBoneWorldTM[bones[i]] = curTransform;
        }
        else
        {
            if(cacheBonesMat.size() > 0)
            {
                 m_curBoneWorldTM[bones[i]] = cacheBonesMat[i];
            }
            else
            {
                kmMat4 curTransform;
                GetBoneCurWorldTM(curTransform ,bones[i]);
                m_curBoneWorldTM[bones[i]] = curTransform;
                bonesMat.push_back(curTransform);
            }

        }
        BEATS_ASSERT(bones[i] == pSkeletonBone->GetBoneType());
        const kmMat4& tPosMatrix = m_initBoneWorldTM[pSkeletonBone->GetBoneType()];
        kmMat4 posMatrixInverse;
        kmMat4Inverse(&posMatrixInverse,&tPosMatrix);
        kmMat4Multiply(&m_deltaMatrices[pSkeletonBone->GetBoneType()],&m_curBoneWorldTM[bones[i]],&posMatrixInverse);
    }
    if(m_bonesMatOfFrameMap.size() < m_pCurrentAnimation->GetFrameCount())
    {
        if(bonesMat.size() > 0)
        {
            m_bonesMatOfFrameMap[currFrame] = bonesMat;
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_AnimationManager_DeltaMatrix)
}

const CAnimationController::BoneMatrixMap& CAnimationController::GetDeltaMatrices() const
{
    return m_deltaMatrices;
}


void  CAnimationController::GetBoneInitWorldTM(kmMat4& transform, const SharePtr<CSkeletonBone> pTPosSkeletonBone)
{
    const kmMat4& tPosMatrix = pTPosSkeletonBone->GetTPosMatrix();
    kmMat4Multiply(&transform,&tPosMatrix,&transform);
    ESkeletonBoneType parentType = pTPosSkeletonBone->GetParentType();

    if(eSBT_Null != parentType)
    {
        SharePtr<CSkeletonBone> pParentSkeletonBone = m_pSkeleton->GetSkeletonBone(parentType);
        GetBoneInitWorldTM(transform,pParentSkeletonBone);
    }
}
void  CAnimationController::GetBoneCurWorldTM(kmMat4&  transform, const ESkeletonBoneType boneType)
{
    const kmMat4* mat = m_pCurrentAnimation->GetOneBoneMatrixByTime(m_fPlayingTime, boneType);  
    SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(boneType);
    kmMat4Assign(&transform, mat);
    ESkeletonBoneType parentType = pSkeletonBone->GetParentType();

    BoneMatrixMap::const_iterator iter = m_curBoneWorldTM.find(parentType);
    if(iter != m_curBoneWorldTM.end())
    {
        const kmMat4 absMat =  iter->second;
        kmMat4Multiply(&transform,&absMat,&transform);
    }
    else
    {
        while(eSBT_Null != parentType)
        {
            const kmMat4* boneMat = m_pCurrentAnimation->GetOneBoneMatrixByTime(m_fPlayingTime, parentType);
            kmMat4Multiply(&transform,boneMat,&transform);

            SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(parentType);
            parentType = pSkeletonBone->GetParentType();
        }
    }
}

void  CAnimationController::BlendAnimation(const SharePtr<CAnimation3D> pBeforeAnimation, const SharePtr<CAnimation3D> pAfterAnimation)
{
     size_t frame = static_cast<size_t>(m_fPlayingTime * m_pCurrentAnimation->GetFPS());
   
     size_t beforeFramePlay = m_pPreAnimation->GetCurFrame();
     size_t beforeFrame =  beforeFramePlay >= pBeforeAnimation->GetFrameCount() ? pBeforeAnimation->GetFrameCount() - 1 : beforeFramePlay;
     size_t afterFrame =  frame >= pAfterAnimation->GetFrameCount() ? pAfterAnimation->GetFrameCount() - 1 : frame ;

     const std::vector<kmMat4*>& beforeMat = pBeforeAnimation->GetBoneMatrixByFrame(beforeFrame);

     const std::vector<kmMat4*>& afterMat = pAfterAnimation->GetBoneMatrixByFrame(afterFrame);

     BlendData(beforeMat, afterMat); 
}

void  CAnimationController::ClearBlendData()
{
    m_blendBonesMat.clear();
}

void CAnimationController::BlendData(const std::vector<kmMat4*>& startBones, const std::vector<kmMat4*>& endBones )
{
    ClearBlendData();

    BEATS_ASSERT(startBones.size() == endBones.size())
    std::vector<std::vector<kmMat4>> boneOfFrame;
    size_t boneCount = startBones.size();
    for (size_t i = 0; i < boneCount; ++i)
    {
        const kmMat4* pMataStart = startBones[i];
        const kmMat4* pMatEnd = endBones[i];
        
        if(pMataStart != pMatEnd && KM_FALSE == kmMat4AreEqual(pMataStart, pMatEnd))
        {
            kmMat4 boneInsert;
            Interpolation(*pMataStart, *pMatEnd,boneInsert);
            m_blendBonesMat.push_back(boneInsert);
        }
        else
        {
            kmMat4 matrix;
            kmMat4Assign(&matrix,pMatEnd);
            m_blendBonesMat.push_back(matrix);
        }
    }
}

void CAnimationController::Interpolation(const kmMat4& startMat, const kmMat4& endMat,kmMat4& insertMat)
{
    kmVec3 pAxis;
    kmScalar radians;
    kmMat4RotationToAxisAngle(&pAxis, &radians,&startMat);

    kmQuaternion start;
    kmQuaternionRotationAxis(&start, &pAxis, radians);

    kmMat4RotationToAxisAngle(&pAxis, &radians,&endMat);

    kmQuaternion end;
    kmQuaternionRotationAxis(&end, &pAxis, radians);

    kmQuaternion result;

    if(m_fPlayingTime <= m_fBlendTime )
    {
        Slerp(&result, &start, &end, (float)m_fPlayingTime / m_fBlendTime);

        kmMat4 mat;
        kmMat4RotationQuaternion(&mat, &result);
        
        kmVec3 posStart;
        kmVec3 posEnd;
        kmVec3 posStep;
        kmVec3 posSub;
        kmVec3 posReult;
        kmVec3Fill(&posStart, startMat.mat[12], startMat.mat[13], startMat.mat[14]);
        kmVec3Fill(&posEnd, endMat.mat[12], endMat.mat[13], endMat.mat[14]);
        kmVec3Subtract(&posSub, &posEnd, &posStart);
        kmVec3Scale(&posStep, &posSub, (float)m_fPlayingTime / m_fBlendTime);
        kmVec3Add(&posReult, &posStart, &posStep);
        mat.mat[12] = posReult.x;
        mat.mat[13] = posReult.y;
        mat.mat[14] = posReult.z;

        kmMat4Assign(&insertMat,&mat);
    }
}

void CAnimationController::Slerp( kmQuaternion* pOut,
                                 const kmQuaternion* q1,
                                 const kmQuaternion* q2,
                                 kmScalar t)
{
    float w0,x0,y0,z0;
    float w1,x1,y1,z1;

    w0 = q1->w;
    x0 = q1->x;
    y0 = q1->y;
    z0 = q1->z;

    w1 = q2->w;
    x1 = q2->x;
    y1 = q2->y;
    z1 = q2->z;

    float w,x,y,z;

    float cosOmega = w0*w1 + x0*x1 + y0*y1 + z0*z1;

    if (cosOmega < 0.0f) 
    {
        w1 = w1 * -1;
        x1 = x1 * -1;
        y1 = y1 * -1;
        z1 = z1 * -1;
        cosOmega = cosOmega * -1;
    }

    float k0, k1;

    if (cosOmega > 0.9999f) 
    {

        k0 = 1.0f-t;
        k1 = t;
    } 
    else
    {
        float sinOmega = sqrt(1.0f - cosOmega * cosOmega);

        float omega = atan2(sinOmega, cosOmega);
        float oneOverSinOmega = 1.0f / sinOmega;

        k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
        k1 = sin(t * omega) * oneOverSinOmega;
    }

    w = w0 * k0 + w1 * k1;
    x = x0 * k0 + x1 * k1;
    y = y0 * k0 + y1 * k1;
    z = z0 * k0 + z1 * k1;

    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    pOut->w = w;
}
