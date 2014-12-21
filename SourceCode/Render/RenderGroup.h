#ifndef BEYOND_ENGINE_RENDER_RENDERGROUP_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERGROUP_H__INCLUDE

#include "RenderGroupManager.h"
#include "Camera.h"

class CRenderBatch;
class CVertexFormat;
class CMaterial;
class CTexture;
class CShaderUniform;
enum ERenderGroupSubID
{
    eRGSI_Solid,
    eRGSI_Alpha,

    eRGSI_Count
};
class CRenderGroup
{
public:
    CRenderGroup(ERenderGroupID ID, bool bShouldScaleContent = false);
    ~CRenderGroup();
    void Render();

    ERenderGroupID ID() const;
    void AddRenderBatch(CRenderBatch *object);
    void Clear();
    void SyncData() const;
    CRenderBatch* GetRenderBatch(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material,
        GLenum primitiveType, bool bIndexed, bool bTextureClampOrRepeat = true, const std::map<unsigned char, SharePtr<CTexture> >* pTextureMap = nullptr,
        const std::map<TString, CShaderUniform>* pUniformMap = nullptr);
    CRenderBatch* GetProxyBatch(CRenderBatch* pRefBatch);

    CCamera::ECameraType GetCameraType() const;
    void SetClearDepthFlag(bool bClearFlag);
    bool GetClearDepthFlag() const;
#ifdef DEVELOP_VERSION
    static bool m_bNewBatchCheck;
    static TString m_strBatchCheckTag;
#endif

private:
    CRenderBatch *GetRenderBatchImpl(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material,
        GLenum primitiveType, bool bIndexed);

private:
    bool m_bShouldScaleContent;
    bool m_bClearDepthBuffer = false;
    std::vector<CRenderBatch *> m_batchPool[2]; //0:non-indexed, 1:indexed
    ERenderGroupID m_nID;
    std::map<ERenderGroupSubID, std::vector<CRenderBatch*> > m_batches;
};

#endif // !RENDER_RENDER_QUEUE_H__INCLUDE
