#ifndef _DADAEXPORTER_H_
#define  _DADAEXPORTER_H_
#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"
#include "impexp.h"
#include "decomp.h"
#include <set>

struct stKeyValue 
{
    INT m_nKeyTime;
    std::string m_strKeyName;
};

struct SWeightData
{
    SWeightData()
        : uboneIndex(0xFF)
        , fWeight(0)
    {
    }
    bool operator < (const SWeightData& ref)
    {
        return fWeight < ref.fWeight;
    }
    uint8_t uboneIndex;
    float fWeight;
};

struct SSkinMeshData
{
    std::vector<Point3> m_vecPos;
    std::vector<Point3> m_vecUV;
    std::vector<std::vector<SWeightData>> m_vecWeightData;
    std::vector<std::string> m_vMaterialName;
    std::vector<int>  m_indices;
};

class CDataExporter
{
public:
    CDataExporter();
    virtual ~CDataExporter();

    bool ExportSkeletonAnimation();

    void ExportSkeleton(CSerializer& modelData);
    BOOL CollectBoneNode(Tab<IGameNode*>& res, IGameNode* pNode);
    BOOL ExportSkeletonNode(IGameNode* pNode, CSerializer& serializer);
    BOOL ExportAnimationNode(IGameNode* pNode, CSerializer& serializer);

    void ExportMesh(CSerializer& modelData);
    void GetSkinInfo();
    void ExportAnimation(CSerializer& modelData);
    void InitAminiationSegment();

    void InitNodeBones();
    void InitFrameInfo();

    void SetExportFileName(const TString& strFileName);
    uint8_t GetBoneIndex(const TString& strBoneName);

private:
    bool FilterVertexData(const Point3& ptPos, const Point3& ptUv);
    void MergeKeyFrame(std::set<int>& outList, const IGameKeyTab& keyList);

private:
    std::string   m_strFileName;
    IGameScene* m_pIGameScene;
    int     m_nBoneCount;
    int     m_nStartFrame;
    int     m_nEndFrame;
    uint8_t  m_uFPS;
    Tab<IGameNode*> m_listBones;
    std::set<TString> m_boneNameCheck;
    std::vector<stKeyValue> m_animationSegment;

    std::vector<SSkinMeshData> m_skinMeshDataList;
    std::vector<Point3> m_vecPos;
    std::vector<Point3> m_vecUV;
    std::vector<SWeightData> m_vecWeightData;

    std::vector<int>  m_vMeshVCount;
    std::vector<int>  m_vMeshMeterialCnt;
    std::vector<int>  m_posIndexVector;
    std::vector<int>  m_MeshPosIndexCnt;
};


#endif//_DADAEXPORTER_H_
