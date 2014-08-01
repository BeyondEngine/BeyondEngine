#ifndef BEYOND_ENGINE_RENDER_RENDERBATCH_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERBATCH_H__INCLUDE

#include "CommonTypes.h"
#include "VertexFormat.h"
#include "RenderGroupManager.h"

class CMaterial;
class CVertexFormat;

class CRenderBatch
{
public:
    CRenderBatch(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material, 
        GLenum primitiveType, bool bIndexed, bool bShared = false, bool bShouldScaleContent = false);
    virtual ~CRenderBatch();

#ifdef _DEBUG
    void SetRenderBatchID(int id);
    void SetName(const TString &name);
#endif

    virtual void PreRender();
    virtual void DoRender();
    virtual void PostRender();
    void Render();

    void SetTransform(const kmMat4& mat);
    const kmMat4& GetTransform() const;

    void SetGroup(CRenderGroup* pGroup);
    CRenderGroup* GetGroup() const;

    void AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, CColor color2, const kmMat4 *pTransform = nullptr);
    void AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, const kmMat4 *pTransform = nullptr);
    void AddQuad(const CQuadP *quadP, const CQuadT *quadT, const kmMat4 *pTransform = nullptr);
    void AddQuad(const CQuadP *quadP, CColor color, const kmMat4 *pTransform = nullptr);

    void AddQuad(const void *pQuad, const kmMat4 *pTransform = nullptr);
    template <typename QuadType>
    void AddQuad(const QuadType *pQuad, const kmMat4 *pTransform = nullptr);

    void AddVertices(const void *pVertices, size_t count, const kmMat4 *pTransform = nullptr);
    template <typename VertexType>
    void AddVertices(const VertexType *pVertices, size_t count, const kmMat4 *pTransform = nullptr);

    void AddIndexedVertices(const void *pVertices, size_t vertexCount, 
        const unsigned short *pIndices, size_t indexCount, const kmMat4 *pTransform = nullptr);
    template <typename VertexType>
    void AddIndexedVertices(const VertexType *pVertices, size_t vertexCount, 
        const unsigned short *pIndices, size_t indexCount, const kmMat4 *pTransform = nullptr);

    size_t GetVertexCount() const;

    const CVertexFormat &GetVertexFormat() const;

    SharePtr<CMaterial> GetMaterial() const;

    GLenum GetPrimitiveType() const;

    bool IsIndexed() const;

    void Clear();

    void Complete();

    bool IsCompleted() const;

    void SetShouldScaleContent(bool bShouldScaleContent);

    void SetStatic(bool bStatic);
    bool IsStatic() const;

    //HACK: Called by BatchRenderGroup only if this batch is created by BatchRenderGroup as a shared batch
    void SetVertexFormat(const CVertexFormat &vertexFormat);
    void SetMaterial(SharePtr<CMaterial> pMaterial);
    void SetPrimitiveType(GLenum primitiveType);

    void SetRecycled(bool bRecycle);
    bool IsRecycled() const;

private:
    void setupVAO();
    void updateVBO();

    void AddVerticesImpl(const void *pVertices, size_t count, const kmMat4 *pTransform = nullptr);

private:
#ifdef _DEBUG
    int m_nRenderBatchId;
    TString m_strName;
#endif
    //Changed by BatchRenderGroup only
    const CVertexFormat *m_pVertexFormat;
    SharePtr<CMaterial> m_pMaterial;
    GLenum m_primitiveType;
    //never changed
    bool m_bIndexed;
    bool m_bShouldScaleContent;
    bool m_bShared;
    bool m_bStatic;
    bool m_bRecycled;

    std::vector<char> m_vertices;
    std::vector<unsigned short> m_indices;
    bool m_bCompleted;
    bool m_bVBOInvalidated;

    GLuint m_VAO;
    GLuint m_VBO[2];
    CRenderGroup* m_pGroup;

    kmMat4 m_mat4Transform;
};

template <typename QuadType>
void CRenderBatch::AddQuad(const QuadType *pQuad, const kmMat4 *pTransform /*= nullptr*/)
{
    BEATS_ASSERT(m_pVertexFormat->Compatible(VERTEX_FORMAT(decltype(pQuad->tl))),
        _T("Vertex type mismatched!"));
    AddQuad((const void *)pQuad, pTransform);
}

template <typename VertexType>
void CRenderBatch::AddVertices(const VertexType *pVertices, size_t count,
                               const kmMat4 *pTransform /*= nullptr*/)
{
    BEATS_ASSERT(m_pVertexFormat->Compatible(VERTEX_FORMAT(VertexType)), _T("Vertex type mismatched!"));
    AddVertices((const void *)pVertices, count, pTransform);
}

template <typename VertexType>
void CRenderBatch::AddIndexedVertices(const VertexType *pVertices, size_t vertexCount,
                                      const unsigned short *pIndices, size_t indexCount,
                                      const kmMat4 *pTransform /*= nullptr*/)
{
    BEATS_ASSERT(m_pVertexFormat->Compatible(VERTEX_FORMAT(VertexType)), _T("Vertex type mismatched!"));
    AddIndexedVertices((const void *)pVertices, vertexCount, pIndices, indexCount, pTransform);
}

#endif