#include "stdafx.h"
#include "RenderBatch.h"
#include "RenderManager.h"
#include "Texture.h"
#include "Renderer.h"
#include "Material.h"
#include "VertexFormat.h"
#include "RenderTarget.h"
#include "external/Configuration.h"
#include "VertexBufferContent.h"
#include "RenderGroup.h"

std::vector<CMat4*> CRenderBatch::m_matPool;

CRenderBatch::CRenderBatch(const CVertexFormat &vertexFormat, SharePtr<CMaterial> material,
                           GLenum primitiveType, bool bIndexed, bool bShouldScaleContent)
   : m_pVertexFormat(&vertexFormat)
   , m_pMaterial(material)
   , m_primitiveType(primitiveType)
   , m_bIndexed(bIndexed)
   , m_bStatic(false)
   , m_bShouldScaleContent(bShouldScaleContent)
   , m_bAutoManage(false)
   , m_uStartPos(0xFFFFFFFF)
   , m_uDataSize(0)
   , m_uVBO(0)
   , m_uEBO(0)
   , m_uVAO(0)
{
    BEATS_ASSERT(m_pMaterial != NULL && m_pMaterial->IsInitialized());
    Reset();
}

CRenderBatch::~CRenderBatch()
{
    if (m_bStatic && m_bAutoManage)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        if (m_uVAO != 0)
        {
            GLint currVAO = 0;
#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
            pRenderer->GetIntegerV(GL_VERTEX_ARRAY_BINDING_OES, &currVAO);
#else
            pRenderer->GetIntegerV(GL_VERTEX_ARRAY_BINDING, &currVAO);
#endif
            if ((uint32_t)currVAO == m_uVAO)
            {
                pRenderer->BindVertexArray(0);
            }
            pRenderer->DeleteVertexArrays(1, &m_uVAO);
        }
        if (m_uVBO != 0)
        {
            GLint currVBO = 0;
            pRenderer->GetIntegerV(GL_ARRAY_BUFFER_BINDING, &currVBO);
            if ((uint32_t)currVBO == m_uVBO)
            {
                pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
            }
            pRenderer->DeleteBuffers(1, &m_uVBO);
        }
        if (m_uEBO != 0)
        {
            GLint currEBO = 0;
            pRenderer->GetIntegerV(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currEBO);
            if ((uint32_t)currEBO == m_uEBO)
            {
                pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            pRenderer->DeleteBuffers(1, &m_uEBO);
        }
    }
}

void CRenderBatch::SetStatic(bool bStatic)
{
    // TODO: some limited here! we can only set this flag to true only once!
    BEATS_ASSERT(bStatic && !m_bStatic && m_vertexData.GetBuffer() == NULL && m_indices.size() == 0);
    m_bStatic = bStatic;
    m_uVBO = 0;
    m_uEBO = 0;
    m_uVAO = 0;
    if (bStatic)
    {
        m_uStartPos = 0;
    }
}

bool CRenderBatch::IsStatic() const
{
    return  m_bStatic;
}

void CRenderBatch::SetAutoManage(bool bAutoManage)
{
    m_bAutoManage = bAutoManage;
}

bool CRenderBatch::IsAutoManage() const
{
    return m_bAutoManage;
}

void CRenderBatch::Render(CRenderGroup* pRenderGroup)
{
    BEATS_ASSERT(pRenderGroup != nullptr && pRenderGroup->ID() != LAYER_UNSET);
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderBatchRender);
    if (m_uDataSize > 0)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        if (m_uVBO == 0)
        {
            BEATS_ASSERT(m_uVAO == 0 && m_uEBO == 0);
            if (m_bStatic)
            {
                RefreshStaticBatch();
            }
            else
            {
                Reset();
            }
        }
        bool bUniformChanged = m_uniformMap.size() > 0;
        if (CRenderManager::GetInstance()->GetLastApplyMaterial().Get() != m_pMaterial.Get())
        {
            std::map<unsigned char, uint32_t>& textureBindingMap = pRenderer->GetCurrentState()->GetBindingTextureMap();
            for (auto iter = textureBindingMap.begin(); iter != textureBindingMap.end(); ++iter)
            {
                pRenderer->ActiveTexture(GL_TEXTURE0 + iter->first);
                pRenderer->BindTexture(GL_TEXTURE_2D, 0);
            }
            textureBindingMap.clear();
            m_pMaterial->Use();
            bUniformChanged = m_pMaterial->GetUniformMap().size() > 0;
            CRenderManager::GetInstance()->SetLastApplyMaterial(m_pMaterial);
        }
        CRenderManager::GetInstance()->ApplyTextureMap(m_textureMap, m_bTextureClampOrRepeat);
        for (auto iter : m_uniformMap)
        {
            iter.second.SendUniform();
        }
        if (CConfiguration::GetInstance()->SupportsShareableVAO())
        {
            BEATS_ASSERT(m_uVAO != 0);
            pRenderer->BindVertexArray(m_uVAO);
        }
        else
        {
            BEATS_ASSERT(m_uVBO != 0);
            m_pVertexFormat->SetupAttribPointer(m_uVBO);
            BEATS_ASSERT(m_bIndexed || m_uEBO == 0);
            pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uEBO);
        }
#ifdef ENABLE_SINGLE_MVP_UNIFORM
        CRenderManager::GetInstance()->SendMVPToShader(pRenderGroup->GetCameraType(), m_pWorldMat, m_pViewMat, m_pProjectMat);
#else
        CRenderManager::GetInstance()->SendMVPToShader(pRenderGroup->GetCameraType(), m_pWorldMat, nullptr, nullptr);
#endif
        if (m_pWorldMat)
        {
            m_matPool.push_back(m_pWorldMat);
            m_pWorldMat = nullptr;
        }
#ifdef ENABLE_SINGLE_MVP_UNIFORM
        if (m_pViewMat)
        {
            m_matPool.push_back(m_pViewMat);
            m_pViewMat = nullptr;
        }
        if (m_pProjectMat)
        {
            m_matPool.push_back(m_pProjectMat);
            m_pProjectMat = nullptr;
        }
#endif
        if (m_pPreRenderAction)
        {
            m_pPreRenderAction();
        }
        // TODO:HACK:Hard code to resolve mesh render conflict in universe scene.
        // because all static mesh use the same material, we can't specific a material for some of it.
        if (pRenderGroup->ID() >= LAYER_UNIVERSE_BUTTOM && pRenderGroup->ID() <= LAYER_UNIVERSE_STAR)
        {
            CRenderer::GetInstance()->DepthMask(false);
            CRenderer::GetInstance()->DisableGL(CBoolRenderStateParam::eBSP_DepthTest);
        }
        if (m_bIndexed)
        {
            BEATS_ASSERT(m_uStartPos != 0xFFFFFFFF);
            pRenderer->DrawElements(m_primitiveType, m_uDataSize / sizeof(unsigned short), GL_UNSIGNED_SHORT, (GLvoid*)m_uStartPos);
        }
        else
        {
#ifdef _DEBUG
            GLint currEBO = 0;
            pRenderer->GetIntegerV(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currEBO);
            BEATS_ASSERT(currEBO == 0);
#endif
            BEATS_ASSERT(m_uStartPos != 0xFFFFFFFF);
            uint32_t uVertexSize = m_pVertexFormat->Size();
            BEATS_ASSERT(uVertexSize > 0 && m_uDataSize % uVertexSize == 0 && m_uStartPos % uVertexSize == 0);
            pRenderer->DrawArrays(m_primitiveType, (GLint)m_uStartPos / uVertexSize, m_uDataSize / uVertexSize);
        }
        if (m_pPostRenderAction)
        {
            m_pPostRenderAction();
        }
        // Restore the uniform value for render environment
        if (bUniformChanged)
        {
            CRenderManager::GetInstance()->InitDefaultShaderUniform();
        }
        if (CConfiguration::GetInstance()->SupportsShareableVAO())
        {
            pRenderer->BindVertexArray(0);
        }
        else
        {
            m_pVertexFormat->DisableAttribPointer();
        }
        pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderBatchRender);
}

void CRenderBatch::SetVBO(uint32_t uVBO)
{
    m_uVBO = uVBO;
}

void CRenderBatch::SetEBO(uint32_t uEBO)
{
    m_uEBO = uEBO;
}

void CRenderBatch::SetVAO(uint32_t uVAO)
{
    m_uVAO = uVAO;
}

uint32_t CRenderBatch::GetVBO() const
{
    return m_uVBO;
}

uint32_t CRenderBatch::GetEBO() const
{
    return m_uEBO;
}

uint32_t CRenderBatch::GetVAO() const
{
    return m_uVAO;
}

void CRenderBatch::SetWorldTM(const CMat4* mat)
{
    BEATS_ASSERT(m_pWorldMat == nullptr);
    m_pWorldMat = RequestMat();
    *m_pWorldMat = *mat;
}

const CMat4* CRenderBatch::GetWorldTM() const
{
    return m_pWorldMat;
}

#ifdef ENABLE_SINGLE_MVP_UNIFORM
void CRenderBatch::SetViewTM(const CMat4* mat)
{
    BEATS_ASSERT(m_pViewMat == nullptr);
    m_pViewMat = RequestMat();
    *m_pViewMat = *mat;
}

const CMat4* CRenderBatch::GetViewTM() const
{
    return m_pViewMat;
}

void CRenderBatch::SetProjectionTM(const CMat4* mat)
{
    BEATS_ASSERT(m_pProjectMat == nullptr);
    m_pProjectMat = RequestMat();
    *m_pProjectMat = *mat;
}

const CMat4* CRenderBatch::GetProjectionTM() const
{
    return m_pProjectMat;
}
#endif

static void CopyAttribBuf2Buf(void *destBuffer, const void *srcBuffer,
                const CVertexFormat::SAttrib &vertexAttribute, uint32_t index)
{
    memcpy((char *)destBuffer + vertexAttribute.stride * index + vertexAttribute.offset,
        (const char *)srcBuffer + vertexAttribute.common.sizeInBytes * index,
        vertexAttribute.common.sizeInBytes);
}

static void CopyAttribVar2Buf(void *destBuffer, const void *srcVarPointer,
                const CVertexFormat::SAttrib &vertexAttribute, uint32_t index)
{
    memcpy((char *)destBuffer + vertexAttribute.stride * index + vertexAttribute.offset,
        (const char *)srcVarPointer, vertexAttribute.common.sizeInBytes);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, CColor color2, const CMat4 *pTransform/* = nullptr*/)
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
    for(uint32_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribBuf2Buf(quadBuffer, quadT, attribT, i);
        CopyAttribVar2Buf(quadBuffer, &color, attribC, i);
        CopyAttribVar2Buf(quadBuffer, &color2, attribCC, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, const CQuadT *quadT, CColor color, const CMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.sizeInBytes == sizeof(quadP->tl));

    const CVertexFormat::SAttrib &attribT = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_TEXCOORD0);
    BEATS_ASSERT(attribT.common.sizeInBytes == sizeof(quadT->tl));

    const CVertexFormat::SAttrib &attribC = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_COLOR);
    BEATS_ASSERT(attribC.common.sizeInBytes == sizeof(CColor));

    char quadBuffer[CVertexFormat::MAX_VERTEX_SIZE * 4];
    for(uint32_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribBuf2Buf(quadBuffer, quadT, attribT, i);
        CopyAttribVar2Buf(quadBuffer, &color, attribC, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, const CQuadT *quadT, const CMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.sizeInBytes == sizeof(quadP->tl));

    const CVertexFormat::SAttrib &attribT = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_TEXCOORD0);
    BEATS_ASSERT(attribT.common.sizeInBytes == sizeof(quadT->tl));

    char quadBuffer[CVertexFormat::MAX_VERTEX_SIZE * 4];
    for(uint32_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribBuf2Buf(quadBuffer, quadT, attribT, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const CQuadP *quadP, CColor color, const CMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.sizeInBytes == sizeof(quadP->tl));

    const CVertexFormat::SAttrib &attribC = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_COLOR);
    BEATS_ASSERT(attribC.common.sizeInBytes == sizeof(CColor));

    char quadBuffer[CVertexFormat::MAX_VERTEX_SIZE * 4];
    for(uint32_t i = 0; i < 4; ++i)
    {
        CopyAttribBuf2Buf(quadBuffer, quadP, attribP, i);
        CopyAttribVar2Buf(quadBuffer, &color, attribC, i);
    }
    AddQuad((const void *)quadBuffer, pTransform);
}

void CRenderBatch::AddQuad(const void *pQuad, const CMat4 *pTransform)
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

void CRenderBatch::AddVertices(const void *pVertices, uint32_t count, const CMat4 *pTransform)
{
    BEATS_ASSERT(!m_bIndexed);
    CSerializer* pVertexBuffer = GetVertexBuffer();
    if (m_uStartPos == 0xFFFFFFFF)
    {
        m_uStartPos = pVertexBuffer->GetWritePos();
    }
    AddVerticesImpl(pVertices, count, pTransform);
    m_uDataSize += m_pVertexFormat->Size() * count;
}

void CRenderBatch::AddIndexedVertices(const void *pVertices, uint32_t vertexCount,
                                    const unsigned short *pIndices, uint32_t indexCount,
                                    const CMat4 *pTransform /* = nullptr */)
{
    BEATS_ASSERT(m_bIndexed);
    uint32_t currVertexCount = GetVertexCount();
    AddVerticesImpl(pVertices, vertexCount, pTransform);
    uint32_t uIndicesSize = indexCount * sizeof (const unsigned short);
    m_uDataSize += uIndicesSize;
    for (uint32_t i = 0; i < indexCount; ++i)
    {
        BEATS_ASSERT(pIndices[i] < vertexCount);
        BEATS_ASSERT(currVertexCount < 0xFFFF, "index overflow!");
        unsigned short newIndex = pIndices[i] + (unsigned short)currVertexCount;
        m_indices.push_back(newIndex);
    }
}

void CRenderBatch::AddVerticesImpl(const void *pVertices, uint32_t count, const CMat4 *pTransform)
{
    const CVertexFormat::SAttrib &attribP = m_pVertexFormat->GetAttrib(ATTRIB_INDEX_POSITION);
    BEATS_ASSERT(attribP.common.size == 3 && attribP.common.type == GL_FLOAT);
    uint32_t dataSize = m_pVertexFormat->Size() * count;
    uint32_t uWritePos = m_vertexData.GetWritePos();
    m_vertexData.Serialize(pVertices, dataSize);
    if(pTransform || m_bShouldScaleContent)
    {
        const unsigned char* pVertexBufferReader = m_vertexData.GetBuffer();
        pVertexBufferReader += uWritePos;
        float fScale = CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetScaleFactor();
        if ((m_bShouldScaleContent && fScale != 1.0f) || pTransform != NULL)
        {
            for (uint32_t i = 0; i < count; ++i)
            {
                CVec3 *pPosition = (CVec3 *)&(pVertexBufferReader[i * m_pVertexFormat->Size() + attribP.offset]);
                if (pTransform)
                {
                    (*pPosition) *= (*pTransform);
                }
                if (m_bShouldScaleContent && fScale != 1.0f)
                {
                    pPosition->X() *= fScale;
                    pPosition->Y() *= fScale;
                }
            }
        }
    }
}

CSerializer* CRenderBatch::GetVertexBuffer()
{
    return &m_vertexData;
}

CMat4* CRenderBatch::RequestMat()
{
    CMat4* pRet = nullptr;
    if (m_matPool.empty())
    {
        pRet = new CMat4;
    }
    else
    {
        pRet = m_matPool.back();
        m_matPool.pop_back();
    }
    return pRet;
}

void CRenderBatch::DestroyMatPool()
{
    for (size_t i = 0; i < m_matPool.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_matPool[i]);
    }
    m_matPool.clear();
}

uint32_t CRenderBatch::GetVertexCount()
{
    const CSerializer* pVertexBuffer = GetVertexBuffer();
    uint32_t uVertexBufferSize = pVertexBuffer->GetWritePos();
    BEATS_ASSERT(uVertexBufferSize % m_pVertexFormat->Size() == 0 && m_pVertexFormat->Size() > 0);
    uint32_t uRet = uVertexBufferSize / m_pVertexFormat->Size();
    return uRet;
}

uint32_t CRenderBatch::GetDataSize() const
{
    return m_uDataSize;
}

void CRenderBatch::SetDataSize(uint32_t uDataSize)
{
    m_uDataSize = uDataSize;
}

uint32_t CRenderBatch::GetStartPos() const
{
    return m_uStartPos;
}

void CRenderBatch::SetStartPos(uint32_t uStartPos)
{
    m_uStartPos = uStartPos;
}

void CRenderBatch::Clear()
{
    m_bTextureClampOrRepeat = true;
    m_uStartPos = 0xFFFFFFFF;
    m_uDataSize = 0;
    m_textureMap.clear();
    m_uniformMap.clear();
    m_pPreRenderAction = nullptr;
    m_pPostRenderAction = nullptr;
    SetRefBatch(nullptr);
    m_vertexData.Reset();
    m_indices.clear();
#ifdef DEVELOP_VERSION
    m_usage = ERenderBatchUsage::eRBU_Count;
#endif
}

bool CRenderBatch::GetShouldScaleContent() const
{
    return m_bShouldScaleContent;
}

const CVertexFormat &CRenderBatch::GetVertexFormat() const
{
    return *m_pVertexFormat;
}

void CRenderBatch::SetVertexFormat(const CVertexFormat &vertexFormat)
{
    BEATS_ASSERT( m_uDataSize == 0, 
        _T("Can't set material of a render batch which already been used!"));
    if(*m_pVertexFormat != vertexFormat)
    {
        m_pVertexFormat = &vertexFormat;
    }
}

SharePtr<CMaterial> CRenderBatch::GetMaterial() const
{
    return m_pMaterial;
}

void CRenderBatch::SetMaterial(SharePtr<CMaterial> pMaterial)
{
    m_pMaterial = pMaterial;
}

GLenum CRenderBatch::GetPrimitiveType() const
{
    return m_primitiveType;
}

void CRenderBatch::SetPrimitiveType(GLenum primitiveType)
{
    BEATS_ASSERT(m_uDataSize == 0,
        _T("Can't set primitive type of a render batch which already been used!"));
    m_primitiveType = primitiveType;
}

void CRenderBatch::SetTextureClampOrRepeat(bool bClamp)
{
    m_bTextureClampOrRepeat = bClamp;
}

bool CRenderBatch::GetTextureClampOrRepeat() const
{
    return m_bTextureClampOrRepeat;
}

void CRenderBatch::Reset()
{
    bool bHandled = false;
    if (m_pRefBatch != nullptr)
    {
        if (m_pRefBatch->IsStatic())
        {
            if (m_pRefBatch->GetVBO() == 0)
            {
                m_pRefBatch->RefreshStaticBatch();
            }
            m_uVAO = m_pRefBatch->GetVAO();
            m_uVBO = m_pRefBatch->GetVBO();
            m_uEBO = m_pRefBatch->GetEBO();
            m_uDataSize = m_pRefBatch->GetDataSize();
            m_uStartPos = m_pRefBatch->GetStartPos();
            bHandled = true;
        }
    }
    if (!bHandled)
    {
        CVertexBufferContent* pBufferContent = CRenderManager::GetInstance()->GetVertexContent(m_pVertexFormat, m_primitiveType);
        if (CConfiguration::GetInstance()->SupportsShareableVAO())
        {
            m_uVAO = m_bIndexed ? pBufferContent->GetIndexVAO() : pBufferContent->GetArrayVAO();
        }
        m_uVBO = pBufferContent->GetVBO();
        m_uEBO = m_bIndexed ? pBufferContent->GetEBO() : 0;
    }
}

void CRenderBatch::RefreshStaticBatch()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    // Create buffer objects for this static batch.
    if (m_uVBO == 0)
    {
        pRenderer->GenBuffers(1, &m_uVBO);
    }
    if (m_bIndexed && m_uEBO == 0)
    {
        pRenderer->GenBuffers(1, &m_uEBO);
    }
    if (CConfiguration::GetInstance()->SupportsShareableVAO())
    {
        if (m_uVAO == 0)
        {
            pRenderer->GenVertexArrays(1, &m_uVAO);
        }
        pRenderer->BindVertexArray(m_uVAO);
    }
    m_pVertexFormat->SetupAttribPointer(m_uVBO);
    if (m_uDataSize == 0)
    {
        pRenderer->BufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uEBO);
        if (m_uEBO != 0)
        {
            pRenderer->BufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        }
    }
    else
    {
        pRenderer->BufferData(GL_ARRAY_BUFFER, m_vertexData.GetWritePos(), m_vertexData.GetBuffer(), GL_STATIC_DRAW);
        m_vertexData.Reset();//save the memory because it is useless.
        BEATS_ASSERT(m_bIndexed || m_uEBO == 0);
        pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uEBO);
        if (m_uEBO != 0)
        {
            BEATS_ASSERT(m_indices.size() > 0);
            pRenderer->BufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(m_indices[0]), m_indices.data(), GL_STATIC_DRAW);
            m_indices.clear();//save the memory because it is useless.
        }
    }

    if (CConfiguration::GetInstance()->SupportsShareableVAO())
    {
        pRenderer->BindVertexArray(0);
    }
    m_pVertexFormat->DisableAttribPointer();
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
}

void CRenderBatch::SetTexture(unsigned char uChannel, const SharePtr<CTexture>& texture)
{
    m_textureMap[uChannel] = texture;
}

const std::map<unsigned char, SharePtr<CTexture> >& CRenderBatch::GetTextureMap() const
{
    return m_textureMap;
}

void CRenderBatch::SetTextureMap(const std::map<unsigned char, SharePtr<CTexture> >& textureMap)
{
    m_textureMap = textureMap;
}

std::map<TString, CShaderUniform>& CRenderBatch::GetUniformMap()
{
    return m_uniformMap;
}

void CRenderBatch::SetUniformMap(const std::map<TString, CShaderUniform>& uniformMap)
{
    m_uniformMap = uniformMap;
}

void CRenderBatch::SetRefBatch(CRenderBatch* pBatch)
{
    m_pRefBatch = pBatch;
}

void CRenderBatch::SetPreRenderAction(std::function<void()> function)
{
    m_pPreRenderAction = function;
}

void CRenderBatch::SetPostRenderAction(std::function<void()> function)
{
    m_pPostRenderAction = function;
}

bool CRenderBatch::IsIndexed() const
{
    return m_bIndexed;
}

void CRenderBatch::SyncData()
{
    if (!m_bStatic && m_pRefBatch == nullptr && m_uDataSize > 0)
    {
        CVertexBufferContent* pBufferContent = CRenderManager::GetInstance()->GetVertexContent(m_pVertexFormat, m_primitiveType);
        BEATS_ASSERT(pBufferContent != nullptr);
        CSerializer* pVertexBuffer = pBufferContent->GetVertexBuffer();
        m_uStartPos = pVertexBuffer->GetWritePos();
        uint32_t uVertexBufferSize = pVertexBuffer->GetWritePos();
        BEATS_ASSERT(uVertexBufferSize % m_pVertexFormat->Size() == 0 && m_pVertexFormat->Size() > 0);
        uint32_t uCurrVertexCount = uVertexBufferSize / m_pVertexFormat->Size();
        pVertexBuffer->Serialize(m_vertexData.GetBuffer(), m_vertexData.GetWritePos());
        if (m_bIndexed)
        {
            BEATS_ASSERT(m_indices.size() > 0);
            CSerializer* pIndexBuffer = pBufferContent->GetIndexBuffer();
            m_uStartPos = pIndexBuffer->GetWritePos();
            for (size_t i = 0; i < m_indices.size(); ++i)
            {
                BEATS_ASSERT(uCurrVertexCount + m_indices[i] < 0xFFFF, "index overflow!");
                m_indices[i] += (unsigned short)uCurrVertexCount;
            }
            pIndexBuffer->Serialize(m_indices.data(), m_indices.size() * sizeof(m_indices[0]));
        }
    }
}
