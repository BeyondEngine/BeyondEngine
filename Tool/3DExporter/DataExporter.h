#ifndef _DADAEXPORTER_H_
#define  _DADAEXPORTER_H_
#include "IGame\IGame.h"
#include "IGame\IGameModifier.h"
#include "impexp.h"
#include "decomp.h"

struct stKeyValue 
{
    INT m_nKeyTime;
    std::string m_strKeyName;
};

struct SWeightData
{
    SWeightData()
        : type(eSBT_Null)
        , fWeight(0)
    {
    }
    bool operator < (const SWeightData& ref)
    {
        return fWeight < ref.fWeight;
    }
    ESkeletonBoneType type;
    float fWeight;
};

struct SSkinMeshData
{
    std::vector<Point3> m_vecPos;
    std::vector<Point3> m_vecUV;
    std::vector<SWeightData> m_vecWeightData;
    std::vector<std::string> m_vMaterialName;
    std::vector<int>  m_indices;
};

class CDataExporter
{
public:
	CDataExporter();
	virtual ~CDataExporter();

	void    ExportSkeletonAnimation();

	void    ExportSkeleton();
	BOOL    CollectBoneNode(Tab<IGameNode*>& res, IGameNode* pNode);
	BOOL    ExportSkeletonNode(IGameNode* pNode, CSerializer& serializer);
    BOOL	ExportAnimationNode(IGameNode* pNode, CSerializer& serializer);

	void    ExportMesh();
    void    GetSkinInfo();
	void	ExportSkinnedMesh();

	void	ExportAnimation();
    void    InitAminiationSegment();

    void    InitNodeBones();
    void    InitFrameInfo();

    void SetExportFileName(const TString& strFileName);

private:

    void    GetMaterialInfo(IGameNode* pNode);
    bool    FilterVertexData(const Point3& ptPos, const Point3& ptUv);

    std::string   m_strFileName;

    IGameScene* m_pIGameScene;

    int     m_nBoneCount;
    int     m_nStartFrame;
    int     m_nEndFrame;
    size_t  m_uFPS;
    Tab<IGameNode*> m_listBones;

    std::vector<stKeyValue> m_vKeys;

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
