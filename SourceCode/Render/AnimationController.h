#ifndef BEYOND_ENGINE_RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#include "BaseAnimationController.h"
class CAnimation3D;
class CSkeleton;
class CSkeletonBone;

class CAnimationController : public CBaseAnimationController
{
public:

    typedef std::map<ESkeletonBoneType, kmMat4> BoneMatrixMap;
    typedef std::vector<kmMat4>                 BoneMatrix;

    CAnimationController();
    virtual ~CAnimationController();

    void SetSkeleton(SharePtr<CSkeleton> skeleton);
    SharePtr<CAnimation3D> GetCurrentAnimation();
    void PlayAnimation(const SharePtr<CAnimation3D>& pAnimation, float fBlendTime, bool bLoop);
    void GoToFrame(size_t frame);
    void Pause();
    void Resume();
    void Stop();
    bool IsPlaying();
    size_t GetCurrFrame() const;
    float GetPlayingTime() const;
    void Update(float fDeltaTime) override;
    const BoneMatrixMap& GetDeltaMatrices() const;

    const BoneMatrixMap& GetInitWorldTransform() const;
    const BoneMatrixMap& GetCurWorldTransform() const;

    void CalcDeltaMatrices();

    void  GetBoneInitWorldTM(kmMat4& transform, const SharePtr<CSkeletonBone> pTposSkeletonBone);
    void  GetBoneCurWorldTM(kmMat4&  transform, const ESkeletonBoneType boneType);
    void  BlendAnimation(const SharePtr<CAnimation3D> pBeforeAnimation, const SharePtr<CAnimation3D> pAfterAnimation);
    const kmMat4*  GetBoneTM(ESkeletonBoneType boneType);
    size_t GetPlayingFrame();
private:
    void  BlendData(const std::vector<kmMat4*>& startBones, const std::vector<kmMat4*>& endBones);
    void  Interpolation(const kmMat4& startMat, const kmMat4& endMat,kmMat4& insertMat);
    void  Slerp( kmQuaternion* pOut,const kmQuaternion* q1,const kmQuaternion* q2,kmScalar t);
    void  ClearBlendData();
    bool  CheckBlend();
    void  CalcInitBoneWorldTM();

private:
    bool m_bLoop;
    size_t m_uLoopCount;
    bool m_bPlaying;
    float m_fPlayingTime;
    float m_fBlendTime;
    SharePtr<CAnimation3D> m_pCurrentAnimation;
    SharePtr<CAnimation3D> m_pPreAnimation;
    SharePtr<CSkeleton> m_pSkeleton;

    // this matrix convert Tpose skeleton to current frame pos.
    BoneMatrixMap m_deltaMatrices;

    //this matrix convert  bone space to world space
    BoneMatrixMap m_initBoneWorldTM;
    BoneMatrixMap m_curBoneWorldTM;
    BoneMatrix m_blendBonesMat;
    std::map<size_t,BoneMatrix> m_bonesMatOfFrameMap;
};

#endif