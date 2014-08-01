#ifndef BEYOND_ENGINE_RENDER_STATICMESHDATA_H__INCLUDE
#define BEYOND_ENGINE_RENDER_STATICMESHDATA_H__INCLUDE

#include "Resource/Resource.h"

class CRenderBatch;
class CMaterial;

struct SSubMesh
{
    SSubMesh()
        : m_pRenderBatch(nullptr)
    {
    }

    std::vector<CVertexPT> m_vertices;
    SharePtr<CMaterial> m_material;
    CRenderBatch *m_pRenderBatch;
};

class CStaticMeshData : public CResource
{
    DECLARE_REFLECT_GUID(CStaticMeshData, 0x9130C631, CResource)
    DECLARE_RESOURCE_TYPE(eRT_StaticMesh)
public:
    CStaticMeshData();
    virtual ~CStaticMeshData();

    virtual bool Load() override;
    virtual bool Unload() override;

    const std::vector<SSubMesh *> &GetSubMeshes() const;

private:
    std::vector<SSubMesh *> m_subMeshes;
};

#endif