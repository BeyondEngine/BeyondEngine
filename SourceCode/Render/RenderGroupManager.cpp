#include "stdafx.h"
#include "RenderGroupManager.h"
#include "RenderGroup.h"
#include "Render/RenderGroup.h"
#include "Camera.h"

CRenderGroupManager *CRenderGroupManager::m_pInstance = nullptr;

CRenderGroupManager::CRenderGroupManager()
    : m_pDefault3DCamera(NULL)
    , m_pDefault2DCamera(NULL)
{
}

CRenderGroupManager::~CRenderGroupManager()
{
    for(auto group : m_groupMap)
    {
        BEATS_SAFE_DELETE(group.second);
    }
}

CRenderGroup *CRenderGroupManager::GetRenderGroup(ERenderGroupID groupID)
{
    CRenderGroup *group = nullptr;
    auto itr = m_groupMap.find(groupID);
    if(itr == m_groupMap.end())
    {
        group = createRenderGroup(groupID);
    }
    else
    {
        group = itr->second;
    }
    return group;
}

void CRenderGroupManager::Render()
{
    for(auto group : m_groupMap)
    {
        if(group.second->PreRender())
        {
            group.second->Render();
            group.second->PostRender();
        }
    }
}

void CRenderGroupManager::Clear()
{
    for(auto group : m_groupMap)
    {
        group.second->Clear();
    }
}

void CRenderGroupManager::SetDefault3DCamera(CCamera* pDefaultCamera)
{
    BEATS_ASSERT(pDefaultCamera == NULL || pDefaultCamera->GetType() == CCamera::eCT_3D);
    m_pDefault3DCamera = pDefaultCamera;
}

CCamera* CRenderGroupManager::GetDefault3DCamera() const
{
    return m_pDefault3DCamera;
}

void CRenderGroupManager::SetDefault2DCamera(CCamera* pDefaultCamera)
{
    BEATS_ASSERT(pDefaultCamera == NULL || pDefaultCamera->GetType() == CCamera::eCT_2D);
    m_pDefault2DCamera = pDefaultCamera;
}

CCamera* CRenderGroupManager::GetDefault2DCamera() const
{
    return m_pDefault2DCamera;
}

CRenderGroup *CRenderGroupManager::createRenderGroup(ERenderGroupID groupID)
{
    BEATS_ASSERT(groupID & 0x0000FFFF, _T("Invalid render group ID: %X"), groupID);

    CRenderGroup *group = nullptr;
    if( groupID == LAYER_GUI ||
        groupID == LAYER_GUI_EDITOR ||
        groupID == LAYER_2D)
    {
        bool bShouldScaleContent = !(groupID < LAYER_3D_MAX && groupID > LAYER_3D_MIN);
        group = new CRenderGroup(groupID, bShouldScaleContent);
    }
    else
    {
        group = new CRenderGroup(groupID);
    }
    m_groupMap[groupID] = group;
    return group;
}

