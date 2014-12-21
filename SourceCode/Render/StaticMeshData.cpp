#include "stdafx.h"
#include "StaticMeshData.h"
#include "RenderBatch.h"
#include "Material.h"
#include "Texture.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Resource/ResourceManager.h"
#include "NodeAnimation/NodeAnimationData.h"
#include "RenderManager.h"

SSubMesh::SSubMesh()
{

}

SSubMesh::~SSubMesh()
{
    BEATS_SAFE_DELETE(m_pRenderBatch);
    BEATS_SAFE_DELETE(m_pNodeAnimationData);
}

CStaticMeshData::CStaticMeshData()
{
    m_aabb.m_maxPos.Fill(FLT_MIN, FLT_MIN, FLT_MIN);
    m_aabb.m_minPos.Fill(FLT_MAX, FLT_MAX, FLT_MAX);
}

CStaticMeshData::~CStaticMeshData()
{
    if (IsInitialized())
    {
        Uninitialize();
    }
}

bool CStaticMeshData::Load()
{
    BEATS_ASSERT(!IsLoaded());

#ifdef DEVELOP_VERSION
    m_uVertexCount = 0;
#endif

    // Load From File
    CSerializer serializer;
    if (CFilePathTool::GetInstance()->LoadFile(&serializer, GetFilePath().c_str(), _T("rb")))
    {
        BEATS_ASSERT(serializer.GetWritePos() > 0, _T("Read empty file %s !"), GetFilePath().c_str());
        uint32_t meshCount = 0;
        serializer >> meshCount;
        uint32_t nAnimationFrameCount = 0;
        serializer >> nAnimationFrameCount;
        for (uint32_t i = 0; i < meshCount; ++i)
        {
            SSubMesh *pSubMesh = new SSubMesh;
            uint32_t matCount;
            serializer >> matCount;
            BEATS_ASSERT(matCount <= 1, _T("mesh has %d mat, but we only support 1 static mesh\n%s"), matCount, GetFilePath().c_str());
            bool bUseBlend = false;
            bool bUseLightMap = false;
            std::string strTextureName;
            std::string strLightMapTextureName;
            if (matCount > 0)
            {
                for (uint32_t j = 0; j < matCount; ++j)
                {
                    serializer >> strTextureName;
                    BEATS_ASSERT(!strTextureName.empty(), _T("Texutre can't be empty in static mesh data!"));
                    serializer >> bUseLightMap;
                    if (bUseLightMap)
                    {
                        serializer >> strLightMapTextureName;
                        BEATS_ASSERT(!strLightMapTextureName.empty());
                    }
                    serializer >> bUseBlend;
                }
            }
            uint32_t uVertexCount = 0;
            serializer >> uVertexCount;
            BEATS_ASSERT(uVertexCount > 0, "static mesh data %s contains no vertices!", GetFilePath().c_str());
#ifdef DEVELOP_VERSION
            m_uVertexCount += uVertexCount;
#endif
            CVec3 localPos;
            serializer >> localPos.X() >> localPos.Y() >> localPos.Z();
            pSubMesh->SetPosition(localPos);
            if (uVertexCount > 0)
            {
                uint32_t uDataSize = uVertexCount * (bUseLightMap ? sizeof(CVertexPTT) : sizeof(CVertexPT));
                SharePtr<CMaterial> pNewMaterial = bUseLightMap ? CRenderManager::GetInstance()->GetMeshLightMapMaterial(bUseBlend) : CRenderManager::GetInstance()->GetMeshMaterial(bUseBlend, false);
                pSubMesh->m_pRenderBatch = new CRenderBatch(bUseLightMap ? VERTEX_FORMAT(CVertexPTT) : VERTEX_FORMAT(CVertexPT), pNewMaterial, GL_TRIANGLES, false);
                pSubMesh->m_pRenderBatch->SetStatic(true);
                pSubMesh->m_pRenderBatch->AddVertices(serializer.GetReadPtr(), uVertexCount);
                serializer.SetReadPos(serializer.GetReadPos() + uDataSize);
                if (!strTextureName.empty())
                {
                    pSubMesh->m_pRenderBatch->SetTexture(0, CResourceManager::GetInstance()->GetResource<CTexture>(strTextureName.c_str()));
                }
                if (!strLightMapTextureName.empty())
                {
                    pSubMesh->m_pRenderBatch->SetTexture(1, CResourceManager::GetInstance()->GetResource<CTexture>(strLightMapTextureName.c_str()));
                }
                bool bUseRepeatMode = false;
                serializer >> bUseRepeatMode;
                serializer >> pSubMesh->m_aabb.m_minPos;
                serializer >> pSubMesh->m_aabb.m_maxPos;
                // If on point UV value is more than 1, we think we will use repeat mode to sample the texture.
                pSubMesh->m_pRenderBatch->SetTextureClampOrRepeat(!bUseRepeatMode);
                m_subMeshes.push_back(pSubMesh);
            }
            // Export animation.
            bool bHasPosAnimation = false;
            serializer >> bHasPosAnimation;
            if (bHasPosAnimation)
            {
                BEATS_ASSERT(pSubMesh->m_pNodeAnimationData == NULL);
                pSubMesh->m_pNodeAnimationData = new CNodeAnimationData;
                pSubMesh->m_pNodeAnimationData->SetFrameCount(nAnimationFrameCount);
                CNodeAnimationElement* pPosElement = new CNodeAnimationElement;
                pPosElement->SetType(eNAET_Pos);
                pSubMesh->m_pNodeAnimationData->AddElements(pPosElement);
                int nStartFrame = 0;
                int nEndFrame = 0;
                serializer >> nStartFrame >> nEndFrame;
                for (int n = nStartFrame; n <= nEndFrame; ++n)
                {
                    CVec3 data;
                    serializer >> data.X() >> data.Y() >> data.Z();
                    pPosElement->AddKeyFrame(n, data);
                }
            }
            bool bHasRotateAnimation = false;
            serializer >> bHasRotateAnimation;
            if (bHasRotateAnimation)
            {
                if (pSubMesh->m_pNodeAnimationData == NULL)
                {
                    pSubMesh->m_pNodeAnimationData = new CNodeAnimationData;
                    pSubMesh->m_pNodeAnimationData->SetFrameCount(nAnimationFrameCount);
                }
                CNodeAnimationElement* pRotateElement = new CNodeAnimationElement;
                pRotateElement->SetType(eNAET_Rotation);
                pSubMesh->m_pNodeAnimationData->AddElements(pRotateElement);
                int nStartFrame = 0;
                int nEndFrame = 0;
                serializer >> nStartFrame >> nEndFrame;
                for (int n = nStartFrame; n <= nEndFrame; ++n)
                {
                    CVec3 data;
                    serializer >> data.X() >> data.Y() >> data.Z();
                    pRotateElement->AddKeyFrame(n, data);
                }
            }
            bool bHasScaleAnimation = false;
            serializer >> bHasScaleAnimation;
            if (bHasScaleAnimation)
            {
                if (pSubMesh->m_pNodeAnimationData == NULL)
                {
                    pSubMesh->m_pNodeAnimationData = new CNodeAnimationData;
                    pSubMesh->m_pNodeAnimationData->SetFrameCount(nAnimationFrameCount);
                }
                CNodeAnimationElement* pScaleElement = new CNodeAnimationElement;
                pScaleElement->SetType(eNAET_Scale);
                pSubMesh->m_pNodeAnimationData->AddElements(pScaleElement);
                int nStartFrame = 0;
                int nEndFrame = 0;
                serializer >> nStartFrame >> nEndFrame;
                for (int n = nStartFrame; n <= nEndFrame; ++n)
                {
                    CVec3 data;
                    serializer >> data.X() >> data.Y() >> data.Z();
                    data.Z() *= -1;// HACK: I don't know why we need to do this to fit 3dmax.
                    pScaleElement->AddKeyFrame(n, data);
                }
            }
            if (uVertexCount == 0)
            {
                BEATS_SAFE_DELETE(pSubMesh);
            }
        }
        serializer >> m_aabb.m_minPos;
        serializer >> m_aabb.m_maxPos;
    }
    super::Load();
    return true;
}

void CStaticMeshData::Initialize()
{
    super::Initialize();
    for (size_t i = 0; i < m_subMeshes.size(); ++i)
    {
        m_subMeshes[i]->Initialize();
    }
}

void CStaticMeshData::Uninitialize()
{
    for (auto pSubMesh : m_subMeshes)
    {
        pSubMesh->Uninitialize();
        BEATS_SAFE_DELETE(pSubMesh);
    }
    m_subMeshes.clear();
    super::Uninitialize();
}

const std::vector<SSubMesh *> & CStaticMeshData::GetSubMeshes() const
{
    return m_subMeshes;
}

#ifdef EDITOR_MODE
void CStaticMeshData::Reload()
{
    super::Reload();
    Uninitialize();
    Initialize();
}
#endif

#ifdef DEVELOP_VERSION
TString CStaticMeshData::GetDescription() const
{
    TString strRet = super::GetDescription();
    TCHAR szBuffer[MAX_PATH];
    _stprintf(szBuffer, "VertexCount: %d, ", m_uVertexCount);
    strRet.append(szBuffer);
    return strRet;
}
#endif

const CAABBBox& CStaticMeshData::GetAABB() const
{
    return m_aabb;
}
