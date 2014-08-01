#ifndef BEYOND_ENGINE_SCENE_SCENEMANAGE_H__INCLUDE
#define BEYOND_ENGINE_SCENE_SCENEMANAGE_H__INCLUDE

class CScene;
class CNode;
class CLoadComponentFiles;

class CSceneManager
{
    typedef std::vector< CNode* > TNodeVector;
    typedef std::map< TString, CScene* > TSceneMap;
    BEATS_DECLARE_SINGLETON( CSceneManager );
public:
    unsigned int GetSceneNumber();

    CScene* GetSceneByName( const TString& name );

    CScene* GetCurrentScene();
    bool SetCurrentScene( CScene* pScene );

    void UpdateScene( float dtt );

    bool RenderScene();

    void AddScene( CScene* pScene );
    bool RemoveScene( CScene* pScene );
    bool ChangeAddSceneName( CScene* pScene );

    void SwitchScene(size_t uNewSceneComponentId);
    void SwitchSceneAsync(size_t uNewSceneComponentId);

    SharePtr<CLoadComponentFiles> GetLoadingSceneTask() const;

private:
    size_t m_uSwitchSceneFileId;
    CScene* m_pCurrentScene;
    TSceneMap m_createSceneMap;
    TSceneMap m_addScene;
    TNodeVector m_nodeVector;
    SharePtr<CLoadComponentFiles> m_loadingSceneTask;
    std::vector<size_t> m_unloadFiles;
};

#endif