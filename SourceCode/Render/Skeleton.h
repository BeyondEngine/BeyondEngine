#ifndef BEYOND_ENGINE_RENDER_SKELETON_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SKELETON_H__INCLUDE

#include "resource/Resource.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
class CSkeletonBone;

class CSkeleton : public CResource
{
    DECLARE_REFLECT_GUID(CSkeleton, 0x1147A0EF, CResource)
    DECLARE_RESOURCE_TYPE(eRT_Skeleton)

public:
    CSkeleton();
    virtual ~CSkeleton();

    virtual bool Load() override;

    SharePtr<CSkeletonBone> GetSkeletonBone(ESkeletonBoneType type) const;
    std::map<ESkeletonBoneType, SharePtr<CSkeletonBone>>& GetBoneMap();
private:
    std::map<ESkeletonBoneType, SharePtr<CSkeletonBone>> m_bonesMap;
};

#endif