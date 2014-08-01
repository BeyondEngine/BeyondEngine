#include "stdafx.h"
#include "Skin.h"
#include "RenderManager.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Renderer.h"
#include "Material.h"
#include "Resource/ResourceManager.h"
#include "RenderBatch.h"
#include "RenderGroup.h"

CSkin::CSkin()
{
}

CSkin::~CSkin()
{
}

bool CSkin::Load()
{
    bool bRet = false;
    BEATS_ASSERT(!IsLoaded(), _T("Can't Load a skin which is already loaded!"));
    if (m_pSkinMaterial == NULL)
    {
        m_pSkinMaterial = new CMaterial();
        m_pSkinMaterial->SetSharders( _T("SkinShader.vs"), _T("SkinShader.ps"));
        m_pSkinMaterial->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_DepthTest, true);
        m_pSkinMaterial->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_Blend, false);
        m_pSkinMaterial->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_ScissorTest, false);
        m_pSkinMaterial->Initialize();
    }
    // Load From File
    const TString& strFilePath = GetFilePath();
    if (!strFilePath.empty())
    {
        CSerializer serializer(strFilePath.c_str());
        size_t uMeshcount = 0;
        serializer >> uMeshcount;
        // HACK: TODO: temp fix
        uMeshcount = 1;
        for (size_t i = 0; i < uMeshcount; ++i)
        {
            std::vector<CVertexPTB> vertices;
            std::vector<unsigned short> indices;
            size_t uMaterialCount = 0;
            serializer >> uMaterialCount;
            BEATS_ASSERT(uMaterialCount == 1, _T("Only support one material!"));
            for (size_t j = 0; j < uMaterialCount; ++j)
            {
                TString strTextureName;
                serializer >> strTextureName;
                BEATS_ASSERT(!strTextureName.empty(), _T("Texutre can't be empty in skin!"));
                SharePtr<CTexture> pTexture = CResourceManager::GetInstance()->GetResource<CTexture>(strTextureName);
                BEATS_ASSERT(pTexture != NULL);
                m_pSkinMaterial->SetTexture(0, pTexture);
            }

            CSerializer tmpVerticesBufferPos, tmpVerticesBufferUV;
            size_t uVertexCount = 0;
            serializer >> uVertexCount;
            for (size_t k = 0; k < uVertexCount; ++k)
            {
                CVertexPTB vertexPTB;
                serializer >> vertexPTB.position.x >> vertexPTB.position.y >> vertexPTB.position.z >> vertexPTB.tex.u >> vertexPTB.tex.v;
                serializer >> vertexPTB.bones.x;
                serializer >> vertexPTB.weights.x;
                serializer >> vertexPTB.bones.y;
                serializer >> vertexPTB.weights.y;
                serializer >> vertexPTB.bones.z;
                serializer >> vertexPTB.weights.z;
                serializer >> vertexPTB.bones.w;
                serializer >> vertexPTB.weights.w;

                vertices.push_back(vertexPTB);
#ifdef _DEBUG
                float sum = vertexPTB.weights.x + vertexPTB.weights.y + vertexPTB.weights.z + vertexPTB.weights.w;
                BEATS_ASSERT(sum < 1.01F, _T("Weight can't be greater than 1.01F, cur Value : %f!"), sum);
                BEATS_WARNING(sum > 0.99F, _T("Weight can't be smaller than 0.99F, cur Value : %f!"), sum);
#endif
            }
            size_t uIndexCount = 0;
            serializer >> uIndexCount;
            for (size_t l = 0; l < uIndexCount; ++l)
            {
                int nIndex = 0;
                serializer >> nIndex;
                BEATS_ASSERT(nIndex < (int)vertices.size() && nIndex < 0xFFFF, _T("Invalid index %d"), nIndex);
                indices.push_back((unsigned short)nIndex);
            }
            CRenderGroup *p3DRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_3D);
            CRenderBatch* pRenderBatch = p3DRenderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPTB), m_pSkinMaterial, GL_TRIANGLES, true);
            pRenderBatch->SetStatic(true);
            pRenderBatch->AddIndexedVertices(vertices.data(), vertices.size(), indices.data(), indices.size());
            pRenderBatch->SetGroup(p3DRenderGroup);
            bool bAlreadyExists = false;
            for (size_t w = 0; w < m_renderBatches.size(); ++w)
            {
                if (m_renderBatches[w] == pRenderBatch)
                {
                    bAlreadyExists = true;
                    break;
                }
            }
            if (!bAlreadyExists)
            {
                m_renderBatches.push_back(pRenderBatch);
            }
        }
        bRet = true;
    }

    super::Load();
    return bRet;
}

bool CSkin::Unload()
{
    super::Unload();
    for (size_t i = 0; i < m_renderBatches.size(); ++i)
    {
        m_renderBatches[i]->Clear();
        BEATS_SAFE_DELETE(m_renderBatches[i]);
    }
    m_renderBatches.clear();
    return true;
}

const std::vector<CRenderBatch*>& CSkin::GetRenderBatches() const
{
    return m_renderBatches;
}

#ifdef SW_SKEL_ANIM
void CSkin::CalcSkelAnim(const CAnimationController::BoneMatrixMap &matrices)
{
    size_t uVertexCount = m_vertices.size();
    CVertexPT *vertices = new CVertexPT[uVertexCount];
    for(size_t i = 0; i < uVertexCount; ++i)
    {
        const CVertexPTB &vertex = m_vertices[i];
        CVertexPT &vertex1 = vertices[i];
        if(matrices.empty())
        {
            vertex1.position = vertex.position;
            vertex1.tex = vertex.tex;
            continue;
        }

        kmVec3  pos;
        kmVec3Fill(&pos,vertex.position.x,vertex.position.y, vertex.position.z);

        kmVec3 finalpos;
        kmMat4 mat, mat2, mat3, mat4;
        if(vertex.bones.x >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.x));
            BEATS_ASSERT(itr != matrices.end());
            mat = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat);
            kmVec3Scale(&postmp, &postmp, vertex.weights.x);
            finalpos = postmp;
        }
        if(vertex.bones.y >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.y));
            BEATS_ASSERT(itr != matrices.end());
            mat2 = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat2);
            kmVec3Scale(&postmp, &postmp, vertex.weights.y);
            kmVec3Add(&finalpos,&finalpos,&postmp);
        }
        if(vertex.bones.z >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.z));
            BEATS_ASSERT(itr != matrices.end());
            mat3 = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat3);
            kmVec3Scale(&postmp, &postmp, vertex.weights.z);
            kmVec3Add(&finalpos,&finalpos,&postmp);
        }
        if(vertex.bones.w >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.w));
            BEATS_ASSERT(itr != matrices.end());
            mat4 = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat4);
            kmVec3Scale(&postmp, &postmp, vertex.weights.w);
            kmVec3Add(&finalpos,&finalpos,&postmp);
        }
        vertex1.position = finalpos;

        vertex1.tex = vertex.tex;
    }

    buildVBOVertex(vertices, uVertexCount * sizeof(CVertexPT));
    BEATS_SAFE_DELETE_ARRAY(vertices);
}
#endif