#include "stdafx.h"
#include "DataExporter.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "3DSceneExport.h"
#include "notetrck.h"
#include "FilePathTool.h"

#define  MAX_BONE_BLEND 4

extern IGameTextureMap* GetTextureMap(IGameMaterial* pMat, int nSlot);

inline static void GetMaterialCnt(IGameMaterial* pRootMat, UINT& uMatCnt)
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
        INT nSubMatCnt = pRootMat->GetSubMaterialCount();
        for(INT x = 0; x<nSubMatCnt; x++)
        {
            IGameMaterial* pSubMat = pRootMat->GetSubMaterial(x);
            assert(pSubMat != NULL);
            GetMaterialCnt(pSubMat, uMatCnt);
        }
    }
}

inline static IGameMaterial* GetMaterial(UINT& index, IGameMaterial* pRootMat, IGameMaterial* pParent, INT nChildID, IGameMaterial** pParentRet, INT* pChildIDRet)
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
        INT nSubMatCnt = pRootMat->GetSubMaterialCount();

        for(INT x = 0; x < nSubMatCnt; x++)
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

        INT nSubMatCnt = pRootMat->GetSubMaterialCount();
        for(INT x = 0; x<nSubMatCnt; x++)
        {
            IGameMaterial* pSubMat = pRootMat->GetSubMaterial(x);
            pSubMat = GetMaterial(index, pSubMat, pRootMat, x, pParentRet, pChildIDRet);
            if(pSubMat != NULL)
                return pSubMat;
        }
    }
    else
    {
        INT nSubMatCnt = pRootMat->GetSubMaterialCount();
        for(INT x = 0; x < nSubMatCnt; x++)
        {
            IGameMaterial* pSubMat = pRootMat->GetSubMaterial(x);
            pSubMat = GetMaterial(index, pSubMat, pRootMat, x, pParentRet, pChildIDRet);
            if(pSubMat != NULL)
                return pSubMat;
        }
    }

    return NULL;
}

CDataExporter::CDataExporter()
    : m_nBoneCount(0)
    ,m_nStartFrame(0)
    ,m_nEndFrame(0)
{
    m_pIGameScene = GetIGameInterface();
    m_pIGameScene ->InitialiseIGame();
}

CDataExporter::~CDataExporter()
{
    m_pIGameScene->ReleaseIGame();
}

void CDataExporter::InitNodeBones()
{
    m_boneNameCheck.clear();
    UINT uTotalRootNodeCnt = m_pIGameScene->GetTopLevelNodeCount();
    for(UINT x = 0; x < uTotalRootNodeCnt; x++)
    {
        IGameNode* pNode = m_pIGameScene->GetTopLevelNode(x);
        CollectBoneNode(m_listBones, pNode);
    }
}

void CDataExporter::InitFrameInfo()
{
    TimeValue start_time = m_pIGameScene->GetSceneStartTime();
    TimeValue end_time = m_pIGameScene->GetSceneEndTime();
    INT nTickPerFrame = m_pIGameScene->GetSceneTicks();
    m_nStartFrame = start_time / nTickPerFrame;
    m_nEndFrame = end_time / nTickPerFrame;
    m_uFPS = TIME_TICKSPERSEC / nTickPerFrame;
}

void CDataExporter::SetExportFileName(const TString& strFileName)
{
    m_strFileName = strFileName;
}

uint8_t CDataExporter::GetBoneIndex(const TString& strBoneName)
{
    uint8_t uBoneIndex = 0xFF;
    BEATS_ASSERT(!strBoneName.empty(), _T("Bone name will never be empty!"));
    if (!strBoneName.empty())
    {
        for (int32_t i = 0; i < m_listBones.Count(); ++i)
        {
            if (m_listBones[i]->GetName() == strBoneName)
            {
                uBoneIndex = (uint8_t)i;
                break;
            }
        }
    }
    BEATS_ASSERT(uBoneIndex != 0xFF);
    return uBoneIndex;
}

bool CDataExporter::ExportSkeletonAnimation()
{
    bool bRet = false;
    InitNodeBones();
    InitFrameInfo();
    m_pIGameScene->SetStaticFrame(0);

    CSerializer modeldata;
    ExportMesh(modeldata);
    ExportSkeleton(modeldata);
    ExportAnimation(modeldata);
    if (modeldata.GetWritePos() > 0)
    {
        std::string filename = m_strFileName;
        filename = filename.substr(0, filename.length() - 4);
        filename.append(_T(".model"));
        modeldata.Deserialize(filename.c_str());
        bRet = true;
    }
    return bRet;
}

void CDataExporter::ExportSkeleton(CSerializer& modelData)
{
    m_pIGameScene->SetStaticFrame(0);

    uint32_t uBoneCount = (uint32_t)m_listBones.Count();
    if (uBoneCount > 0)
    {
        CSerializer serializer;
        serializer << uBoneCount;
        if (uBoneCount >= 32)
        {
            TCHAR szBuffer[MAX_PATH];
            _stprintf_s(szBuffer, MAX_PATH, "骨骼的数量不能大于31, 当前骨骼数量为 %d, 导出失败！", uBoneCount);
            MessageBox(NULL, szBuffer, "骨骼过多", MB_OK);
        }
        for (UINT x = 0; x < uBoneCount; x++)
        {
            IGameNode* pNode = m_listBones[x];
            assert(pNode != NULL);
            ExportSkeletonNode(pNode, serializer);
        }
        serializer.SetReadPos(0);
        modelData.Serialize(serializer);
    }
}

void CDataExporter::ExportMesh(CSerializer& modelData)
{
    GetSkinInfo();
    if (m_skinMeshDataList.size() > 0)
    {
        CSerializer serializer;
        serializer << m_skinMeshDataList.size();
        for (uint32_t i = 0; i < m_skinMeshDataList.size(); ++i)
        {
            uint32_t uMaterialSize = m_skinMeshDataList[i].m_vMaterialName.size();
            if (uMaterialSize > 1)
            {
                TCHAR szBuffer[MAX_PATH];
                _stprintf_s(szBuffer, MAX_PATH, "材质的数量不能大于1, 当前骨骼数量为 %d, 导出失败！", uMaterialSize);
                MessageBox(NULL, szBuffer, "材质过多", MB_OK);
            }
            serializer << uMaterialSize;

            for (uint32_t j = 0; j < uMaterialSize; ++j)
            {
                serializer << m_skinMeshDataList[i].m_vMaterialName[j];
            }

            //vertex data
            int iMeshVertexCount = m_skinMeshDataList[i].m_vecPos.size();
            serializer << iMeshVertexCount;

            for (uint32_t j = 0; j < m_skinMeshDataList[i].m_vecPos.size(); ++j)
            {
                const SSkinMeshData& currMeshData = m_skinMeshDataList[i];
                serializer << currMeshData.m_vecPos[j].x << currMeshData.m_vecPos[j].y << currMeshData.m_vecPos[j].z;
                serializer << currMeshData.m_vecUV[j].x << currMeshData.m_vecUV[j].y;
                uint8_t cBoneWeightSize = currMeshData.m_vecWeightData[j].size();
                serializer << cBoneWeightSize;
                for (uint32_t z = 0; z < cBoneWeightSize; ++z)
                {
                    const SWeightData& weightdata = m_skinMeshDataList[i].m_vecWeightData[j][z];
                    serializer << weightdata.uboneIndex << weightdata.fWeight;
                }
            }

            //PosIndex
            int iMeshPosIndexCount = m_skinMeshDataList[i].m_indices.size();
            serializer << iMeshPosIndexCount;
            for (int j = 0; j < iMeshPosIndexCount; ++j)
            {
                serializer << m_skinMeshDataList[i].m_indices[j];
            }
        }

        serializer.SetReadPos(0);
        modelData.Serialize(serializer);
    }
}

BOOL CDataExporter::CollectBoneNode(Tab<IGameNode*>& res, IGameNode* pNode)
{
    IGameObject* pObject = pNode->GetIGameObject();

    IGameObject::ObjectTypes gameType = pObject->GetIGameType();
    IGameObject::MaxType maxType = pObject->GetMaxType();

    BOOL bBone = gameType == IGameObject::IGAME_BONE && maxType != IGameObject::IGAME_MAX_HELPER;

    pNode->ReleaseIGameObject();

    if (bBone)
    {
        TString strBoneName = pNode->GetName();
        BEATS_ASSERT(!strBoneName.empty(), "Bone Name can't be empty!");
        BEATS_ASSERT(m_boneNameCheck.find(strBoneName) == m_boneNameCheck.end(), "Bone name %s is repeated!", strBoneName.c_str());
        m_boneNameCheck.insert(strBoneName);
        res.Append(1, &pNode);
    }

    INT nChildCnt = pNode->GetChildCount();
    for(INT x = 0; x < nChildCnt; x++)
    {
        IGameNode* pChild = pNode->GetNodeChild(x);
        BEATS_ASSERT(pChild != NULL);
        CollectBoneNode(res, pChild);
    }

    return TRUE;
}

BOOL CDataExporter::ExportSkeletonNode(IGameNode* pNode, CSerializer& serializer)
{
#ifdef _DEBUG
    IGameObject* pObject = pNode->GetIGameObject();
    IGameObject::ObjectTypes gameType = pObject->GetIGameType();
    IGameObject::MaxType maxType = pObject->GetMaxType();

    BEATS_ASSERT(gameType == IGameObject::IGAME_BONE && maxType != IGameObject::IGAME_MAX_HELPER, _T("Error at exporting a bone!"));
#endif

    const TString strBoneName = pNode->GetName();
    uint8_t uBoneIndex = GetBoneIndex(strBoneName);
    IGameNode* pParentNode = pNode->GetNodeParent();
    uint8_t uParentNodeIndex = 0xFF;
    if ( pParentNode )
    {
        const TString strParentBoneName = pParentNode->GetName();
        uParentNodeIndex = GetBoneIndex(strParentBoneName);
    }
    serializer << strBoneName << uBoneIndex << uParentNodeIndex;

    GMatrix matParent;
    GMatrix matRelative;
    GMatrix matWorld = pNode->GetWorldTM();

    if(pParentNode)
    {
        matParent = pParentNode->GetWorldTM();
        if(matParent == matWorld)
        {
            matRelative.SetIdentity();
        }
        else
        {
             matRelative = matWorld * matParent.Inverse();
        }
    }
    else
    {
        matRelative = matWorld;
    }

    Point3 translate = matRelative.Translation();
    Point3 scale = matRelative.Scaling();
    matRelative.SetRow(3, Point4(0, 0, 0, 1)); // Clear translation
    GMatrix transposeMat;
    for (int i = 0; i < 4; ++i)
    {
        transposeMat.SetRow(i, matRelative.GetColumn(i));
    }
    Quat quaternion = transposeMat.Rotation();
    quaternion.Normalize();
    serializer << scale.x << scale.y << scale.z;
    serializer << quaternion.x << quaternion.y << quaternion.z << quaternion.w;
    serializer << translate.x << translate.y << translate.z;

    return TRUE;
}

BOOL CDataExporter::ExportAnimationNode(IGameNode* pNode, CSerializer& serializer)
{
#ifdef _DEBUG
    IGameObject* pObject = pNode->GetIGameObject();
    IGameObject::ObjectTypes gameType = pObject->GetIGameType();
    IGameObject::MaxType maxType = pObject->GetMaxType();

    BOOL bBone = gameType == IGameObject::IGAME_BONE && maxType != IGameObject::IGAME_MAX_HELPER;
    BEATS_ASSERT(bBone, _T("Error at exporting a bone! ExportAnimationNode"));
#endif // _DEBUG

    GMatrix matParent;
    GMatrix matRelative;
    GMatrix matWorld = pNode->GetWorldTM();
    IGameNode* pParentNode = pNode->GetNodeParent();
    if(pParentNode)
    {
        matParent = pParentNode->GetWorldTM();
        if(matParent == matWorld)
        {
            matRelative.SetIdentity();
        }
        else
        {
            matRelative = matWorld * matParent.Inverse();
        }
    }
    else
    {
        matRelative = matWorld;
    }

    Point3 translate = matRelative.Translation();
    Point3 scale = matRelative.Scaling();
    matRelative.SetRow(3, Point4(0, 0, 0, 1)); // Clear translation
    GMatrix transposeMat;
    for (int i = 0; i < 4; ++i)
    {
        transposeMat.SetRow(i, matRelative.GetColumn(i));
    }
    Quat quaternion = transposeMat.Rotation();
    quaternion.Normalize();
    serializer << scale.x << scale.y << scale.z;
    serializer << quaternion.x << quaternion.y << quaternion.z << quaternion.w;
    serializer << translate.x << translate.y << translate.z;
    return TRUE;
}

bool CDataExporter::FilterVertexData(const Point3& ptPos, const Point3& ptUv)
{
    bool bRet = true;
    int iSizePos = m_vecPos.size();
    int iIndex = 0;
    for(iIndex = 0; iIndex < iSizePos; iIndex++)
    {
        if(ptPos == m_vecPos[iIndex] && ptUv == m_vecUV[iIndex])
        {
            break;
        }
    }

    if(iIndex == iSizePos)
    {
        m_vecUV.push_back(ptUv);
        m_vecPos.push_back(ptPos);
        bRet = false;
    }
    
    m_posIndexVector.push_back(iIndex);

    return bRet;
}

void CDataExporter::GetSkinInfo()
{
    m_vecPos.clear();
    m_vecUV.clear();
    m_vecWeightData.clear();
    m_vMeshVCount.clear();
    m_posIndexVector.clear();
    m_skinMeshDataList.clear();
    UINT uTotalRootNodeCnt = m_pIGameScene->GetTopLevelNodeCount();
    for(UINT x = 0; x < uTotalRootNodeCnt; x++)
    {
        IGameNode* pNode = m_pIGameScene->GetTopLevelNode(x);
        IGameObject* pObject = pNode->GetIGameObject();

        IGameObject::ObjectTypes gameType = pObject->GetIGameType();

        if(gameType == IGameObject::IGAME_MESH)
        {
            SSkinMeshData meshData;

            // 1. Get all material data.
            IGameMaterial* pMaterial = pNode->GetNodeMaterial();
            if(pMaterial != NULL)
            {
                UINT uMatCnt = 0;
                GetMaterialCnt(pMaterial, uMatCnt);
                for(UINT x = 0; x < uMatCnt; x++)
                {
                    UINT index = x;
                    INT nChildID;
                    IGameMaterial* pParentMat;
                    IGameMaterial* pMat = GetMaterial(index, pMaterial, NULL, -1, &pParentMat, &nChildID);
                    IGameTextureMap* pTextureMap = GetTextureMap(pMat, ID_DI);
                    BEATS_ASSERT(pTextureMap != NULL);
                    const TCHAR* pszTexFileName = pTextureMap->GetBitmapFileName();
                    std::string textureName = CFilePathTool::GetInstance()->FileName(pszTexFileName);
                    meshData.m_vMaterialName.push_back(textureName);
                }
            }

            IGameMesh* pMesh = (IGameMesh*)pNode->GetIGameObject();
            IGameSkin* pSkin = pMesh->GetIGameSkin();
            if (pSkin)
            {
                pMesh->SetUseWeightedNormals();
                pMesh->InitializeData();
                Tab<int> mapNums = pMesh->GetActiveMapChannelNum();
                BEATS_ASSERT(mapNums.Count() == 1);
                int uFaceCount = pMesh->GetNumberOfFaces();

                for (int i = 0; i < uFaceCount; ++i)
                {
                    FaceEx* pFace = pMesh->GetFace(i);
                    DWORD mapIndex[3];
                    bool bAccessUV = pMesh->GetMapFaceIndex(mapNums[0], pFace->meshFaceIndex, mapIndex);
                    for (int j = 0; j < 3; ++j)
                    {
                        Point3 ptUV = pMesh->GetMapVertex(mapNums[0], bAccessUV ? mapIndex[j] : pFace->texCoord[j]);
                        int indexPos = pFace->vert[j];
                        Point3 pos = pMesh->GetVertex(indexPos);

                        //Filter the pos
                        uint32_t uVertexCount = meshData.m_vecPos.size();
                        uint32_t uVertexIndex = 0;
                        for (; uVertexIndex < uVertexCount; ++uVertexIndex)
                        {
                            if (meshData.m_vecPos[uVertexIndex] == pos && meshData.m_vecUV[uVertexIndex] == ptUV)
                            {
                                break;
                            }
                        }
                        bool bNewVertex = uVertexIndex == uVertexCount;
                        if (bNewVertex)
                        {
                            meshData.m_vecPos.push_back(pos);
                            meshData.m_vecUV.push_back(ptUV);
                        }
                        meshData.m_indices.push_back(uVertexIndex);

                        if (bNewVertex)
                        {
                            int uEffectedBoneCount = pSkin->GetNumberOfBones(indexPos);
                            float fWeightSum = 0;
                            int nValidDataCounter = 0;
                            std::vector<SWeightData> weightData;
                            // HACK: Sometimes the uEffectedBoneCount will be greater than MAX_BONE_BLEND, so we filter those data with 0 weight.
                            // This should never happens.
                            for (int w = 0; w < uEffectedBoneCount && w < MAX_BONE_BLEND; ++w)
                            {
                                float fWeight = pSkin->GetWeight(indexPos, w);
                                if (!BEATS_FLOAT_EQUAL(fWeight, 0))
                                {
                                    SWeightData data;
                                    INode* pBone = pSkin->GetBone(indexPos, w);
                                    const char* pszBoneName = pBone->GetName();
                                    uint8_t boneIndex = GetBoneIndex(pszBoneName);
                                    data.uboneIndex = boneIndex;
                                    data.fWeight = fWeight;
                                    fWeightSum += fWeight;
                                    weightData.push_back(data);
                                }
                            }
                            meshData.m_vecWeightData.push_back(weightData);
                            BEATS_ASSERT(fWeightSum > 0.98f && fWeightSum < 1.01f);
                        }
                    }
                }
                m_skinMeshDataList.push_back(meshData);
            }
        }
        pNode->ReleaseIGameObject();
    }
}

void CDataExporter::ExportAnimation(CSerializer& modelData)
{
    InitAminiationSegment();

    uint8_t uBoneCount = (uint8_t)m_listBones.Count();
    if (uBoneCount > 0)
    {
        CSerializer serializer;
        uint32_t animationCount = m_animationSegment.size();
        if (animationCount > 0)
        {
            modelData << animationCount << m_uFPS;

            int16_t _nStartFrame = m_nStartFrame;
            uint16_t uFrameCount = 0;
            for (uint32_t i = 0; i < animationCount; i++)
            {
                int _nEndFrame = m_animationSegment[i].m_nKeyTime;
                uFrameCount = _nEndFrame - _nStartFrame + 1;
                const TString& strAniName = m_animationSegment[i].m_strKeyName;
                if (strAniName.find("\r") != TString::npos ||
                    strAniName.find("\n") != TString::npos)
                {
                    TCHAR szBuffer[MAX_PATH];
                    _stprintf_s(szBuffer, MAX_PATH, "骨骼动画名 %s 中不能包含回车或者换行！", strAniName.c_str());
                    MessageBox(BEYONDENGINE_HWND, szBuffer, "导出错误", MB_OK);
                }
                serializer << uFrameCount << strAniName << _nStartFrame;
                for (uint32_t x = 0; x < (uint32_t)m_listBones.Count(); ++x)
                {
                    IGameNode* pNode = m_listBones[x];
                    BEATS_ASSERT(pNode != NULL);
                    auto pControl = pNode->GetIGameControl();
                    std::set<int> keyList;
                    IGameKeyTab posList;
                    pControl->GetQuickSampledKeys(posList, IGAME_POS);
                    MergeKeyFrame(keyList, posList);
                    IGameKeyTab rotList;
                    pControl->GetQuickSampledKeys(rotList, IGAME_ROT);
                    MergeKeyFrame(keyList, rotList);
                    IGameKeyTab scaleList;
                    pControl->GetQuickSampledKeys(scaleList, IGAME_SCALE);
                    MergeKeyFrame(keyList, scaleList);
                    serializer << keyList.size();
                    for (auto iter = keyList.begin(); iter != keyList.end(); ++iter)
                    {
                        uint16_t keyIndex = *iter;
                        serializer << keyIndex;
                        m_pIGameScene->SetStaticFrame(keyIndex);
                        ExportAnimationNode(pNode, serializer);
                    }
                }
                _nStartFrame = _nEndFrame + 1;
            }
            serializer.SetReadPos(0);
            modelData.Serialize(serializer);
        }
    }
}

void CDataExporter::InitAminiationSegment()
{
    BEATS_ASSERT(m_pIGameScene->GetTopLevelNodeCount() > 0);

    IGameNode* pGameNode = m_pIGameScene->GetTopLevelNode(0);
    INode* pNode = pGameNode->GetMaxNode()->GetParentNode();
   
    int numNoteTracks = pNode->NumNoteTracks();
   
    INT nTickPerFrame = m_pIGameScene->GetSceneTicks();
    for(int i = 0; i < numNoteTracks; i++)
    {
        DefNoteTrack* nt = (DefNoteTrack*)pNode->GetNoteTrack(i);
        for (int j = 0; j < nt->keys.Count(); j++)
        { 
           stKeyValue keyValue;
           keyValue.m_nKeyTime  = ( nt->GetKeyTime(j) / nTickPerFrame );
           keyValue.m_strKeyName =  nt->keys[j]->note;
           m_animationSegment.push_back( keyValue );
        }
    }
}

void CDataExporter::MergeKeyFrame(std::set<int>& outList, const IGameKeyTab& keyList)
{
    int nTickPerFrame = m_pIGameScene->GetSceneTicks();
    for (int i = 0; i < keyList.Count(); ++i)
    {
        int nIndex = keyList[i].t / nTickPerFrame;
        outList.insert(nIndex);
    }
}