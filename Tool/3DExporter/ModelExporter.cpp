#include "stdafx.h"
#include "ModelExporter.h"

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

const TCHAR* GetFileFullNameFromPathName(const TCHAR* pszPathName, size_t uStrLen)
{
    assert(pszPathName != NULL);

    const TCHAR* ret = &pszPathName[uStrLen - 1];
    for(int x = uStrLen - 1; x >= 0; x--, ret--)
    {
        if(*ret == '\\')
            return ret + 1;
    }

    return NULL;
}

inline static void GetMaterialCnt(IGameMaterial* pRootMat, size_t& uMatCnt)
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

inline static IGameMaterial*		GetMaterial(size_t& index, IGameMaterial* pRootMat, IGameMaterial* pParent, int nChildID, IGameMaterial** pParentRet, int* pChildIDRet)
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

void CModelExporter::CollectMeshFaces( Tab<FaceEx*>& faceList, IGameMesh* pMesh, size_t uMatID, BOOL bMultiMat )
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
        faceList = pMesh->GetFacesFromMatID(uMatID);
}

void CModelExporter::ExportTexture( IGameNode* /*pNode*/, IGameMaterial* /*pMat*/, IGameTextureMap* pTex )
{
    const TCHAR* pszTexFileName = pTex->GetBitmapFileName();
    BEATS_ASSERT(pszTexFileName != NULL);
    std::string textureName = GetFileFullNameFromPathName(pszTexFileName, (size_t)_tcslen(pszTexFileName));
    if (textureName.empty())
    {
        MessageBox(NULL, _T("Export empty texture name!"), _T("Error"), MB_OK);
    }
    m_serializer << textureName;
}

void CModelExporter::ExportMaterialTexture( IGameNode* pNode, IGameMaterial* pMat )
{
    IGameTextureMap* pTex = GetTextureMap(pMat, ID_DI);
    if(pTex != NULL)
    {
        this->ExportTexture(pNode, pMat, pTex);
    }
    bool bSetOpacityTexture = GetTextureMap(pMat, ID_OP) != NULL;
    if (bSetOpacityTexture)
    {
        IGameTextureMap* pTex = GetTextureMap(pMat, ID_OP);
        const TCHAR* pszTexFileName = pTex->GetBitmapFileName();
        BEATS_ASSERT(pszTexFileName != NULL);
        std::string textureName = GetFileFullNameFromPathName(pszTexFileName, (size_t)_tcslen(pszTexFileName));
        bSetOpacityTexture = textureName.length() > 0;
    }
    m_serializer << bSetOpacityTexture;
}

bool CModelExporter::ExportMeshMaterial( IGameNode* pNode, IGameMesh* /*pMesh*/, IGameMaterial* pMat, size_t /*uMatID*/, BOOL /*bMultiMat*/ )
{
    BEATS_ASSERT(pMat->GetMaterialName());

    ExportMaterialTexture(pNode, pMat);
    return true;
}

void CModelExporter::ExportMeshVertex(IGameNode* /*pNode*/, IGameMesh* pMesh, IGameMaterial* pMat, size_t uMatID, BOOL bMultiMat)
{
    pMesh->SetUseWeightedNormals();
    pMesh->InitializeData();

    Tab<FaceEx*> faceTab;

    CollectMeshFaces(faceTab, pMesh, uMatID, bMultiMat);
    size_t uFaceCount = faceTab.Count();
    size_t uVertexCount = uFaceCount * 3;
    m_serializer << uVertexCount;

    BOOL bDiffusemap = GetTextureMap(pMat, ID_DI) == NULL ? FALSE : TRUE;
    BOOL bNormalmap = GetTextureMap(pMat, ID_BU) == NULL ? FALSE : TRUE;
    BOOL bSpecularmap = GetTextureMap(pMat, ID_SS) == NULL ? FALSE : TRUE;
    BOOL bLightmap = GetTextureMap(pMat, ID_AM) == NULL ? FALSE : TRUE;
    float centerX = 0;
    float centerY = 0;
    float centerZ = 0;
    for(size_t i = 0; i < uFaceCount; ++i)
    {
        FaceEx* pFace = faceTab[i];
        for(int  j = 0; j < 3; ++j)
        {
            DWORD mapIndex[3];
            Point3 ptUV;
            int indexUV = pFace->texCoord[j];
            int nChannel = 0;
            if(bDiffusemap || bNormalmap || bSpecularmap)
            {
                IGameTextureMap* pMap = GetTextureMap(pMat, ID_DI);
                nChannel = pMap->GetMapChannel();
            }
            else if(bLightmap)
            {
                IGameTextureMap* pMap = GetTextureMap(pMat, ID_AM);
                nChannel = pMap->GetMapChannel();
            }
            if(pMesh->GetMapFaceIndex(nChannel, pFace->meshFaceIndex, mapIndex))
                ptUV = pMesh->GetMapVertex(nChannel, mapIndex[j]);
            else
                ptUV = pMesh->GetMapVertex(nChannel, indexUV);
            int indexPos = pFace->vert[j];
            Point3 pos = pMesh->GetVertex(indexPos);

            m_serializer << pos.x << pos.y << pos.z;
            m_serializer << ptUV.x << ptUV.y;
            centerX += pos.x;
            centerY += pos.y;
            centerZ += pos.z;
        }
    }
    if (uFaceCount > 0)
    {
        centerX /= (uFaceCount * 3);
        centerY /= (uFaceCount * 3);
        centerZ /= (uFaceCount * 3);
        m_serializer << centerX << centerY << centerZ;
    }
}

void CModelExporter::ExportMesh( IGameNode* pNode )
{
    IGameMesh* pMesh = (IGameMesh*)pNode->GetIGameObject();

    pMesh->SetUseWeightedNormals();
    pMesh->InitializeData();
    pMesh->InitializeBinormalData();

    IGameMaterial* pRootMat = pNode->GetNodeMaterial();
    if(pRootMat != NULL)
    {
        size_t uMatCnt = 0;
        GetMaterialCnt(pRootMat, uMatCnt);

        m_serializer << uMatCnt;
        for(size_t x = 0; x < uMatCnt; x++)
        {
            size_t index = x;
            int nChildID;
            IGameMaterial* pParentMat;
            IGameMaterial* pMat = GetMaterial(index, pRootMat, NULL, -1, &pParentMat, &nChildID);

            int nMatID = 0;
            if(pParentMat != NULL)
            {
                nMatID = pParentMat->GetMaterialID(nChildID);
                BEATS_ASSERT(pParentMat->GetSubMaterial(nChildID) == pMat);
            }

            if(pMat != NULL)
            {
                ExportMeshMaterial(pNode, pMesh, pMat, nMatID, pRootMat->IsMultiType());
                ExportMeshVertex(pNode, pMesh,pMat,nMatID,pRootMat->IsMultiType());
            }
        }
    }

}

bool CModelExporter::ExportNode( IGameNode* pNode )
{
    IGameMesh* pMesh = (IGameMesh*)pNode->GetIGameObject();
    IGameMaterial* pRootMat = pNode->GetNodeMaterial();

    pMesh->SetUseWeightedNormals();
    pMesh->InitializeData();
    int uFaceCount = pMesh->GetNumberOfFaces();

    for(int i = 0; i < uFaceCount; ++i)
    {
        FaceEx* pFace = pMesh->GetFace(i);
        for(int  j = 0; j < 3; ++j)
        {
            DWORD mapIndex[3];
            Point3 ptUV;
            int indexUV = pFace->texCoord[j];
            IGameTextureMap* pMap = GetTextureMap(pRootMat, ID_DI);
            int nChannel = pMap->GetMapChannel();
            if(pMesh->GetMapFaceIndex(nChannel, pFace->meshFaceIndex, mapIndex))
                ptUV = pMesh->GetMapVertex(nChannel, mapIndex[j]);
            else
                ptUV = pMesh->GetMapVertex(nChannel, indexUV);

            int indexPos = pFace->vert[j];
            Point3 pos = pMesh->GetVertex(indexPos);
        }
    }

    return true;
}

void CModelExporter::Export(const char* pszFileName)
{
    m_serializer.Reset();
    size_t uTotalRootNodeCnt = m_pIGameScene->GetTopLevelNodeCount();
    std::vector<IGameNode*> meshNodeVector;
    for(size_t x = 0; x < uTotalRootNodeCnt; x++)
    {
        IGameNode* pNode = m_pIGameScene->GetTopLevelNode(x);
        IGameObject* pObject = pNode->GetIGameObject();

        IGameObject::ObjectTypes gameType = pObject->GetIGameType();

        if(gameType == IGameObject::IGAME_MESH)
        {
            meshNodeVector.push_back(pNode);
        }
    }

    int uMeshNodeCount = meshNodeVector.size();
    m_serializer << uMeshNodeCount;

    for(auto pNode : meshNodeVector)
    {
        ExportMesh(pNode);
    }

    m_serializer.Deserialize(pszFileName);
}

