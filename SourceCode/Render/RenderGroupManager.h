#ifndef BEYOND_ENGINE_RENDER_RENDERGROUPMANAGER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERGROUPMANAGER_H__INCLUDE

#include "RenderPublic.h"

class CRenderGroup;
class CCamera;
class CRenderGroupManager
{
    BEATS_DECLARE_SINGLETON(CRenderGroupManager);
public:
    CRenderGroup* GetRenderGroup();
    CRenderGroup* GetRenderGroupByID(ERenderGroupID renderGroupID);
    void Render();
    void Clear();
    void PushRenderGroupID(ERenderGroupID renderGroupID);
    ERenderGroupID PopRenderGroupID();
    const std::vector<ERenderGroupID>& GetRenderGroupIDStack() const;
    void SyncData();
private:
    CRenderGroup *createRenderGroup(ERenderGroupID groupID);

private:
    std::vector<ERenderGroupID> m_renderGroupIDStack;
    std::map<ERenderGroupID, CRenderGroup *> m_groupMap;
};

#endif // !RENDER_RENDER_GROUP_MANAGER_H__INCLUDE
