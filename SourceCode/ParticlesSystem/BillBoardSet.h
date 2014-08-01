#ifndef BEYOND_ENGINE_PARTICLESYSTEM_BILLBOARDSET_H__INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_BILLBOARDSET_H__INCLUDE

#include "Scene/Node3D.h"
#include "Render/Material.h"
#include "Render/RenderBatch.h"

enum EBillboardType
{
    eBTFACE_CAMERA_TOTALLY = 0,
    eBTFACE_CAMERA = 1,
    eBTFACE_AXLEZ = 2,
    eBTFACE_AXLEY = 3,
    eBTFACE_AXLEX
};

class CBillboard;

class CBillboardSet : public CNode3D
{
public:
    CBillboardSet();
    virtual ~CBillboardSet();

    void SetType( EBillboardType type );

    virtual void DoRender() override;

    void AddBillboard( CBillboard* pBillBoard );
    void RemoveBillboard( CBillboard* pBillboard );

    void SetMaterial( SharePtr<CMaterial> pMaterial );
    SharePtr<CMaterial> GetMaterial() const;

    void Clear();

    size_t GetBillboardCount() const;

    void SortBillBoards();

private:
    void CalclationUniform();

    void CalclationUniformFaceCameraTotally( kmMat4& mat );
    void CalclationUniformFaceCamera( kmMat4& mat );
    void CalclationUniformFaceY( kmMat4& mat );
    void CalclationUniformFaceX( kmMat4& mat );

    void MatrixRotate( kmMat4& mat );

    void SortBillBoards( CBillboard** pBillBoard , int lowIndex, int highIndex, const kmVec3& cameraPosition );

private:
    EBillboardType m_eType;
    std::vector< CBillboard* > m_vecBillboards;
    SharePtr<CMaterial> m_pMaterial;
    //batch
    CRenderBatch* m_pRenderBatch;
    kmMat4 m_mat4Uniform;
    CShaderUniform m_uniform;
};
#endif // !BILLBOARDSET_H__INCLUDE

