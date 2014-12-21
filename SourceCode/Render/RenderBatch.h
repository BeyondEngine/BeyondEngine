#ifndef BEYOND_ENGINE_RENDER_RENDERBATCH_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERBATCH_H__INCLUDE

#include "CommonTypes.h"
#include "VertexFormat.h"
#include "RenderGroupManager.h"
#include "ShaderUniform.h"

class CMaterial;
class CVertexFormat;
class CTexture;
class CVertexBufferContent;
//#define ENABLE_SINGLE_MVP_UNIFORM

class CRenderBatch
{
public:
    CRenderBatch(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material, 
        GLenum primitiveType, bool bIndexed, bool bShouldScaleContent = false);
    virtual ~CRenderBatch();
    void Render(CRenderGroup* pRenderGroup);
    void SetVBO(uint32_t uVBO);
    void SetEBO(uint32_t uEBO);
    void SetVAO(uint32_t uVAO);
    uint32_t GetVBO() const;
    uint32_t GetEBO() const;
    uint32_t GetVAO() const;

    void SetWorldTM(const CMat4* mat);
    const CMat4* GetWorldTM() const;
#ifdef ENABLE_SINGLE_MVP_UNIFORM
    void SetViewTM(const CMat4* mat);
    const CMat4* GetViewTM() const;
    void SetProjectionTM(const CMat4* mat);
    const CMat4* GetProjectionTM() const;
#endif
    void AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, CColor color2, const CMat4 *pTransform = nullptr);
    void AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, const CMat4 *pTransform = nullptr);
    void AddQuad(const CQuadP *quadP, const CQuadT *quadT, const CMat4 *pTransform = nullptr);
    void AddQuad(const CQuadP *quadP, CColor color, const CMat4 *pTransform = nullptr);

    void AddQuad(const void *pQuad, const CMat4 *pTransform = nullptr);
    template <typename QuadType>
    void AddQuad(const QuadType *pQuad, const CMat4 *pTransform = nullptr);

    void AddVertices(const void *pVertices, uint32_t count, const CMat4 *pTransform = nullptr);
    template <typename VertexType>
    void AddVertices(const VertexType *pVertices, uint32_t count, const CMat4 *pTransform = nullptr);

    void AddIndexedVertices(const void *pVertices, uint32_t vertexCount, 
        const unsigned short *pIndices, uint32_t indexCount, const CMat4 *pTransform = nullptr);
    template <typename VertexType>
    void AddIndexedVertices(const VertexType *pVertices, uint32_t vertexCount, 
        const unsigned short *pIndices, uint32_t indexCount, const CMat4 *pTransform = nullptr);

    uint32_t GetVertexCount();
    uint32_t GetDataSize() const;
    void SetDataSize(uint32_t uDataSize);
    uint32_t GetStartPos() const;
    void SetStartPos(uint32_t uStartPos);

    const CVertexFormat &GetVertexFormat() const;

    SharePtr<CMaterial> GetMaterial() const;

    GLenum GetPrimitiveType() const;

    bool IsIndexed() const;
    void SyncData();
    void Clear();
    bool GetShouldScaleContent() const;

    void SetStatic(bool bStatic);
    bool IsStatic() const;

    void SetAutoManage(bool bAutoManage);
    bool IsAutoManage() const;
    void SetVertexFormat(const CVertexFormat &vertexFormat);
    void SetMaterial(SharePtr<CMaterial> pMaterial);
    void SetPrimitiveType(GLenum primitiveType);
    void SetTextureClampOrRepeat(bool bClamp);
    bool GetTextureClampOrRepeat() const;

    void Reset();
    void RefreshStaticBatch();
    void SetTexture(unsigned char uChannel, const SharePtr<CTexture>& texture);
    const std::map<unsigned char, SharePtr<CTexture> >& GetTextureMap() const;
    void SetTextureMap(const std::map<unsigned char, SharePtr<CTexture> >& textureMap);
    std::map<TString, CShaderUniform>& GetUniformMap();
    void SetUniformMap(const std::map<TString, CShaderUniform>& uniformMap);
    void SetRefBatch(CRenderBatch* pBatch);
    void SetPreRenderAction(std::function<void()> function);
    void SetPostRenderAction(std::function<void()> function);
    static CMat4* RequestMat();
    static void DestroyMatPool();

private:
    void AddVerticesImpl(const void *pVertices, uint32_t count, const CMat4 *pTransform = nullptr);
    CSerializer* GetVertexBuffer();
private:
    const CVertexFormat* m_pVertexFormat;
    SharePtr<CMaterial> m_pMaterial;
    GLenum m_primitiveType;
    //never changed
    bool m_bIndexed;
    bool m_bShouldScaleContent;
    bool m_bStatic;
    // For non-static batch, this flag indicate if this batch will be recycle and reuse
    // For static batch, this flag indicate if this batch will delete VAO VBO EBO when destruct.
    bool m_bAutoManage;
    bool m_bTextureClampOrRepeat = true;
    uint32_t m_uStartPos;
    uint32_t m_uDataSize;
    uint32_t m_uVBO;
    uint32_t m_uEBO;
    uint32_t m_uVAO;
#ifdef ENABLE_SINGLE_MVP_UNIFORM
    CMat4* m_pViewMat = nullptr;
    CMat4* m_pProjectMat = nullptr;
#endif
    CMat4* m_pWorldMat = nullptr;
    CSerializer m_vertexData;
    std::vector<unsigned short> m_indices;
    std::map<unsigned char, SharePtr<CTexture> > m_textureMap;
    std::map<TString, CShaderUniform> m_uniformMap;
    CRenderBatch* m_pRefBatch = nullptr;
    std::function<void()> m_pPreRenderAction;
    std::function<void()> m_pPostRenderAction;
    static std::vector<CMat4*> m_matPool;
#ifdef DEVELOP_VERSION
public:
    ERenderBatchUsage m_usage = ERenderBatchUsage::eRBU_Count;
#endif
};

template <typename QuadType>
void CRenderBatch::AddQuad(const QuadType *pQuad, const CMat4 *pTransform /*= nullptr*/)
{
    BEATS_ASSERT(m_pVertexFormat->Compatible(VERTEX_FORMAT(decltype(pQuad->tl))),
        _T("Vertex type mismatched!"));
    AddQuad((const void *)pQuad, pTransform);
}

template <typename VertexType>
void CRenderBatch::AddVertices(const VertexType *pVertices, uint32_t count,
                               const CMat4 *pTransform /*= nullptr*/)
{
    if (count > 0)
    {
        BEATS_ASSERT(m_pVertexFormat->Compatible(VERTEX_FORMAT(VertexType)), _T("Vertex type mismatched!"));
        AddVertices((const void *)pVertices, count, pTransform);
    }
}

template <typename VertexType>
void CRenderBatch::AddIndexedVertices(const VertexType *pVertices, uint32_t vertexCount,
                                      const unsigned short *pIndices, uint32_t indexCount,
                                      const CMat4 *pTransform /*= nullptr*/)
{
    if (vertexCount > 0 && indexCount > 0)
    {
        BEATS_ASSERT(m_pVertexFormat->Compatible(VERTEX_FORMAT(VertexType)), _T("Vertex type mismatched!"));
        AddIndexedVertices((const void *)pVertices, vertexCount, pIndices, indexCount, pTransform);
    }
}

#endif