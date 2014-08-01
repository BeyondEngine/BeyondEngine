#include "stdafx.h"
#include "StaticMeshData.h"
#include "RenderBatch.h"
#include "Material.h"
#include "RenderState.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Resource/ResourceManager.h"

CStaticMeshData::CStaticMeshData()
{

}

CStaticMeshData::~CStaticMeshData()
{

}

bool CStaticMeshData::Load()
{
    BEATS_ASSERT(!IsLoaded());

    CRenderGroup *p3DRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_3D);
    CRenderGroup *pAlphaRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_3D_ALPHA);

    // Load From File
    CSerializer serializer;
    CFilePathTool::GetInstance()->LoadFile(&serializer, GetFilePath().c_str(), _T("rb"));

    BEATS_ASSERT(serializer.GetWritePos() > 0, _T("Read empty file %s !"), GetFilePath().c_str());
    size_t meshCount;
    serializer >> meshCount;
    std::map<float, std::vector<SSubMesh*>> meshSortMap;
    for (size_t i = 0; i < meshCount; ++i)
    {
        size_t matCount;
        serializer >> matCount;
        for (size_t j = 0; j < matCount; ++j)
        {
            SSubMesh *pSubMesh = new SSubMesh;
            CMaterial* pNewMaterial = new CMaterial();
            std::string strTextureName;
            serializer >> strTextureName;
            BEATS_ASSERT(!strTextureName.empty(), _T("Texutre can't be empty in static mesh data!"));
            TCHAR buffer[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(strTextureName.c_str(), buffer, MAX_PATH);
            pNewMaterial->SetTexture(0, CResourceManager::GetInstance()->GetResource<CTexture>(buffer));
            pNewMaterial->SetSharders( _T("PointTexShader.vs"), _T("PointTexShader.ps"));
            CRenderState* pRenderState = pNewMaterial->GetRenderState();
            pRenderState->SetBoolState(CBoolRenderStateParam::eBSP_DepthTest, true);
            bool bUseBlend = false;
            serializer >> bUseBlend;
            pRenderState->SetBoolState(CBoolRenderStateParam::eBSP_Blend, bUseBlend);
            pRenderState->SetBoolState(CBoolRenderStateParam::eBSP_ScissorTest, false);
            pRenderState->SetBlendEquation(GL_FUNC_ADD);
            pRenderState->SetBlendFuncSrcFactor(GL_SRC_ALPHA);
            pRenderState->SetBlendFuncTargetFactor(GL_ONE_MINUS_SRC_ALPHA);
            pNewMaterial->Initialize();
            pSubMesh->m_material = pNewMaterial;
            size_t uVertexCount = 0;
            serializer >> uVertexCount;
            if (uVertexCount > 0)
            {
                pSubMesh->m_vertices.resize(uVertexCount);
                for (size_t k = 0; k < uVertexCount; ++k)
                {
                    kmVec3 &position = pSubMesh->m_vertices[k].position;
                    serializer >> position.x >> position.y >> position.z;
                    CTex &tex = pSubMesh->m_vertices[k].tex;
                    serializer >> tex.u >> tex.v;
                }

                pSubMesh->m_pRenderBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPT), pSubMesh->m_material, GL_TRIANGLES, false);
                pSubMesh->m_pRenderBatch->SetStatic(true);
                pSubMesh->m_pRenderBatch->AddVertices(pSubMesh->m_vertices.data(), pSubMesh->m_vertices.size());
                pSubMesh->m_pRenderBatch->SetGroup(bUseBlend ? pAlphaRenderGroup : p3DRenderGroup);
                // HACK: For transparent object, we decide which object should be render first(further in view matrix)
                // We just simply detect the distance between center pos and the camera direction pos 5000, 5000.
                kmVec3 meshCenterPos;
                serializer >> meshCenterPos.x >> meshCenterPos.y >> meshCenterPos.z;
                if (bUseBlend)
                {
                    meshCenterPos.y = 0;
                    kmVec3 positivePos;
                    kmVec3Fill(&positivePos, 5000, 0, 5000);
                    kmVec3 distance;
                    kmVec3Subtract(&distance, &meshCenterPos, &positivePos);
                    meshSortMap[kmVec3Length(&distance)].push_back(pSubMesh);
                }
                else
                {
                    // Solid object always render first.
                    m_subMeshes.push_back(pSubMesh);
                }
            }
        }
    }
    for (auto iter = meshSortMap.rbegin(); iter != meshSortMap.rend(); ++iter)
    {
        for (size_t i = 0; i < iter->second.size(); ++i)
        {
            m_subMeshes.push_back(iter->second.at(i));
        }
    }

    super::Load();
    return true;
}

bool CStaticMeshData::Unload()
{
    BEATS_ASSERT(IsLoaded());
    for(auto pSubMesh : m_subMeshes)
    {
        BEATS_SAFE_DELETE(pSubMesh->m_pRenderBatch);
        BEATS_SAFE_DELETE(pSubMesh);
    }
    super::Unload();
    return true;
}

const std::vector<SSubMesh *> & CStaticMeshData::GetSubMeshes() const
{
    return m_subMeshes;
}
