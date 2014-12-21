#ifndef MODELEXPORTER_H_INCLUDE
#define MODELEXPORTER_H_INCLUDE
#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"
#include "impexp.h"
#include "decomp.h"
#include "Utility/BeatsUtility/Serializer.h"

class CModelExporter
{
public:
    CModelExporter();
    virtual ~CModelExporter();
    void CollectMeshFaces(Tab<FaceEx*>& faceList, IGameMesh* pMesh, UINT uMatID, BOOL bMultiMat);
    void ExportNodeModelMatrix(IGameNode* pNode);
    bool ExportMeshMaterial(IGameNode* pNode, IGameMesh* pMesh, IGameMaterial* pMat, UINT uMatID, BOOL bMultiMat);
    void ExportMeshVertex(IGameNode* pNode, IGameMesh* pMesh, IGameMaterial* pMat, UINT uMatID, BOOL bMultiMat);
    void ExportMesh(IGameNode* pNode);
    void ExportMeshAnimation(IGameNode* pNode, IGameControlType animationType);
    void Export(const char* pszFileName);

private:
    void RefreshRootBoundingBox(const Box3& newBox);
    
private:
    IGameScene* m_pIGameScene;
    CSerializer m_serializer;
    Box3 m_rootBoundingBox;
};
#endif//!MODELEXPORTER_H_INCLUDE
