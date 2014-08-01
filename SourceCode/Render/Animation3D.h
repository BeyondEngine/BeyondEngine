#ifndef BEYOND_ENGINE_ANIMATION_3D_H__INCLUDE
#define BEYOND_ENGINE_ANIMATION_3D_H__INCLUDE

#include "resource/Resource.h"

struct kmMat4;

class CAnimation3D : public CResource
{
    DECLARE_REFLECT_GUID(CAnimation3D, 0xC17B21CD, CResource)
    DECLARE_RESOURCE_TYPE(eRT_Animation)
public:
    CAnimation3D();
    virtual ~CAnimation3D();
    
    virtual bool Load() override;
    virtual bool Unload() override;

    float Duration();
    const std::vector<ESkeletonBoneType>& GetBones() const;
    size_t GetBoneIndexFromType(const ESkeletonBoneType type) const;
    const std::vector<kmMat4*>& GetBoneMatrixByFrame(size_t uFrame) const;
    const std::vector<kmMat4*>& GetBoneMatrixByTime(float playtime) const;
    size_t GetFPS() const;
    const kmMat4*GetOneBoneMatrixByTime(float fTime, ESkeletonBoneType boneType) const;
    unsigned short GetFrameCount() const;
    void SetCurFrame(size_t curFrame);
    size_t GetCurFrame() const;
private:
    unsigned short m_uFrameCount;
    size_t m_uFPS;
    float m_fDuration;
    size_t m_uCurFrame;
    std::vector<ESkeletonBoneType> m_bones;
    std::vector<std::vector<kmMat4*>> m_posOfFrame; 
    std::map<ESkeletonBoneType, size_t> m_boneIndexMap;
};

#endif