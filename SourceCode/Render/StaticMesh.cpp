#include "stdafx.h"
#include "StaticMesh.h"
#include "Material.h"
#include "Resource/ResourceManager.h"
#include "RenderBatch.h"
#include "RenderGroup.h"
#include "StaticMeshData.h"

CStaticMesh::CStaticMesh()
{

}

CStaticMesh::CStaticMesh(const TString &strFileName)
{
    m_path.m_value = strFileName;
    Load();
}

CStaticMesh::~CStaticMesh()
{
}

void CStaticMesh::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_path, true, 0xFFFFFFFF, _T("ÎÄ¼þÂ·¾¶"), NULL, NULL, NULL);
}

bool CStaticMesh::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bHandled = super::OnPropertyChange(pVariableAddr, pSerializer);
    if(!bHandled)
    {
        if(pVariableAddr == &m_path)
        {
            if (m_pMeshData != NULL)
            {
                m_pMeshData->Unload();
                CResourceManager::GetInstance()->UnregisterResource(m_pMeshData);
            }
            DeserializeVariable(m_path, pSerializer);
            LoadMeshData();
            bHandled = true;
        }
    }
    return bHandled;
}

void CStaticMesh::Initialize()
{
    super::Initialize();
    LoadMeshData();
}

void CStaticMesh::LoadMeshData()
{
    if(!m_path.m_value.empty())
        m_pMeshData = CResourceManager::GetInstance()->GetResource<CStaticMeshData>(m_path.m_value);
}

void CStaticMesh::DoRender()
{
    if(m_pMeshData != NULL)
    {
        for(auto pSubMesh : m_pMeshData->GetSubMeshes())
        {
            pSubMesh->m_pRenderBatch->SetTransform(GetWorldTM());
            pSubMesh->m_pRenderBatch->GetGroup()->AddRenderBatch(pSubMesh->m_pRenderBatch);
        }
    }
}

