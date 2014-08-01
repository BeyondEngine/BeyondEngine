#include "stdafx.h"
#include "RenderBatch.h"
#include "RenderManager.h"
#include "Texture.h"
#include "Renderer.h"
#include "Material.h"
#include "VertexFormat.h"
#include "RenderTarget.h"
#include "external/Configuration.h"

CRenderBatch::CRenderBatch(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material,
                           GLenum primitiveType, bool bIndexed,
                           bool bShared, bool bShouldScaleContent)
   : m_pVertexFormat(&vertexFormat)
   , m_pMaterial(material)
   , m_primitiveType(primitiveType)
   , m_bIndexed(bIndexed)
   , m_bShared(bShared)
   , m_bStatic(false)
   , m_bShouldScaleContent(bShouldScaleContent)
   , m_bCompleted(false)
   , m_bVBOInvalidated(false)
   , m_VAO(0)
#ifdef _DEBUG
   , m_nRenderBatchId(0)
#endif
   , m_pGroup(NULL)
   , m_bRecycled(false)
{
    memset(m_VBO, 0, sizeof(m_VBO));
    kmMat4Identity( &m_mat4Transform );
}

CRenderBatch::~CRenderBatch()
{
    if(m_VAO != 0)
    {
        CRenderer::GetInstance()->BindVertexArray(0);
        CRenderer::GetInstance()->DeleteVertexArrays(1, &m_VAO);
    }
    if(m_VBO[0] != 0)
    {
        CRenderer::GetInstance()->BindBuffer(GL_ARRAY_BUFFER, 0);
        CRenderer::GetInstance()->DeleteBuffers(m_bIndexed ? 2 : 1, m_VBO);
    }
}

#ifdef _DEBUG
void CRenderBatch::SetRenderBatchID(int id)
{
    m_nRenderBatchId = id;
}

void CRenderBatch::SetName(const TString &name)
{
    m_strName = name;
}
#endif

void CRenderBatch::SetStatic(bool bStatic)
{
    m_bStatic = bStatic;
    m_bVBOInvalidated = true;
}

bool CRenderBatch::IsStatic() const
{
    return  m_bStatic;
}

void CRenderBatch::PreRender( )
{
    BEYONDENGINE_PERFORMDETECT_SCOPE(ePNT_RenderBatchPreRender)
    if(!m_vertices.empty())
    {
        if(m_VBO[0] == 0)
        {
            CRenderer::GetInstance()->GenBuffers(m_bIndexed ? 2 : 1, m_VBO);
        }
        if(m_VAO == 0)
        {
            if(CConfiguration::GetInstance()->SupportsShareableVAO())
            {
                CRenderer::GetInstance()->GenVertexArrays(1, &m_VAO);
                setupVAO();
            }
        }
        m_pMaterial->Use();
        if(m_bVBOInvalidated)
            updateVBO();
        if(CConfiguration::GetInstance()->SupportsShareableVAO())
        {
            CRenderer::GetInstance()->BindVertexArray(m_VAO);
        }
        else
        {
            m_pVertexFormat->SetupAttribPointer(m_VBO[0]);
            CRenderer::GetInstance()->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bIndexed ? m_VBO[1] : 0);
        }
        CRenderManager::GetInstance()->SendMVPToShader(m_mat4Transform);
    }
}

void CRenderBatch::DoRender( )
{
    BEYONDENGINE_PERFORMDETECT_SCOPE(ePNT_RenderBatchRender);
    if(!m_vertices.empty())
    {
        if(m_bIndexed)
        {
            BEATS_ASSERT(!m_indices.empty());
            CRenderer::GetInstance()->DrawElements(
                m_primitiveType, m_indices.size(), GL_UNSIGNED_SHORT, 0);
        }
        else
        {
            CRenderer::GetInstance()->DrawArrays( m_primitiveType, 0, GetVertexCount());
        }
    }
}

void CRenderBatch::PostRender( )
{
    BEYONDENGINE_PERFORMDETECT_SCOPE(ePNT_RenderBatchPostRender);
    if(!m_vertices.empty())
    {
        if(CConfiguration::GetInstance()->SupportsShareableVAO())
        {
            CRenderer::GetInstance()->BindVertexArray(0);
        }
        else
        {
            m_pVertexFormat->DisableAttribPointer();
            CRenderer::GetInstance()->BindBuffer(GL_ARRAY_BUFFER, 0);
            CRenderer::GetInstance()->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
}

void CRenderBatch::Render()
{
    PreRender();
    DoRender();
    PostRender();
}

void CRenderBatch::SetTransform(const kmMat4& mat)
{
    kmMat4Assign(&m_mat4Transform, &mat);
}

const kmMat4& CRenderBatch::GetTransform() const
{
    return m_mat4Transform;
}

void CRenderBatch::SetGroup(CRenderGroup* pGroup)
{
    m_pGroup = pGroup;
}

CRenderGroup* CRenderBatch::GetGroup() const
{
    return m_pGroup;
}

static void CopyAttribBuf2Buf(void *destBuffer, const void *srcBuffer,
                const CVertexFormat::SAttrib &vertexAttribute, size_t index)
{
    memcpy((char *)destBuffer + vertexAttribute.stride * index + vertexAttribute.offset,
        (const char *)srcBuffer + vertexAttribute.common.sizeInBytes * index,
        vertexAttribute.common.sizeInBytes);
}

static void CopyAttribVar2Buf(void *destBuffer, const void *srcVarPointer,
                const CVertexFormat::SAttrib &vertexAttribute, size_t index)
{
    memcpy((char *)destBuffer + vertexAttribute.stride * index + vertexAttribute.offset,
        (const char *)srcVarPointer, vertexAttribute.common.sizeInBytes);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, CColor color2, const kmMat4 *pTransform/* = nullptr*/)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.sizeInBytes == sizeof(quadP->tl));

    const CVertexFormat::SAttrib &attribT = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_TEXCOORD0);
    BEATS_ASSERT(attribT.common.sizeInBytes == sizeof(quadT->tl));

    const CVertexFormat::SAttrib &attribC = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_COLOR);
    BEATS_ASSERT(attribC.common.sizeInBytes == sizeof(CColor));
    
    const CVertexFormat::SAttrib &attribCC = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_COLOR2);
    BEATS_ASSERT(attribCC.common.sizeInBytes == sizeof(CColor));

    char quadBuffer[CVertexFormat::MAX_VERTEX_SIZE * 4];
    for(size_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribBuf2Buf(quadBuffer, quadT, attribT, i);
        CopyAttribVar2Buf(quadBuffer, &color, attribC, i);
        CopyAttribVar2Buf(quadBuffer, &color2, attribCC, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, const kmMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.sizeInBytes == sizeof(quadP->tl));

    const CVertexFormat::SAttrib &attribT = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_TEXCOORD0);
    BEATS_ASSERT(attribT.common.sizeInBytes == sizeof(quadT->tl));

    const CVertexFormat::SAttrib &attribC = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_COLOR);
    BEATS_ASSERT(attribC.common.sizeInBytes == sizeof(CColor));

    char quadBuffer[CVertexFormat::MAX_VERTEX_SIZE * 4];
    for(size_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribBuf2Buf(quadBuffer, quadT, attribT, i);
        CopyAttribVar2Buf(quadBuffer, &color, attribC, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, const CQuadT *quadT, const kmMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.sizeInBytes == sizeof(quadP->tl));

    const CVertexFormat::SAttrib &attribT = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_TEXCOORD0);
    BEATS_ASSERT(attribT.common.sizeInBytes == sizeof(quadT->tl));

    char quadBuffer[CVertexFormat::MAX_VERTEX_SIZE * 4];
    for(size_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribBuf2Buf(quadBuffer, quadT, attribT, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, CColor color, const kmMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.sizeInBytes == sizeof(quadP->tl));

    const CVertexFormat::SAttrib &attribC = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_COLOR);
    BEATS_ASSERT(attribC.common.sizeInBytes == sizeof(CColor));

    char quadBuffer[CVertexFormat::MAX_VERTEX_SIZE * 4];
    for(size_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribVar2Buf(quadBuffer, &color, attribC, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const void *pQuad, const kmMat4 *pTransform)
{
    switch ( m_primitiveType )
    {
    case GL_LINE_STRIP:
        {
            unsigned short indices[5] = {0, 1, 3, 2, 0};
            AddIndexedVertices(pQuad, 4, indices, 5, pTransform);
        }
    default:
        {
            unsigned short indices[6] = {0, 1, 2, 3, 2, 1};
            AddIndexedVertices(pQuad, 4, indices, 6, pTransform);
        }
        break;
    }
}

void CRenderBatch::AddVertices(const void *pVertices, size_t count, const kmMat4 *pTransform)
{
    BEATS_ASSERT(!m_bIndexed);
    AddVerticesImpl(pVertices, count, pTransform);
}

void CRenderBatch::AddIndexedVertices(const void *pVertices, size_t vertexCount,
                                    const unsigned short *pIndices, size_t indexCount,
                                    const kmMat4 *pTransform /* = nullptr */)
{
    BEATS_ASSERT(m_bIndexed);
    unsigned short currVertexCount = static_cast<unsigned short>(GetVertexCount());
    AddVerticesImpl(pVertices, vertexCount, pTransform);
    for(size_t i = 0; i < indexCount; ++i)
    {
        BEATS_ASSERT(pIndices[i] < vertexCount);
        m_indices.push_back(currVertexCount + pIndices[i]);
    }
}

void CRenderBatch::AddVerticesImpl(const void *pVertices, size_t count, const kmMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.size == 3 && attribP.common.type == GL_FLOAT);

    size_t bufferTop = m_vertices.size();
    size_t dataSize = m_pVertexFormat->Size() * count;
    m_vertices.resize(bufferTop + dataSize);
    memcpy(&m_vertices[bufferTop], pVertices, dataSize);

    if(pTransform || m_bShouldScaleContent)
    {
        float fScale = CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetScaleFactor();
        for(size_t i = 0; i < count; ++i)
        {
            kmVec3 *position = (kmVec3 *)&m_vertices[bufferTop + i * m_pVertexFormat->Size() + attribP.offset];
            if(pTransform)
                kmVec3Transform(position, position, pTransform);
            if(m_bShouldScaleContent)
            {
                position->x *= fScale;
                position->y *= fScale;
            }
        }
    }
    m_bVBOInvalidated = true;
}

size_t CRenderBatch::GetVertexCount() const
{
    return m_vertices.size() / m_pVertexFormat->Size();
}

void CRenderBatch::setupVAO()
{
    CRenderer *pRenderer = CRenderer::GetInstance();
    pRenderer->BindVertexArray(m_VAO);

    m_pVertexFormat->SetupAttribPointer(m_VBO[0]);

    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bIndexed ? m_VBO[1] : 0);

    pRenderer->BindVertexArray(0);
    m_pVertexFormat->DisableAttribPointer();
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
}

void CRenderBatch::updateVBO()
{
    BEYONDENGINE_PERFORMDETECT_SCOPE(ePNT_RenderBatchUpdateVBO);
    if(!m_vertices.empty())
    {
        CRenderer *pRenderer = CRenderer::GetInstance();

        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderBatchUpdateVBO0);
        pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
        pRenderer->BufferData(GL_ARRAY_BUFFER, m_vertices.size(), &m_vertices[0],
            m_bStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderBatchUpdateVBO0);

        if(m_bIndexed)
        {
            BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderBatchUpdateVBO1);
            BEATS_ASSERT(!m_indices.empty());
            pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[1]);
            pRenderer->BufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned short),
                &m_indices[0], m_bStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
            BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderBatchUpdateVBO1);
        }
    }
    m_bVBOInvalidated = false;
}

void CRenderBatch::Clear()
{
    m_vertices.clear();
    m_indices.clear();
    m_bCompleted = false;
    m_bVBOInvalidated = false;
}

const CVertexFormat &CRenderBatch::GetVertexFormat() const
{
    return *m_pVertexFormat;
}

void CRenderBatch::SetVertexFormat(const CVertexFormat &vertexFormat)
{
    BEATS_ASSERT(!m_bShared || m_vertices.empty(), 
        _T("Can't set material of a render batch which already been used!"));
    if(*m_pVertexFormat != vertexFormat)
    {
        m_pVertexFormat = &vertexFormat;
        if(CConfiguration::GetInstance()->SupportsShareableVAO())
        {
            setupVAO();
        }
    }
}

SharePtr<CMaterial> CRenderBatch::GetMaterial() const
{
    return m_pMaterial;
}

void CRenderBatch::SetMaterial(SharePtr<CMaterial> pMaterial)
{
    BEATS_ASSERT(!m_bShared || m_vertices.empty(), 
        _T("Can't set material of a render batch which already been used!"));
    m_pMaterial = pMaterial;
}

GLenum CRenderBatch::GetPrimitiveType() const
{
    return m_primitiveType;
}

void CRenderBatch::SetPrimitiveType(GLenum primitiveType)
{
    BEATS_ASSERT(!m_bShared || m_vertices.empty(),
        _T("Can't set primitive type of a render batch which already been used!"));
    m_primitiveType = primitiveType;
}

void CRenderBatch::SetRecycled(bool bRecycle)
{
    m_bRecycled = bRecycle;
}

bool CRenderBatch::IsRecycled() const
{
    return m_bRecycled;
}

bool CRenderBatch::IsIndexed() const
{
    return m_bIndexed;
}

void CRenderBatch::Complete()
{
    m_bCompleted = true;
}

bool CRenderBatch::IsCompleted() const
{
    return m_bCompleted;
}
