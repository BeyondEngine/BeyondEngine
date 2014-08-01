#ifndef MODELEXPORTER_H_INCLUDE
#define MODELEXPORTER_H_INCLUDE
#include "IGame\IGame.h"
#include "IGame\IGameModifier.h"
#include "impexp.h"
#include "decomp.h"
#include "Serializer.h"

class CModelExporter
{
public:
    CModelExporter();
    virtual ~CModelExporter();
    void CollectMeshFaces(Tab<FaceEx*>& faceList, IGameMesh* pMesh, UINT uMatID, BOOL bMultiMat);
    void ExportNodeModelMatrix(IGameNode* pNode);
    void ExportTexture(IGameNode* pNode, IGameMaterial* pMat, IGameTextureMap* pTex);
    void ExportMaterialTexture(IGameNode* pNode, IGameMaterial* pMat);
    bool ExportMeshMaterial(IGameNode* pNode, IGameMesh* pMesh, IGameMaterial* pMat, UINT uMatID, BOOL bMultiMat);
    void ExportMeshVertex(IGameNode* pNode, IGameMesh* pMesh, IGameMaterial* pMat, UINT uMatID, BOOL bMultiMat);
    void ExportMesh(IGameNode* pNode);
    bool ExportNode(IGameNode* pNode);
    void Export(const char* pszFileName);
private:
    IGameScene* m_pIGameScene;
    CSerializer m_serializer;
};
#endif//!MODELEXPORTER_H_INCLUDE
