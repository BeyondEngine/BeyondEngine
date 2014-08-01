#include "stdafx.h"
#include "RenderGroup.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "RenderGroupManager.h"
#include "RenderManager.h"
#include "Render/RenderTarget.h"
#include "GUI/WindowManager.h"
#include "RenderBatch.h"
#include "Material.h"

CRenderGroup::CRenderGroup(CRenderGroupManager::ERenderGroupID ID, bool bShouldScaleContent/* = false*/)
    : m_nID(ID)
    , m_bShouldScaleContent(bShouldScaleContent)
{

}

CRenderGroup::~CRenderGroup()
{
}

bool CRenderGroup::PreRender()
{
    bool bCameraValid = false;
    CCamera *pCamera = nullptr;

    if (CRenderGroupManager::LAYER_3D_MIN < m_nID && 
        m_nID < CRenderGroupManager::LAYER_3D_MAX)
    {
        pCamera = CRenderGroupManager::GetInstance()->GetDefault3DCamera();
    }
    else if ((CRenderGroupManager::LAYER_2D_MIN < m_nID && m_nID < CRenderGroupManager::LAYER_2D_MAX) || 
        (CRenderGroupManager::LAYER_GUI_MIN < m_nID && m_nID < CRenderGroupManager::LAYER_GUI_MAX))
    {
        pCamera = CRenderGroupManager::GetInstance()->GetDefault2DCamera();
    }
    else
    {
        BEATS_ASSERT(false, _T("Never reach here!"));
    }
    BEATS_ASSERT(pCamera != NULL);
    if(pCamera)
    {
        bCameraValid = true;
        CRenderManager::GetInstance()->SetCurrrentCamera(pCamera);
    }
    return bCameraValid;
}

void CRenderGroup::Render()
{
    for(auto batch : m_batches)
    {
        batch->Render();
    }
}

void CRenderGroup::PostRender()
{

}

CRenderGroupManager::ERenderGroupID CRenderGroup::ID() const
{
    return m_nID;
}

void CRenderGroup::AddRenderBatch(CRenderBatch *object)
{
    m_batches.push_back(object);
}

void CRenderGroup::Clear()
{
    for (size_t i = 0; i < m_batches.size(); ++i)
    {
        if (!m_batches[i]->IsStatic())
        {
            m_batches[i]->Clear();
            if( !m_batches[i]->IsRecycled() )
            {
                m_batches[i]->SetRecycled(true);
                m_batchPool[m_batches[i]->IsIndexed() ? 1 : 0].push_back(m_batches[i]);
            }
        }
    }
    m_batches.clear();
}

CRenderBatch* CRenderGroup::GetRenderBatch(const CVertexFormat &format,
                                                SharePtr<CMaterial> material,
                                                GLenum primitiveType, bool bIndexed)
{
    BEATS_ASSERT(material != NULL, _T("Material can't be null when get render batch."));
    CRenderBatch *batch = nullptr;
    if(m_batches.empty())
    {
        batch = GetRenderBatchImpl(format, material, primitiveType, bIndexed);
        AddRenderBatch(batch);
    }
    else
    {
        // We need to keep the render order, so only compare with the last batch, to check if we can share the batch.
        batch = static_cast<CRenderBatch *>(m_batches.back());
        if( !batch->GetVertexFormat().Compatible(format) ||
            *batch->GetMaterial() != *material ||
            batch->GetPrimitiveType() != primitiveType ||
            batch->IsIndexed() != bIndexed ||
            batch->IsCompleted())
        {
            batch = GetRenderBatchImpl(format, material, primitiveType, bIndexed);
            AddRenderBatch(batch);
        }
    }
    return batch;
}

CRenderBatch *CRenderGroup::GetRenderBatchImpl(const CVertexFormat &format,
                                                    SharePtr<CMaterial> material, 
                                                    GLenum primitiveType, bool bIndexed)
{
    CRenderBatch *batch = nullptr;
    std::vector<CRenderBatch *> &batchPool = m_batchPool[bIndexed ? 1 : 0];
    if(batchPool.empty())
    {
        batch = new CRenderBatch(format, material, primitiveType, bIndexed, true, m_bShouldScaleContent);
#ifdef _DEBUG
        batch->SetRenderBatchID(m_nID + m_curBatchID++);
#endif
    }
    else
    {
        batch = batchPool.back();
        BEATS_ASSERT(batch->IsRecycled());
        batch->SetVertexFormat(format);
        batch->SetMaterial(material);
        batch->SetPrimitiveType(primitiveType);
        batch->SetRecycled(false);
        batchPool.pop_back();
    }
    return batch;
}