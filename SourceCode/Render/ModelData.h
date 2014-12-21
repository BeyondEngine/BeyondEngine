#ifndef BEYOND_ENGINE_RENDER_MODELDATA_H__INCLUDE
#define BEYOND_ENGINE_RENDER_MODELDATA_H__INCLUDE

#include "Resource/Resource.h"

class CSkeleton;
class CSkin;
class CSkeletonAnimation;
class CModelData : public CResource
{
    DECLARE_REFLECT_GUID(CModelData, 0x15112401, CResource)
    DECLARE_RESOURCE_TYPE(eRT_Model)
public:
    CModelData();
    virtual ~CModelData();

    virtual bool Load() override;
    virtual bool Unload() override;

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    CSkeleton* GetSkeleton() const;
    CSkin* GetSkin() const;
    const std::map<TString, CSkeletonAnimation* >& GetAnimationMap() const;
#ifdef DEVELOP_VERSION
    virtual TString GetDescription() const override;
#endif
#ifdef EDITOR_MODE
    virtual void Reload() override;
#endif

private:
    CSkeleton* m_pSkeleton;
    CSkin* m_pSkin;
    std::map<TString, CSkeletonAnimation* > m_animationMap;
};

#endif