#include "stdafx.h"
#include "RenderGroup.h"
#include "RenderBatch.h"
#include "Material.h"
#include "Texture.h"
#include "RenderManager.h"
#include "Renderer.h"
//#define ENABLE_BATCH_CHECK
#ifdef DEVELOP_VERSION
bool CRenderGroup::m_bNewBatchCheck = false;
TString CRenderGroup::m_strBatchCheckTag;
#endif

CRenderGroup::CRenderGroup(ERenderGroupID ID, bool bShouldScaleContent/* = false*/)
    : m_nID(ID)
    , m_bShouldScaleContent(bShouldScaleContent)
{

}

CRenderGroup::~CRenderGroup()
{
    for (auto iter = m_batches.begin(); iter != m_batches.end(); ++iter)
    {
        for (size_t i = 0; i < iter->second.size(); ++i)
        {
            BEATS_SAFE_DELETE(iter->second[i]);
        }
    }
    m_batches.clear();
    BEATS_SAFE_DELETE_VECTOR(m_batchPool[0]);
    BEATS_SAFE_DELETE_VECTOR(m_batchPool[1]);
}

void CRenderGroup::Render()
{
    if (m_bClearDepthBuffer)
    {
        CRenderer::GetInstance()->DepthMask(true);
        CRenderer::GetInstance()->ClearDepth(1.0f);
        CRenderer::GetInstance()->ClearBuffer(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    // HACK: try to find a better way
    float fOriginalFov = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->GetFOV();
    if (m_nID == LAYER_3D_BridgeAnimation)
    {
        CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->SetFOV(35.f);
    }
    for (auto iter = m_batches.begin(); iter != m_batches.end(); ++iter)
    {
        const std::vector<CRenderBatch*>& batches = iter->second;
        for (uint32_t i = 0; i < batches.size(); ++i)
        {
            batches[i]->Render(this);
#ifdef DEVELOP_VERSION
            ++CEngineCenter::GetInstance()->m_renderBatchCount[batches[i]->m_usage];
#endif
        }
    }
    if (m_nID == LAYER_3D_BridgeAnimation)
    {
        CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->SetFOV(fOriginalFov);
    }
}

ERenderGroupID CRenderGroup::ID() const
{
    return m_nID;
}

void CRenderGroup::AddRenderBatch(CRenderBatch *object)
{
    ERenderGroupSubID subID = eRGSI_Solid;
    if (object->GetMaterial()->GetBlendEnable())
    {
        subID = eRGSI_Alpha;
    }
    std::vector<CRenderBatch*>& batches = m_batches[subID];
    BEATS_ASSERT(std::find(batches.begin(), batches.end(), object) == batches.end());
    batches.push_back(object);
}

void CRenderGroup::Clear()
{
    for (auto iter = m_batches.begin(); iter != m_batches.end(); ++iter)
    {
        const std::vector<CRenderBatch*>& batches = iter->second;
        for (uint32_t i = 0; i < batches.size(); ++i)
        {
            if (!batches[i]->IsStatic())
            {
                batches[i]->Clear();
                if (batches[i]->IsAutoManage())
                {
                    batches[i]->SetMaterial(nullptr);
                    std::vector<CRenderBatch *>& renderBatchPool = m_batchPool[batches[i]->IsIndexed() ? 1 : 0];
                    BEATS_ASSERT(std::find(renderBatchPool.begin(), renderBatchPool.end(), batches[i]) == renderBatchPool.end());
                    renderBatchPool.push_back(batches[i]);
                }
            }
        }
    }
    m_batches.clear();
}

void CRenderGroup::SyncData() const
{
    for (auto iter = m_batches.begin(); iter != m_batches.end(); ++iter)
    {
        const std::vector<CRenderBatch*>& batches = iter->second;
        for (uint32_t i = 0; i < batches.size(); ++i)
        {
            batches[i]->SyncData();
        }
    }
}

CRenderBatch* CRenderGroup::GetRenderBatch(const CVertexFormat &format,
                                                SharePtr<CMaterial> material,
                                                GLenum primitiveType, bool bIndexed, bool bTextureClampOrRepeat, const std::map<unsigned char, SharePtr<CTexture> >* pTextureMap,
                                                const std::map<TString, CShaderUniform>* pUniformMap)
{
    BEATS_ASSERT(std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId(), _T("Only main thread can get batch from render group."));
    BEATS_ASSERT(material != NULL, _T("Material can't be null when get render batch."));
    CRenderBatch* pBatch = nullptr;
    ERenderGroupSubID subId = eRGSI_Solid;
    if (material->GetBlendEnable())
    {
        subId = eRGSI_Alpha;
    }
    bool bNeedNewBatch = m_batches.empty() || m_batches[subId].empty();
    if (!bNeedNewBatch)
    {
        // We need to keep the render order, so only compare with the last batch, to check if we can share the batch.
        pBatch = static_cast<CRenderBatch *>(m_batches[subId].back());
        bNeedNewBatch = bTextureClampOrRepeat != pBatch->GetTextureClampOrRepeat()||
                            !pBatch->GetVertexFormat().Compatible(format) ||
                            pBatch->GetMaterial().Get() != material.Get() ||
                            pBatch->GetPrimitiveType() != primitiveType ||
                            pBatch->IsIndexed() != bIndexed;
#if defined(DEVELOP_VERSION) && defined(ENABLE_BATCH_CHECK)
        BEATS_ASSERT(!m_bNewBatchCheck || !bNeedNewBatch, "New render batch check is triggered, material is different! Tag: %s", m_strBatchCheckTag.c_str());
#endif
        if (!bNeedNewBatch && pTextureMap != nullptr)
        {
            const std::map<unsigned char, SharePtr<CTexture> >& textureMap = pBatch->GetTextureMap();
            for (auto iter = pTextureMap->begin(); iter != pTextureMap->end(); ++iter)
            {
                auto textureIter = textureMap.find(iter->first);
                if (textureIter == textureMap.end() || textureIter->second->ID() != iter->second->ID())
                {
                    bNeedNewBatch = true;
#if defined(DEVELOP_VERSION) && defined(ENABLE_BATCH_CHECK)
                    BEATS_ASSERT(!m_bNewBatchCheck || !bNeedNewBatch, "New render batch check is triggered, texture is different:\n%s\nTag: %s", textureIter->second->GetFilePath().c_str(), 
                        m_strBatchCheckTag.c_str());
#endif
                    break;
                }
            }
        }
        if (!bNeedNewBatch && pUniformMap != nullptr)
        {
            const std::map<TString, CShaderUniform>& uniformMap = pBatch->GetUniformMap();
            for (auto iter = pUniformMap->begin(); iter != pUniformMap->end(); ++iter)
            {
                auto uniformIter = uniformMap.find(iter->first);
                if (uniformIter == uniformMap.end() || uniformIter->second != iter->second)
                {
                    bNeedNewBatch = true;
#if defined(DEVELOP_VERSION) && defined(ENABLE_BATCH_CHECK)
                    BEATS_ASSERT(!m_bNewBatchCheck || !bNeedNewBatch, "New render batch check is triggered, uniform is different! Tag: %s", m_strBatchCheckTag.c_str());
#endif
                    break;
                }
            }
        }
    }
    if (bNeedNewBatch)
    {
        pBatch = GetRenderBatchImpl(format, material, primitiveType, bIndexed);
        pBatch->SetTextureClampOrRepeat(bTextureClampOrRepeat);
        if (pTextureMap != NULL)
        {
            pBatch->SetTextureMap(*pTextureMap);
        }
        if (pUniformMap != NULL)
        {
            pBatch->SetUniformMap(*pUniformMap);
        }
        AddRenderBatch(pBatch);
    }
    return pBatch;
}

CRenderBatch* CRenderGroup::GetProxyBatch(CRenderBatch* pRefBatch)
{
    CRenderBatch* pBatch = GetRenderBatchImpl(pRefBatch->GetVertexFormat(), pRefBatch->GetMaterial(), pRefBatch->GetPrimitiveType(), pRefBatch->IsIndexed());
    pBatch->SetEBO(pRefBatch->GetEBO());
    pBatch->SetVBO(pRefBatch->GetVBO());
    pBatch->SetVAO(pRefBatch->GetVAO());
    pBatch->SetDataSize(pRefBatch->GetDataSize());
    pBatch->SetStartPos(pRefBatch->GetStartPos());
    pBatch->SetTextureMap(pRefBatch->GetTextureMap());
    pBatch->SetUniformMap(pRefBatch->GetUniformMap());
    pBatch->SetAutoManage(true);
    pBatch->SetRefBatch(pRefBatch);
    pBatch->SetTextureClampOrRepeat(pRefBatch->GetTextureClampOrRepeat());
    AddRenderBatch(pBatch);
    return pBatch;
}

CCamera::ECameraType CRenderGroup::GetCameraType() const
{
    CCamera::ECameraType ret = CCamera::eCT_2D;
    if ((LAYER_3D_MIN < m_nID &&
        m_nID < LAYER_3D_MAX) || m_nID == LAYER_USER)
    {
        ret = CCamera::eCT_3D;
    }
    else if ((LAYER_2D_MIN < m_nID && m_nID < LAYER_2D_MAX) ||
        m_nID == LAYER_BACKGROUND)
    {
        ret = CCamera::eCT_2D;
    }
    else
    {
        BEATS_ASSERT(false, _T("Never reach here!"));
    }
    return ret;
}

void CRenderGroup::SetClearDepthFlag(bool bClearFlag)
{
    m_bClearDepthBuffer = bClearFlag;
}

bool CRenderGroup::GetClearDepthFlag() const
{
    return m_bClearDepthBuffer;
}

CRenderBatch *CRenderGroup::GetRenderBatchImpl( const CVertexFormat &format,
                                                    SharePtr<CMaterial> material, 
                                                    GLenum primitiveType, bool bIndexed)
{
    CRenderBatch *batch = nullptr;
    std::vector<CRenderBatch *> &batchPool = m_batchPool[bIndexed ? 1 : 0];
    if(batchPool.empty())
    {
        batch = new CRenderBatch(format, material, primitiveType, bIndexed, m_bShouldScaleContent);
        batch->SetAutoManage(true);
    }
    else
    {
        batch = batchPool.back();
        batch->SetVertexFormat(format);
        batch->SetMaterial(material);
        batch->SetPrimitiveType(primitiveType);
        batch->Reset();
        batchPool.pop_back();
    }
#ifdef ENABLE_SINGLE_MVP_UNIFORM
    BEATS_ASSERT(batch->GetViewTM() == nullptr && batch->GetProjectionTM() == nullptr && batch->GetWorldTM() == nullptr);
#endif
    BEATS_ASSERT(batch->IsAutoManage());
    return batch;
}