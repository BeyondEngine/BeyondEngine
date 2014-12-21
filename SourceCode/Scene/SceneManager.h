#ifndef BEYOND_ENGINE_SCENE_SCENEMANAGE_H__INCLUDE
#define BEYOND_ENGINE_SCENE_SCENEMANAGE_H__INCLUDE

class CScene;
class CNode;
class CSwitchSceneTask;
class CSceneManager
{
    BEATS_DECLARE_SINGLETON( CSceneManager );
public:
    CScene* GetCurrentScene();
    bool SetCurrentScene( CScene* pScene );

    void UpdateScene( float dtt );
    bool RenderScene();

    void AddScene( CScene* pScene );
    bool RemoveScene( CScene* pScene );
    bool ChangeAddSceneName( CScene* pScene );

    void LoadScene(const TString& strFileName);
    void SwitchScene(const TString& strFileName);
    bool SwitchSceneAsync(const TString& strFileName, bool UnloadBeforeLoad = true);
    uint32_t RegisterSwitchSceneCallBack(std::function<void()> fn, bool bAutoRemove = false);
    void UnregisterSwitchSceneCallBack(uint32_t id);

    uint32_t GetLoadingProgress() const;
    const std::map< uint32_t, CScene* >& GetLoadedScene();
    uint32_t GetSceneFileId(const TString& strFileName);

    void SetAutoTriggerOnEnterFlag(bool bAuto);
    bool GetAutoTriggerOnEnterFlag() const;
    void SetRenderSwitcher(bool bSwitcher);
    void SetUpdateSwitcher(bool bSwitcher);
    bool GetRenderSwitcher() const;
    bool GetUpdateSwitcher() const;
    void SetSwitchSceneState(bool bIsSwitching);
    bool GetSwitchSceneState() const;
    void TriggerOnEnterAction();
    void TriggerSwitchSceneAction();
    void Reset();
private:
    void SceneSwithNotify();
private:
    bool m_bSwitchingSceneState;
    bool m_bRenderSwitcher;
    bool m_bUpdateSwitcher;
    bool m_bAutoTriggerOnEnter;
    uint32_t m_uSwitchSceneCallbackIndex;
    CScene* m_pCurrentScene;
    std::map< uint32_t, CScene* > m_loadedScene;
    SharePtr<CSwitchSceneTask> m_pSwitchSceneTask;
    std::map<uint32_t, std::function<void()> >  m_switchSceneCallbackMap;
    std::set<uint32_t> m_autoRemoveList;
    // Cache the scene needs to call OnEnter in Update function.
    // Because we must make sure all other components are initialized when OnEnter is called.
    std::vector<CScene*> m_onEnterScene; 
};

#endif