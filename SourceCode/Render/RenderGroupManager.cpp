#include "stdafx.h"
#include "RenderGroupManager.h"
#include "Render/RenderGroup.h"
#include "RenderBatch.h"

CRenderGroupManager *CRenderGroupManager::m_pInstance = nullptr;

CRenderGroupManager::CRenderGroupManager()
{
}

CRenderGroupManager::~CRenderGroupManager()
{
    CRenderBatch::DestroyMatPool();
    for(auto group : m_groupMap)
    {
        BEATS_SAFE_DELETE(group.second);
    }
}

CRenderGroup* CRenderGroupManager::GetRenderGroup()
{
    BEATS_ASSERT(m_renderGroupIDStack.size() > 0);
    ERenderGroupID groupID = LAYER_3D;
    if (m_renderGroupIDStack.size() > 0)
    {
        groupID = m_renderGroupIDStack.back();
    }
    CRenderGroup *group = GetRenderGroupByID(groupID);
    return group;
}

CRenderGroup* CRenderGroupManager::GetRenderGroupByID(ERenderGroupID renderGroupID)
{
    CRenderGroup *group = nullptr;
    auto itr = m_groupMap.find(renderGroupID);
    if (itr == m_groupMap.end())
    {
        group = createRenderGroup(renderGroupID);
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
        group.second->Render();
    }
}

void CRenderGroupManager::Clear()
{
    for(auto group : m_groupMap)
    {
        group.second->Clear();
    }
    BEATS_ASSERT(m_renderGroupIDStack.size() == 0);
    m_renderGroupIDStack.clear();
}

void CRenderGroupManager::PushRenderGroupID(ERenderGroupID renderGroupID)
{
    BEATS_ASSERT(renderGroupID != LAYER_UNSET);
    m_renderGroupIDStack.push_back(renderGroupID);
}

ERenderGroupID CRenderGroupManager::PopRenderGroupID()
{
    BEATS_ASSERT(m_renderGroupIDStack.size() > 0);
    ERenderGroupID ret = m_renderGroupIDStack.back();
    m_renderGroupIDStack.pop_back();
    return ret;
}

const std::vector<ERenderGroupID>& CRenderGroupManager::GetRenderGroupIDStack() const
{
    return m_renderGroupIDStack;
}

void CRenderGroupManager::SyncData()
{
    for (auto group : m_groupMap)
    {
        group.second->SyncData();
    }
}

CRenderGroup *CRenderGroupManager::createRenderGroup(ERenderGroupID groupID)
{
    CRenderGroup *group = nullptr;
    bool bNeedScale = (groupID >= LAYER_GUI_MIN  && groupID <= LAYER_2D_MAX) ||
        groupID == LAYER_GUI_EDITOR ||
        groupID == LAYER_2D;
    bool bNeedClearDepth = groupID == LAYER_3D_BridgeAnimation;
    BEATS_ASSERT(groupID != LAYER_UNSET);
    group = new CRenderGroup(groupID, bNeedScale);
    group->SetClearDepthFlag(bNeedClearDepth);
    m_groupMap[groupID] = group;
    return group;
}

