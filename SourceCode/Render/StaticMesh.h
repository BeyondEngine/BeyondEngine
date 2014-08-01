#ifndef BEYOND_ENGINE_RENDER_STATICMESH_H__INCLUDE
#define BEYOND_ENGINE_RENDER_STATICMESH_H__INCLUDE

#include "Scene/Node3D.h"

class CStaticMeshData;

class CStaticMesh : public CNode3D
{
    DECLARE_REFLECT_GUID( CStaticMesh, 0xe3bb1421, CNode3D )
public:
    CStaticMesh();
    CStaticMesh(const TString &strFileName);
    virtual ~CStaticMesh();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
    virtual void Initialize() override;

    virtual void DoRender() override;

private:
    void LoadMeshData();

private:
    SReflectFilePath m_path;
    SharePtr<CStaticMeshData> m_pMeshData;
};

#endif
