#ifndef BEYOND_ENGINE_RENDER_RENDERGROUP_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERGROUP_H__INCLUDE

#include "RenderGroupManager.h"

class CRenderBatch;
class CVertexFormat;
class CMaterial;

class CRenderGroup
{
public:
    CRenderGroup(CRenderGroupManager::ERenderGroupID ID, bool bShouldScaleContent = false);
    ~CRenderGroup();

    bool PreRender();
    void Render();
    void PostRender();

    CRenderGroupManager::ERenderGroupID ID() const;
    void AddRenderBatch(CRenderBatch* pBatch);
    void Clear();

    /*Never hold the reference of the batch obtained by this method. 
    **If you want to reuse a render batch, create and manage it yourself*/
    CRenderBatch *GetRenderBatch(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material,
        GLenum primitiveType, bool bIndexed);

private:
    CRenderBatch *GetRenderBatchImpl(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material,
        GLenum primitiveType, bool bIndexed);

private:
    bool m_bShouldScaleContent;
#ifdef _DEBUG
    int m_curBatchID;
#endif
    std::vector<CRenderBatch *> m_batchPool[2]; //0:non-indexed, 1:indexed
    CRenderGroupManager::ERenderGroupID m_nID;
    std::vector<CRenderBatch *> m_batches;
};

#endif // !RENDER_RENDER_QUEUE_H__INCLUDE
