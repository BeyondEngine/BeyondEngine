#include "stdafx.h"
#include "Skin.h"
#include "Renderer.h"
#include "VertexFormat.h"
#include "external/Configuration.h"

CSkin::CSkin()
: m_pVertexData(nullptr)
, m_pIndexData(nullptr)
, m_uVAO(0)
, m_uVBO(0)
, m_uEBO(0)
, m_bIsInitialized(false)
{
}

CSkin::~CSkin()
{
    BEATS_SAFE_DELETE(m_pVertexData);
    BEATS_SAFE_DELETE(m_pIndexData);
}

bool CSkin::Load(CSerializer& serializer)
{
    // Load From File
    BEATS_ASSERT(m_pVertexData == nullptr && m_pIndexData == nullptr);
    m_pVertexData = new CSerializer;
    m_pIndexData = new CSerializer;
    uint32_t uMeshcount = 0;
    serializer >> uMeshcount;
    for (uint32_t i = 0; i < uMeshcount; ++i)
    {
        uint32_t uMaterialCount = 0;
        serializer >> uMaterialCount;
        BEATS_ASSERT(uMaterialCount == 1, _T("Only support one material!"));
        SSkinBatchInfo* pSkinBatchInfo = nullptr;
        for (uint32_t j = 0; j < uMaterialCount; ++j)
        {
            TString strTextureName;
            serializer >> strTextureName;
            BEATS_ASSERT(!strTextureName.empty(), _T("Texutre can't be empty in skin!"));
            pSkinBatchInfo = &m_batchInfoMap[strTextureName];
        }
        BEATS_ASSERT(pSkinBatchInfo != nullptr);
        uint32_t uVertexCount = 0;
        serializer >> uVertexCount;
#ifdef DEVELOP_VERSION
        m_uVertexCount = uVertexCount;
#endif
        for (uint32_t k = 0; k < uVertexCount; ++k)
        {
            CVertexPTB vertexPTB;
            serializer >> vertexPTB.position.X() >> vertexPTB.position.Y() >> vertexPTB.position.Z() >> vertexPTB.tex.u >> vertexPTB.tex.v;
            uint8_t uBoneCount = 0;
            serializer >> uBoneCount;
            for (uint8_t z = 0; z < uBoneCount; ++z)
            {
                int8_t nIndex = 0;
                float fWeight = 0;
                serializer >> nIndex >> fWeight;
                float* pIndexFloat = nullptr;
                float* pWeightData = nullptr;
                if (z == 0)
                {
                    pIndexFloat = &vertexPTB.bones.X();
                    pWeightData = &vertexPTB.weights.X();
                }
                else if (z == 1)
                {
                    pIndexFloat = &vertexPTB.bones.Y();
                    pWeightData = &vertexPTB.weights.Y();
                }
                else if (z == 2)
                {
                    pIndexFloat = &vertexPTB.bones.Z();
                    pWeightData = &vertexPTB.weights.Z();
                }
                else if (z == 3)
                {
                    pIndexFloat = &vertexPTB.bones.W();
                    pWeightData = &vertexPTB.weights.W();
                }
                else
                {
                    BEATS_ASSERT(false, "Never reach here!");
                }
                *pIndexFloat = (float)nIndex;
                *pWeightData = fWeight;
            }
            (*m_pVertexData) << vertexPTB;
#ifdef _DEBUG
            float sum = vertexPTB.weights.X() + vertexPTB.weights.Y() + vertexPTB.weights.Z() + vertexPTB.weights.W();
            BEATS_ASSERT(sum < 1.01F, _T("Weight can't be greater than 1.01F, cur Value : %f!"), sum);
            BEATS_WARNING(sum > 0.99F, _T("Weight can't be smaller than 0.99F, cur Value : %f!"), sum);
#endif
        }
        uint32_t uIndexCount = 0;
        serializer >> uIndexCount;
        pSkinBatchInfo->m_uStartPos = m_pIndexData->GetWritePos();
        pSkinBatchInfo->m_uDataSize = uIndexCount * sizeof(unsigned short);
        for (uint32_t l = 0; l < uIndexCount; ++l)
        {
            int nIndex = 0;
            serializer >> nIndex;
            BEATS_ASSERT(nIndex < 0xFFFF, _T("Invalid index %d"), nIndex);
            (*m_pIndexData) << (unsigned short)nIndex;
        }
    }
    return true;
}

void CSkin::Initialize()
{
    BEATS_ASSERT(!m_bIsInitialized);
    BEATS_ASSERT(m_uEBO == 0 && m_uVBO == 0 && m_uVAO == 0);
    CRenderer* pRenderer = CRenderer::GetInstance();
    // Create buffer objects for this static batch.
    pRenderer->GenBuffers(1, &m_uVBO);
    pRenderer->GenBuffers(1, &m_uEBO);
    if (CConfiguration::GetInstance()->SupportsShareableVAO())
    {
        pRenderer->GenVertexArrays(1, &m_uVAO);
        pRenderer->BindVertexArray(m_uVAO);
    }
    const CVertexFormat& vertexFormat = VERTEX_FORMAT(CVertexPTB);
    vertexFormat.SetupAttribPointer(m_uVBO);
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uEBO);
    pRenderer->BufferData(GL_ARRAY_BUFFER, m_pVertexData->GetWritePos(), m_pVertexData->GetBuffer(), GL_STATIC_DRAW);
    BEATS_SAFE_DELETE(m_pVertexData); //save the memory because it is useless.
    pRenderer->BufferData(GL_ELEMENT_ARRAY_BUFFER, m_pIndexData->GetWritePos(), m_pIndexData->GetBuffer(), GL_STATIC_DRAW);
    BEATS_SAFE_DELETE(m_pIndexData); //save the memory because it is useless.
    if (CConfiguration::GetInstance()->SupportsShareableVAO())
    {
        pRenderer->BindVertexArray(0);
    }
    vertexFormat.DisableAttribPointer();
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    m_bIsInitialized = true;
}

void CSkin::Uninitialize()
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
        m_uVAO = 0;
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
        m_uVBO = 0;
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
        m_uEBO = 0;
    }
    m_bIsInitialized = false;
}

uint32_t CSkin::GetVAO() const
{
    return m_uVAO;
}

uint32_t CSkin::GetVBO() const
{
    return m_uVBO;
}

uint32_t CSkin::GetEBO() const
{
    return m_uEBO;
}

const std::map<std::string, SSkinBatchInfo>& CSkin::GetBatchInfoMap() const
{
    return m_batchInfoMap;
}

bool CSkin::IsInitialized() const
{
    return m_bIsInitialized;
}