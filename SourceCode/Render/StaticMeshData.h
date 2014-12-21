#ifndef BEYOND_ENGINE_RENDER_STATICMESHDATA_H__INCLUDE
#define BEYOND_ENGINE_RENDER_STATICMESHDATA_H__INCLUDE

#include "Resource/Resource.h"
#include "Scene/Node3D.h"
#include "MathExt/AABBBox.h"

class CMaterial;
class CNodeAnimationData;
class CRenderBatch;
struct SSubMesh : public CNode3D
{
    SSubMesh();
    virtual ~SSubMesh();
    CRenderBatch *m_pRenderBatch = nullptr;
    CNodeAnimationData* m_pNodeAnimationData = nullptr;
    CAABBBox m_aabb;
};

class CStaticMeshData : public CResource
{
    DECLARE_REFLECT_GUID(CStaticMeshData, 0x9130C631, CResource)
    DECLARE_RESOURCE_TYPE(eRT_StaticMesh)
public:
    CStaticMeshData();
    virtual ~CStaticMeshData();

    virtual bool Load() override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;

    const std::vector<SSubMesh*>& GetSubMeshes() const;
#ifdef EDITOR_MODE
    virtual void Reload() override;
#endif
#ifdef DEVELOP_VERSION
    virtual TString GetDescription() const override;
    uint32_t m_uVertexCount = 0;
#endif

    const CAABBBox& GetAABB() const;
private:
    std::vector<SSubMesh *> m_subMeshes;
    CAABBBox m_aabb;
};

#endif