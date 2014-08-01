#ifndef BEYOND_ENGINE_RENDER_SKIN_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SKIN_H__INCLUDE

#include "Resource/Resource.h"
#ifdef SW_SKEL_ANIM
#include "AnimationController.h"
#endif
#include "CommonTypes.h"
class CRenderBatch;
class CMaterial;
class CSkin : public CResource
{
    DECLARE_REFLECT_GUID(CSkin, 0x3479A0EF, CResource)
    DECLARE_RESOURCE_TYPE(eRT_Skin)

public:
    CSkin();
    virtual ~CSkin();

    virtual bool Load() override;
    virtual bool Unload() override;
    const std::vector<CRenderBatch*>& GetRenderBatches() const;

#ifdef SW_SKEL_ANIM
    void CalcSkelAnim(const CAnimationController::BoneMatrixMap &matrices);
#endif

private:
    std::vector<CRenderBatch*> m_renderBatches;
    SharePtr<CMaterial> m_pSkinMaterial;
};

#endif