#ifndef BEYOND_ENGINE_RENDER_RENDERGROUPMANAGER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERGROUPMANAGER_H__INCLUDE

class CRenderGroup;
class CCamera;
class CRenderGroupManager
{
    BEATS_DECLARE_SINGLETON(CRenderGroupManager);
public:
    enum ERenderGroupID
    {
        LAYER_3D_MIN = 0,

        LAYER_3D = LAYER_3D_MIN + (1 << 8),
        LAYER_3D_ALPHA = LAYER_3D_MIN + (2 << 8),

        LAYER_3D_MAX = (1 << 8) << 8,
        LAYER_2D_MIN = LAYER_3D_MAX,

        LAYER_2D = LAYER_2D_MIN + (4 << 8),

        LAYER_2D_MAX = (2 << 8) << 8,
        LAYER_GUI_MIN = LAYER_2D_MAX,

        LAYER_GUI = LAYER_GUI_MIN + (5 << 8),
        LAYER_GUI_EDITOR = LAYER_GUI_MIN + (6 << 8),

        LAYER_GUI_MAX = (3 << 8) << 8,

        LAYER_USER = 100 << 8,
    };

    CRenderGroup *GetRenderGroup(ERenderGroupID groupID);

    void Render();
    void Clear();
    void SetDefault3DCamera(CCamera* pDefaultCamera);
    CCamera* GetDefault3DCamera() const;
    void SetDefault2DCamera(CCamera* pDefaultCamera);
    CCamera* GetDefault2DCamera() const;

private:
    CRenderGroup *createRenderGroup(ERenderGroupID groupID);

private:
    CCamera* m_pDefault3DCamera;
    CCamera* m_pDefault2DCamera;
    std::map<ERenderGroupID, CRenderGroup *> m_groupMap;
};

#endif // !RENDER_RENDER_GROUP_MANAGER_H__INCLUDE
