#include "stdafx.h"
#include "StaticMesh.h"
#include "Material.h"
#include "Resource/ResourceManager.h"
#include "RenderBatch.h"
#include "RenderGroup.h"
#include "StaticMeshData.h"
#include "Renderer.h"
#include "NodeAnimation/NodeAnimationManager.h"
#include "NodeAnimation/NodeAnimationData.h"
#include "RenderManager.h"
#include "ShapeRenderer.h"

CStaticMesh::CStaticMesh()
: m_animationType(eNAPT_NONE)
, m_uSpecifiedFrame(0)
{
    m_defaultGroupID = LAYER_3D;
}

CStaticMesh::~CStaticMesh()
{
    for (size_t i = 0; i < m_meshAnimationList.size(); ++i)
    {
        CNodeAnimationManager::GetInstance()->DeleteNodeAnimation(m_meshAnimationList[i]);
    }
}

void CStaticMesh::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bAddedBlend, true, 0xFFFFFFFF, _T("混合叠加"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fPlaySpeed, true, 0xFFFFFFFF, _T("播放速度"), NULL, NULL, "MinValue:0");
    DECLARE_PROPERTY(serializer, m_animationType, true, 0xFFFFFFFF, _T("动画播放方式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uSpecifiedFrame, true, 0xFFFFFFFF, _T("指定帧"), NULL, NULL, _T("VisibleWhen:m_animationType==eNAPT_NONE"));
    DECLARE_PROPERTY(serializer, m_path, true, 0xFFFFFFFF, _T("文件路径"), NULL, NULL, _T("DefaultValue:Wildcard@*.sce@InitialPath@../Resource/Scene@DialogTitle@选择静态模型文件"));
}
#ifdef EDITOR_MODE
bool CStaticMesh::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bHandled = super::OnPropertyChange(pVariableAddr, pSerializer);
    if(!bHandled)
    {
        if(pVariableAddr == &m_path)
        {
            if (m_pMeshData != NULL)
            {
                m_pMeshData = NULL;
            }
            DeserializeVariable(m_path, pSerializer, this);
            LoadMeshData();
            bHandled = true;
        }
        else if (pVariableAddr == &m_animationType)
        {
            DeserializeVariable(m_animationType, pSerializer, this);
            PlayModelAnimation(m_animationType);
            bHandled = true;
        }
        else if (pVariableAddr == &m_uSpecifiedFrame)
        {
            DeserializeVariable(m_uSpecifiedFrame, pSerializer, this);
            PlayModelAnimation(m_animationType);
            bHandled = true;
        }
        else if (pVariableAddr == &m_bAddedBlend)
        {
            DeserializeVariable(m_bAddedBlend, pSerializer, this);
            if (m_pMeshData != nullptr)
            {
                const std::vector<SSubMesh*>& subMeshes = m_pMeshData->GetSubMeshes();
                for (size_t i = 0; i < subMeshes.size(); ++i)
                {
                    if (subMeshes[i] != nullptr && subMeshes[i]->m_pRenderBatch->GetMaterial() == CRenderManager::GetInstance()->GetMeshMaterial(true, !m_bAddedBlend))
                    {
                        subMeshes[i]->m_pRenderBatch->SetMaterial(CRenderManager::GetInstance()->GetMeshMaterial(true, m_bAddedBlend));
                    }
                }
            }
            bHandled = true;
        }
        else if (pVariableAddr == &m_fPlaySpeed)
        {
            DeserializeVariable(m_fPlaySpeed, pSerializer, this);
            for (size_t i = 0; i < m_meshAnimationList.size(); ++i)
            {
                m_meshAnimationList[i]->SetFPS((uint32_t)(60 * m_fPlaySpeed));
            }
            bHandled = true;
        }
    }
    return bHandled;
}
#endif
bool CStaticMesh::Load()
{
    LoadMeshData();
    PlayModelAnimation(m_animationType);
    return super::Load();
}

void CStaticMesh::LoadMeshData()
{
    BEATS_ASSERT(m_pMeshData == NULL);
    if (!m_path.m_value.empty())
    {
        SharePtr<CStaticMeshData> pMeshData = CResourceManager::GetInstance()->GetResource<CStaticMeshData>(m_path.m_value);
        SetMeshData(pMeshData);
        if (m_pMeshData != nullptr)
        {
            const std::vector<SSubMesh*>& subMeshes = m_pMeshData->GetSubMeshes();
            for (size_t i = 0; i < subMeshes.size(); ++i)
            {
                if (subMeshes[i] != nullptr && subMeshes[i]->m_pRenderBatch->GetMaterial() == CRenderManager::GetInstance()->GetMeshMaterial(true, !m_bAddedBlend))
                {
                    subMeshes[i]->m_pRenderBatch->SetMaterial(CRenderManager::GetInstance()->GetMeshMaterial(true, m_bAddedBlend));
                }
            }
        }
        BEATS_ASSERT(pMeshData != NULL);
    }
}

void CStaticMesh::DoRender()
{
#ifdef DEVELOP_VERSION
    if (!CRenderManager::GetInstance()->m_bRenderMesh)
    {
        return;
    }
    if (CRenderManager::GetInstance()->m_bRenderAABB)
    {
        CShapeRenderer::GetInstance()->DrawAABB(GetAABB(), 0x00FF00FF);
    }
#endif
    if(m_pMeshData != NULL)
    {
        for(auto pSubMesh : m_pMeshData->GetSubMeshes())
        {
            pSubMesh->SetParentNode(this);
            CRenderGroup* pGroup = CRenderGroupManager::GetInstance()->GetRenderGroup();
            CColor colorScale = GetColorScale(true);
            CRenderBatch* pCurrBatch = pGroup->GetProxyBatch(pSubMesh->m_pRenderBatch);
            if ((uint32_t)colorScale != 0x64646464)
            {
                pCurrBatch->SetPreRenderAction([]()
                {
                    CRenderer::GetInstance()->EnableGL(CBoolRenderStateParam::eBSP_Blend);
                });
                // Update alpha 
                std::map<TString, CShaderUniform>& uniformMap = pCurrBatch->GetUniformMap();
                CShaderUniform& alphaUniform = uniformMap[COMMON_UNIFORM_NAMES[UNIFORM_COLOR_SCALE]];
                if (alphaUniform.GetName().empty())
                {
                    alphaUniform.SetName(COMMON_UNIFORM_NAMES[UNIFORM_COLOR_SCALE]);
                    alphaUniform.SetType(eSUT_4f);
                }
                std::vector<float>& alphaData = alphaUniform.GetData();
                BEATS_ASSERT(alphaData.size() == 4);
                alphaData[0] = colorScale.r * 0.01f;
                alphaData[1] = colorScale.g * 0.01f;
                alphaData[2] = colorScale.b * 0.01f;
                alphaData[3] = colorScale.a * 0.01f;
            }
            pCurrBatch->SetWorldTM(&pSubMesh->GetWorldTM());
            pSubMesh->SetParentNode(NULL);
#ifdef DEVELOP_VERSION
            if (pCurrBatch->m_usage == ERenderBatchUsage::eRBU_Count)
            {
                pCurrBatch->m_usage = ERenderBatchUsage::eRBU_StaticMesh;
            }
            BEATS_ASSERT(pCurrBatch->m_usage == ERenderBatchUsage::eRBU_StaticMesh);
#endif
        }
    }
}

void CStaticMesh::PlayModelAnimation(ENodeAnimationPlayType type)
{
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_pMeshData)
    if (m_meshAnimationList.size() == 0)
    {
        const std::vector<SSubMesh*>& subMeshes = m_pMeshData->GetSubMeshes();
        for (size_t i = 0; i < subMeshes.size(); ++i)
        {
            if (subMeshes[i]->m_pNodeAnimationData != NULL)
            {
                CNodeAnimation* pMeshNodeAnimation = CNodeAnimationManager::GetInstance()->RequestNodeAnimation();
                pMeshNodeAnimation->SetFPS(uint32_t(60 * m_fPlaySpeed));
                pMeshNodeAnimation->SetPlayType(type);
                pMeshNodeAnimation->SetData(subMeshes[i]->m_pNodeAnimationData);
                pMeshNodeAnimation->SetOwner(subMeshes[i]);
                m_meshAnimationList.push_back(pMeshNodeAnimation);
                if (type == eNAPT_NONE)
                {
                    pMeshNodeAnimation->SetCurrentFrame(m_uSpecifiedFrame % pMeshNodeAnimation->GetData()->GetFrameCount());
                }
                else
                {
                    pMeshNodeAnimation->Play();
                }
            }
        }
    }
    else
    {
        for (size_t i = 0; i < m_meshAnimationList.size(); ++i)
        {
            m_meshAnimationList[i]->SetPlayType(type);
            if (type == eNAPT_NONE)
            {
                m_meshAnimationList[i]->SetCurrentFrame(m_uSpecifiedFrame % m_meshAnimationList[i]->GetData()->GetFrameCount());
            }
            else
            {
                if (!m_meshAnimationList[i]->IsPlaying())
                {
                    m_meshAnimationList[i]->Play();
                }
            }
        }
    }
    ASSUME_VARIABLE_IN_EDITOR_END
}

void CStaticMesh::SetFilePath(const TString& strFilePath)
{
    m_path.m_value = strFilePath;
}

void CStaticMesh::SetMeshData(SharePtr<CStaticMeshData> pMeshData)
{
    m_pMeshData = pMeshData;
    SetFilePath(pMeshData->GetFilePath());
    m_meshAnimationList.clear();
    CNodeAnimationManager::GetInstance()->RemoveNode(this);
}

CAABBBox CStaticMesh::GetAABB() const
{
    CAABBBox ret;
    if (m_pMeshData != nullptr)
    {
        ret = m_pMeshData->GetAABB();
    }
    const CMat4& worldTM = GetWorldTM();
    ret.m_minPos *= worldTM;
    ret.m_maxPos *= worldTM;
    return ret;
}

SharePtr<CStaticMeshData> CStaticMesh::GetMeshData() const
{
    return m_pMeshData;
}
