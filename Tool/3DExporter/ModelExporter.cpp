#include "stdafx.h"
#include "ModelExporter.h"
#include "FilePathTool.h"

IGameTextureMap* GetTextureMap(IGameMaterial* pMat, int nSlot)
{
    IGameTextureMap* pTex = NULL;

    int nTexCnt = pMat->GetNumberOfTextureMaps();

    for(int x = 0; x < nTexCnt; x++)
    {
        pTex = pMat->GetIGameTextureMap(x);

        if(pTex->GetStdMapSlot() == nSlot)
            return pTex;
    }

    return NULL;
}

inline static void GetMaterialCnt(IGameMaterial* pRootMat, uint32_t& uMatCnt)
{
    if(pRootMat == NULL)
    {
        uMatCnt = 0;
        return;
    }

    if(!pRootMat->IsMultiType())
    {
        uMatCnt++;
    }
    else
    {
        int nSubMatCnt = pRootMat->GetSubMaterialCount();
        for(int x = 0; x<nSubMatCnt; x++)
        {
            IGameMaterial* pSubMat = pRootMat->GetSubMaterial(x);
            BEATS_ASSERT(pSubMat != nullptr);
            GetMaterialCnt(pSubMat, uMatCnt);
        }
    }
}

inline static IGameMaterial* GetMaterial(uint32_t& index, IGameMaterial* pRootMat, IGameMaterial* pParent, int nChildID, IGameMaterial** pParentRet, int* pChildIDRet)
{
    BEATS_ASSERT(pRootMat != NULL);
    BEATS_ASSERT(pParentRet != NULL);

    if(index == 0 && !pRootMat->IsMultiType())
    {
        *pParentRet = pParent;
        *pChildIDRet = nChildID;
        return pRootMat;
    }
    else if(index == 0 && pRootMat->IsMultiType())
    {
        int nSubMatCnt = pRootMat->GetSubMaterialCount();

        for(int x = 0; x < nSubMatCnt; x++)
        {
            IGameMaterial* pSubMat = pRootMat->GetSubMaterial(x);
            pSubMat = GetMaterial(index, pSubMat, pRootMat, x, pParentRet, pChildIDRet);
            if(pSubMat != NULL)
                return pSubMat;
        }
    }
    else if(index != 0 && !pRootMat->IsMultiType())
    {
        index--;

        int nSubMatCnt = pRootMat->GetSubMaterialCount();
        for(int x = 0; x<nSubMatCnt; x++)
        {
            IGameMaterial* pSubMat = pRootMat->GetSubMaterial(x);
            pSubMat = GetMaterial(index, pSubMat, pRootMat, x, pParentRet, pChildIDRet);
            if(pSubMat != NULL)
                return pSubMat;
        }
    }
    else
    {
        int nSubMatCnt = pRootMat->GetSubMaterialCount();
        for(int x = 0; x < nSubMatCnt; x++)
        {
            IGameMaterial* pSubMat = pRootMat->GetSubMaterial(x);
            pSubMat = GetMaterial(index, pSubMat, pRootMat, x, pParentRet, pChildIDRet);
            if(pSubMat != NULL)
                return pSubMat;
        }
    }

    return NULL;
}


CModelExporter::CModelExporter()
{
    m_pIGameScene = GetIGameInterface();
    m_pIGameScene ->InitialiseIGame();
}

CModelExporter::~CModelExporter()
{
    m_pIGameScene->ReleaseIGame();
}

void CModelExporter::CollectMeshFaces( Tab<FaceEx*>& faceList, IGameMesh* pMesh, uint32_t uMatID, BOOL bMultiMat )
{
    if(!bMultiMat)
    {
        for(int x = 0; x < pMesh->GetNumberOfFaces(); x++)
        {
            FaceEx* pTemp = pMesh->GetFace(x);
            faceList.Append(1, &pTemp);
        }
    }
    else
    {
        faceList = pMesh->GetFacesFromMatID(uMatID);
    }
}

bool CModelExporter::ExportMeshMaterial( IGameNode* pNode, IGameMesh* /*pMesh*/, IGameMaterial* pMat, uint32_t /*uMatID*/, BOOL /*bMultiMat*/ )
{
    BEATS_ASSERT(pMat->GetMaterialName());
    IGameTextureMap* pTex = GetTextureMap(pMat, ID_DI);
    if (pTex != NULL)
    {
        const TCHAR* pszTexFileName = pTex->GetBitmapFileName();
        BEATS_ASSERT(pszTexFileName != NULL);
        std::string textureName = CFilePathTool::GetInstance()->FileName(pszTexFileName);
        if (textureName.empty())
        {
            MessageBox(NULL, _T("Export empty texture name!"), _T("Error"), MB_OK);
        }
        m_serializer << textureName;
    }
    IGameTextureMap* pLightMapTex = GetTextureMap(pMat, ID_AM);
    bool bUseLightMap = pLightMapTex != NULL;
    m_serializer << bUseLightMap;
    if (pLightMapTex != NULL)
    {
        const TCHAR* pszTexFileName = pLightMapTex->GetBitmapFileName();
        BEATS_ASSERT(pszTexFileName != NULL);
        std::string textureName = CFilePathTool::GetInstance()->FileName(pszTexFileName);
        if (textureName.empty())
        {
            MessageBox(NULL, _T("Export empty texture name!"), _T("Error"), MB_OK);
        }
        m_serializer << textureName;
    }

    // Just determine if we have enable the opacity channel.
    bool bSetOpacityTexture = GetTextureMap(pMat, ID_OP) != NULL;
    if (bSetOpacityTexture)
    {
        IGameTextureMap* pTex = GetTextureMap(pMat, ID_OP);
        const TCHAR* pszTexFileName = pTex->GetBitmapFileName();
        BEATS_ASSERT(pszTexFileName != NULL);
        std::string textureName = CFilePathTool::GetInstance()->FileName(pszTexFileName);
        bSetOpacityTexture = textureName.length() > 0;
    }
    m_serializer << bSetOpacityTexture;
    return true;
}

void CModelExporter::ExportMeshVertex(IGameNode* pNode, IGameMesh* pMesh, IGameMaterial* pMat, uint32_t uMatID, BOOL bMultiMat)
{
    pMesh->SetUseWeightedNormals();
    pMesh->InitializeData();

    Tab<FaceEx*> faceTab;

    CollectMeshFaces(faceTab, pMesh, uMatID, bMultiMat);
    uint32_t uFaceCount = faceTab.Count();
    uint32_t uVertexCount = uFaceCount * 3;
    m_serializer << uVertexCount;

    bool bDiffusemap = pMat != nullptr && GetTextureMap(pMat, ID_DI) != nullptr;
    bool bLightmap = pMat != nullptr && GetTextureMap(pMat, ID_AM) != nullptr;
    bool bUseRepeatMode = false;
    GMatrix localTm = pNode->GetLocalTM();
    Point3 localPos = localTm.Translation();
    m_serializer << localPos.x << localPos.y << localPos.z;
    if (uVertexCount > 0)
    {
        GMatrix worldTm = pNode->GetWorldTM();
        for (uint32_t i = 0; i < uFaceCount; ++i)
        {
            FaceEx* pFace = faceTab[i];
            for (int j = 0; j < 3; ++j)
            {
                int indexPos = pFace->vert[j];
                Point3 pos = pMesh->GetVertex(indexPos, false);
                // HACK: we want to the the relative pos of the vertex
                // If we set the second param ObjectSpace to true, it returns a position which is from the center of all mesh faces.
                // But what we want is a position from the relative origin pos.
                // So we get it by calculate pos from vertex world pos and the node's world pos.
                //Point3 pos2 = pMesh->GetVertex(indexPos, true); // Don't use this.
                Point3 parentNodeWorldPos = worldTm.Translation();
                pos.x -= parentNodeWorldPos.x;
                pos.y -= parentNodeWorldPos.y;
                pos.z -= parentNodeWorldPos.z;
                m_serializer << pos.x << pos.y << pos.z;
                Point3 ptUV;
                int indexUV = pFace->texCoord[j];
                int nChannel = 0;
                DWORD mapIndex[3];
                if (bDiffusemap)
                {
                    IGameTextureMap* pMap = GetTextureMap(pMat, ID_DI);
                    nChannel = pMap->GetMapChannel();
                    if (pMesh->GetMapFaceIndex(nChannel, pFace->meshFaceIndex, mapIndex))
                        ptUV = pMesh->GetMapVertex(nChannel, mapIndex[j]);
                    else
                        ptUV = pMesh->GetMapVertex(nChannel, indexUV);
                    m_serializer << ptUV.x << ptUV.y;
                    if (ptUV.x > 1 || ptUV.y > 1)
                    {
                        bUseRepeatMode = true;
                    }
                }
                else
                {
                    m_serializer << 0.f << 0.f;
                }
                if (bLightmap)
                {
                    IGameTextureMap* pMap = GetTextureMap(pMat, ID_AM);
                    nChannel = pMap->GetMapChannel();
                    if (pMesh->GetMapFaceIndex(nChannel, pFace->meshFaceIndex, mapIndex))
                        ptUV = pMesh->GetMapVertex(nChannel, mapIndex[j]);
                    else
                        ptUV = pMesh->GetMapVertex(nChannel, indexUV);
                    m_serializer << ptUV.x << ptUV.y;
                    if (ptUV.x > 1 || ptUV.y > 1)
                    {
                        bUseRepeatMode = true;
                    }
                }
            }
        }
        m_serializer << bUseRepeatMode;
        Box3 boundingBox;
        pMesh->GetBoundingBox(boundingBox);
        GMatrix mtxWorldTM = pNode->GetWorldTM();
        boundingBox.pmin.x -= mtxWorldTM[3][0];
        boundingBox.pmin.y -= mtxWorldTM[3][1];
        boundingBox.pmin.z -= mtxWorldTM[3][2];
        boundingBox.pmax.x -= mtxWorldTM[3][0];
        boundingBox.pmax.y -= mtxWorldTM[3][1];
        boundingBox.pmax.z -= mtxWorldTM[3][2];
        RefreshRootBoundingBox(boundingBox);
        m_serializer << boundingBox.pmin.x << boundingBox.pmin.y << boundingBox.pmin.z;
        m_serializer << boundingBox.pmax.x << boundingBox.pmax.y << boundingBox.pmax.z;
    }
}

void CModelExporter::ExportMesh( IGameNode* pNode )
{
    IGameMesh* pMesh = (IGameMesh*)pNode->GetIGameObject();
    pMesh->SetUseWeightedNormals();
    pMesh->InitializeData();
    pMesh->InitializeBinormalData();

    IGameMaterial* pRootMat = pNode->GetNodeMaterial();
    IGameMaterial* pMat = nullptr;
    int nMatID = 0;
    if(pRootMat != NULL)
    {
        uint32_t uMatCnt = 0;
        GetMaterialCnt(pRootMat, uMatCnt);
        BEATS_ASSERT(uMatCnt < 2, "Material count in %s should always less than 2, current count: %d", pRootMat->GetMaterialName(), uMatCnt);
        m_serializer << uMatCnt;
        for(uint32_t x = 0; x < uMatCnt; x++)
        {
            uint32_t index = x;
            int nChildID;
            IGameMaterial* pParentMat;
            pMat = GetMaterial(index, pRootMat, NULL, -1, &pParentMat, &nChildID);

            if(pParentMat != NULL)
            {
                nMatID = pParentMat->GetMaterialID(nChildID);
                BEATS_ASSERT(pParentMat->GetSubMaterial(nChildID) == pMat);
            }

            if(pMat != NULL)
            {
                ExportMeshMaterial(pNode, pMesh, pMat, nMatID, pRootMat->IsMultiType());
            }
        }
    }
    else
    {
        m_serializer << 0;
    }

    ExportMeshVertex(pNode, pMesh, pMat, nMatID, pRootMat != nullptr && pRootMat->IsMultiType());
}

void CModelExporter::ExportMeshAnimation(IGameNode* pNode, IGameControlType animationType)
{
    auto pControl = pNode->GetIGameControl();
    IGameKeyTab quickList;
    pControl->GetQuickSampledKeys(quickList, animationType);
    // if the key frame is 1, it won't make sense.
    BEATS_ASSERT(quickList.Count() == 0 || quickList.Count() >= 2);
    bool bHasAnimation = quickList.Count() >= 2;
    m_serializer << bHasAnimation;
    if (bHasAnimation)
    {
        int nTickPerFrame = m_pIGameScene->GetSceneTicks();
        int nStartFrame = quickList[0].t / nTickPerFrame;
        int nEndFrame = quickList[quickList.Count() - 1].t / nTickPerFrame;
        m_serializer << nStartFrame << nEndFrame;
        BEATS_ASSERT(nStartFrame >= 0 && nEndFrame >= 0);
        IGameKeyTab dataList;
        pControl->GetFullSampledKeys(dataList, 1, animationType);
        for (int n = nStartFrame; n <= nEndFrame; ++n)
        {
            switch (animationType)
            {
            case IGAME_POS:
                m_serializer << dataList[n].sampleKey.pval.x << dataList[n].sampleKey.pval.y << dataList[n].sampleKey.pval.z;
                break;
            case IGAME_SCALE:
                m_serializer << dataList[n].sampleKey.sval.s.x << dataList[n].sampleKey.sval.s.y << dataList[n].sampleKey.sval.s.z;
                break;
            case IGAME_ROT:
            {
                float x, y, z;
                dataList[n].sampleKey.qval.GetEuler(&x, &y, &z);
                static const float PIUnder180 = 57.295779f; // 180 / PI
                x *= PIUnder180;
                y *= PIUnder180;
                z *= PIUnder180;
                m_serializer << x << y << z;
            }
            break;
            default:
                BEATS_ASSERT(false, "never reach here!");
                break;
            }
        }
    }
}

void CModelExporter::Export(const char* pszFileName)
{
    m_rootBoundingBox.pmin.Set(FLT_MAX, FLT_MAX, FLT_MAX);
    m_rootBoundingBox.pmax.Set(FLT_MIN, FLT_MIN, FLT_MIN);
    m_serializer.Reset();
    uint32_t uTotalRootNodeCnt = m_pIGameScene->GetTopLevelNodeCount();
    std::vector<IGameNode*> meshNodeVector;
    for(uint32_t x = 0; x < uTotalRootNodeCnt; x++)
    {
        IGameNode* pNode = m_pIGameScene->GetTopLevelNode(x);
        IGameObject* pObject = pNode->GetIGameObject();

        IGameObject::ObjectTypes gameType = pObject->GetIGameType();

        if(gameType == IGameObject::IGAME_MESH)
        {
            meshNodeVector.push_back(pNode);
        }
    }

    int nMeshNodeCount = meshNodeVector.size();
    m_serializer << nMeshNodeCount;
    TimeValue end_time = m_pIGameScene->GetSceneEndTime();
    int nEndFrame = end_time / m_pIGameScene->GetSceneTicks();
    nEndFrame += 1;// we also need to export the 0 frame, so the count should be add 1.
    m_serializer << nEndFrame;

    for(auto pNode : meshNodeVector)
    {
        ExportMesh(pNode);
        ExportMeshAnimation(pNode, IGAME_POS);
        ExportMeshAnimation(pNode, IGAME_ROT);
        ExportMeshAnimation(pNode, IGAME_SCALE);
    }
    m_serializer << m_rootBoundingBox.pmin.x << m_rootBoundingBox.pmin.y << m_rootBoundingBox.pmin.z;
    m_serializer << m_rootBoundingBox.pmax.x << m_rootBoundingBox.pmax.y << m_rootBoundingBox.pmax.z;
    if (m_serializer.GetWritePos() > 0)
    {
        m_serializer.Deserialize(pszFileName);
    }
}

void CModelExporter::RefreshRootBoundingBox(const Box3& newBox)
{
    if (m_rootBoundingBox.pmin.x > newBox.pmin.x)
        m_rootBoundingBox.pmin.x = newBox.pmin.x;

    if (m_rootBoundingBox.pmin.y > newBox.pmin.y)
        m_rootBoundingBox.pmin.y = newBox.pmin.y;

    if (m_rootBoundingBox.pmin.z > newBox.pmin.z)
        m_rootBoundingBox.pmin.z = newBox.pmin.z;

    if (m_rootBoundingBox.pmax.x < newBox.pmax.x)
        m_rootBoundingBox.pmax.x = newBox.pmax.x;

    if (m_rootBoundingBox.pmax.y < newBox.pmax.y)
        m_rootBoundingBox.pmax.y = newBox.pmax.y;

    if (m_rootBoundingBox.pmax.z < newBox.pmax.z)
        m_rootBoundingBox.pmax.z = newBox.pmax.z;
}