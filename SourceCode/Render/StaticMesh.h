#ifndef BEYOND_ENGINE_RENDER_STATICMESH_H__INCLUDE
#define BEYOND_ENGINE_RENDER_STATICMESH_H__INCLUDE

#include "Scene/Node3D.h"
#include "NodeAnimation/NodeAnimation.h"
#include "MathExt/AABBBox.h"

class CStaticMeshData;
class CRenderGroup;
struct SSubMesh;
class CStaticMesh : public CNode3D
{
    DECLARE_REFLECT_GUID( CStaticMesh, 0xe3bb1421, CNode3D )
public:
    CStaticMesh();
    virtual ~CStaticMesh();

    virtual void ReflectData(CSerializer& serializer) override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    virtual bool Load() override;
    virtual void DoRender() override;
    void PlayModelAnimation(ENodeAnimationPlayType type);
    void SetFilePath(const TString& strFilePath);
    void SetMeshData(SharePtr<CStaticMeshData> pMeshData);
    CAABBBox GetAABB() const;
    SharePtr<CStaticMeshData> GetMeshData() const;

private:
    void LoadMeshData();

private:
    bool m_bAddedBlend = false;
    uint32_t m_uSpecifiedFrame;
    float m_fPlaySpeed = 1.0f;
    ENodeAnimationPlayType m_animationType;
    SReflectFilePath m_path;
    SharePtr<CStaticMeshData> m_pMeshData;
    std::vector<CNodeAnimation*> m_meshAnimationList;
};

#endif
