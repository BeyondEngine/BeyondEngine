#include "stdafx.h"
#include "VertexBufferContent.h"
#include "Renderer.h"

CVertexBufferContent::CVertexBufferContent()
{

}

CVertexBufferContent::~CVertexBufferContent()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    if (m_uArrayVAO != 0)
    {
        GLint currVAO = 0;
#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
        pRenderer->GetIntegerV(GL_VERTEX_ARRAY_BINDING_OES, &currVAO);
#else
        pRenderer->GetIntegerV(GL_VERTEX_ARRAY_BINDING, &currVAO);
#endif
        if ((uint32_t)currVAO == m_uArrayVAO)
        {
            pRenderer->BindVertexArray(0);
        }
        pRenderer->DeleteVertexArrays(1, &m_uArrayVAO);
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
    BEATS_SAFE_DELETE(m_pIndexBuffer);
    BEATS_SAFE_DELETE(m_pVertexBuffer);
}

CSerializer* CVertexBufferContent::GetVertexBuffer()
{
    if (m_pVertexBuffer == nullptr)
    {
        m_pVertexBuffer = new CSerializer;
    }
    return m_pVertexBuffer;
}

CSerializer* CVertexBufferContent::GetIndexBuffer()
{
    if (m_pIndexBuffer == nullptr)
    {
        m_pIndexBuffer = new CSerializer;
    }
    return m_pIndexBuffer;
}

uint32_t CVertexBufferContent::GetArrayVAO() const
{
    return m_uArrayVAO;
}

void CVertexBufferContent::SetArrayVAO(uint32_t uVAO)
{
    m_uArrayVAO = uVAO;
}

uint32_t CVertexBufferContent::GetIndexVAO() const
{
    return m_uIndexVAO;
}

void CVertexBufferContent::SetIndexVAO(uint32_t uIndexVAO)
{
    m_uIndexVAO = uIndexVAO;
}

uint32_t CVertexBufferContent::GetVBO() const
{
    return m_uVBO;
}

void CVertexBufferContent::SetVBO(uint32_t uVBO)
{
    m_uVBO = uVBO;
}

uint32_t CVertexBufferContent::GetEBO() const
{
    return m_uEBO;
}

void CVertexBufferContent::SetEBO(uint32_t uEBO)
{
    m_uEBO = uEBO;
}

void CVertexBufferContent::SyncData()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderBatchUpdateVBO);
    if (m_pVertexBuffer->GetWritePos() > 0)
    {
        CRenderer *pRenderer = CRenderer::GetInstance();

        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderBatchUpdateVBO0);
        pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uVBO);
        BEATS_ASSERT(m_pVertexBuffer->GetWritePos() > 0);
        pRenderer->BufferData(GL_ARRAY_BUFFER, m_pVertexBuffer->GetWritePos(), m_pVertexBuffer->GetBuffer(), GL_DYNAMIC_DRAW);
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderBatchUpdateVBO0);

        if (m_pIndexBuffer != NULL && m_pIndexBuffer->GetWritePos() > 0)
        {
            BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderBatchUpdateVBO1);
            pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uEBO);
            pRenderer->BufferData(GL_ELEMENT_ARRAY_BUFFER, m_pIndexBuffer->GetWritePos(), m_pIndexBuffer->GetBuffer(), GL_DYNAMIC_DRAW);
            BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderBatchUpdateVBO1);
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderBatchUpdateVBO);
}

void CVertexBufferContent::Reset()
{
    if (m_pVertexBuffer)
    {
        m_pVertexBuffer->Reset();
    }
    if (m_pIndexBuffer)
    {
        m_pIndexBuffer->Reset();
    }
}