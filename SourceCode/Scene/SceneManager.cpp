#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Node.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "Task/LoadComponentFiles.h"
#include "Task/TaskManager.h"
#include "GUI/Window/Control.h"

CSceneManager* CSceneManager::m_pInstance = nullptr;

CSceneManager::CSceneManager()
    : m_uSwitchSceneFileId(0xFFFFFFFF)
    , m_pCurrentScene(NULL)
    , m_loadingSceneTask(new CLoadComponentFiles)
{
}

CSceneManager::~CSceneManager()
{
    TSceneMap::iterator iter = m_createSceneMap.begin();
    for ( ; iter != m_createSceneMap.end(); ++iter)
    {
        BEATS_SAFE_DELETE( iter->second );
    }
    m_createSceneMap.clear();

    for ( auto itNode : m_nodeVector )
    {
        BEATS_SAFE_DELETE( itNode );
    }
    m_nodeVector.clear();
}

unsigned int CSceneManager::GetSceneNumber()
{
    return m_addScene.size();
}

CScene* CSceneManager::GetSceneByName( const TString& name )
{
    CScene* pRetScene = nullptr;
    TSceneMap::iterator iter = m_addScene.find( name );
    if ( iter != m_addScene.end() )
    {
        pRetScene = iter->second;
    }
    return pRetScene;
}

CScene* CSceneManager::GetCurrentScene()
{
    return m_pCurrentScene;
}

bool CSceneManager::SetCurrentScene( CScene* pScene )
{
    bool bRet = true;
    if (pScene != NULL)
    {
        TSceneMap::iterator iter = m_addScene.find( pScene->GetName() );
        if ( iter == m_addScene.end() )
        {
            bRet = false;
        }
    }

    if ( bRet && m_pCurrentScene != pScene)
    {
        if(m_pCurrentScene && m_pCurrentScene->IsInitialized())
        {
            m_pCurrentScene->Deactivate();
        }
        m_pCurrentScene = pScene;
        if(m_pCurrentScene)
        {
            m_pCurrentScene->Activate();
        }
    }
    return bRet;
}

void CSceneManager::AddScene( CScene* pScene )
{
    TSceneMap::iterator iter = m_addScene.find( pScene->GetName() );
    BEATS_ASSERT(iter == m_addScene.end(),
        _T("A scene with name: %s already exists!"), pScene->GetName().c_str());
    if ( iter == m_addScene.end() )
    {
        m_addScene.insert( std::make_pair( pScene->GetName(), pScene ));
        if ( m_addScene.size() + m_createSceneMap.size() == 1 )
        {
            SetCurrentScene( pScene );
        }
    }
}

bool CSceneManager::RemoveScene( CScene* pScene )
{
    bool ret = false;
    TSceneMap::iterator iter = m_addScene.find( pScene->GetName() );
    if ( iter != m_addScene.end() )
    {
        m_addScene.erase( iter );
        ret = true;
    }

    if ( m_addScene.size() > 0 )
    {
        SetCurrentScene( m_addScene.begin()->second );
    }
    else
    {
        SetCurrentScene( nullptr );
    }
    return ret;
}

bool CSceneManager::ChangeAddSceneName( CScene* pScene )
{
    bool ret = false;
    TSceneMap::iterator iter = m_addScene.begin();
    for ( ; iter != m_addScene.end(); ++iter )
    {
        if ( iter->second == pScene )
        {
            m_addScene.erase( iter );
            ret = true;
            break;
        }
    }
    if ( ret )
    {
        m_addScene.emplace( pScene->GetName(), pScene );
    }
    return ret;
}

void CSceneManager::SwitchScene(size_t uNewSceneComponentId)
{
    CScene* pOldScene = m_pCurrentScene;
    if (pOldScene != NULL)
    {
        pOldScene->OnLeave();
    }
    if (uNewSceneComponentId == pOldScene->GetId()) // HACK: Means we need to go back the base.
    {
        uNewSceneComponentId = 30;
    }
#ifdef _DEBUG
    size_t uOldSceneId = pOldScene->GetId();
#endif

#ifdef EDITOR_MODE
    size_t uNewSceneFileId = CComponentProxyManager::GetInstance()->GetProject()->QueryFileId(uNewSceneComponentId, false);
    const TString& strFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(uNewSceneFileId);
    CComponentProxyManager::GetInstance()->OpenFile(strFileName.c_str());
#else
    size_t uNewSceneFileId = CComponentInstanceManager::GetInstance()->GetProject()->QueryFileId(uNewSceneComponentId, false);
    CComponentInstanceManager::GetInstance()->SwitchFile(uNewSceneFileId);
#endif
    BEATS_ASSERT(m_pCurrentScene != NULL, _T("SwitchScene faield!"));
    BEATS_ASSERT(m_pCurrentScene->GetId() != uOldSceneId, _T("Scene hasn't changed after call switch Scene."));
    m_pCurrentScene->OnEnter();
}

void CSceneManager::SwitchSceneAsync(size_t uNewSceneComponentId)
{
#ifdef EDITOR_MODE
    size_t uFileId = CComponentProxyManager::GetInstance()->GetProject()->QueryFileId(uNewSceneComponentId, false);
#else
    size_t uFileId = CComponentInstanceManager::GetInstance()->GetProject()->QueryFileId(uNewSceneComponentId, false);
#endif
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    m_loadingSceneTask->GetFiles().clear();
    CComponentInstanceManager::GetInstance()->CalcSwitchFile(uFileId, m_loadingSceneTask->GetFiles(), m_unloadFiles);
    CTaskManager::GetInstance()->AddTask(m_loadingSceneTask, true);
    m_uSwitchSceneFileId = uFileId;
}

SharePtr<CLoadComponentFiles> CSceneManager::GetLoadingSceneTask() const
{
    return m_loadingSceneTask;
}

bool CSceneManager::RenderScene()
{
    bool bRet = false;
    if ( m_pCurrentScene )
    {
        m_pCurrentScene->Render();
        bRet = true;
    }
#ifdef EDITOR_MODE
    else
    {
        const std::map<size_t, CComponentProxy*>& componentsInScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
        for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            CComponentProxy* pProxy = iter->second;
            if (pProxy->GetProxyId() == pProxy->GetId() && 
                pProxy->GetHostComponent() &&
                pProxy->GetBeConnectedDependencyLines()->size() == 0)
            {
                CNode* pNode = dynamic_cast<CNode*>(pProxy->GetHostComponent());
                CControl* pControl = dynamic_cast<CControl*>( pNode );
                if (pNode && pNode->GetParentNode() == NULL && pControl == nullptr )
                {
                    pNode->Render();
                    bRet = true;
                }
            }
        }
    }
#endif
    return bRet;
}

void CSceneManager::UpdateScene( float dtt )
{
    if (m_uSwitchSceneFileId != 0xFFFFFFFF)
    {
        if (m_loadingSceneTask->GetProgress() == 100)
        {
            if (m_pCurrentScene != NULL)
            {
                m_pCurrentScene->OnLeave();
            }
            CComponentProject* pProject = CComponentInstanceManager::GetInstance()->GetProject();
            std::map<size_t, std::vector<size_t> >* pFileToComponentMap = pProject->GetFileToComponentMap();
            std::vector<CComponentBase*> unloadComponents;
            for (size_t i = 0; i < m_unloadFiles.size(); ++i)
            {
                auto iter = pFileToComponentMap->find(m_unloadFiles[i]);
                BEATS_ASSERT(iter != pFileToComponentMap->end());
                for (size_t j = 0; j <iter->second.size(); ++j)
                {
                    size_t uComponentId = iter->second.at(j);
                    CComponentBase* pComponent = CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId);
                    BEATS_ASSERT(pComponent != NULL);
                    unloadComponents.push_back(pComponent);
                    pComponent->Uninitialize();
                }
            }
            for (size_t i = 0; i < unloadComponents.size(); ++i)
            {
                BEATS_SAFE_DELETE(unloadComponents[i]);
            }
            unloadComponents.clear();
            std::vector<CComponentBase*>& loadedComponents = m_loadingSceneTask->GetLoadedComponents();
            CComponentInstanceManager::GetInstance()->ResolveDependency();
            for (size_t i = 0; i < loadedComponents.size(); ++i)
            {
                loadedComponents[i]->Initialize();
            }
            m_loadingSceneTask->GetFiles().clear();
            m_loadingSceneTask->GetLoadedComponents().clear();
            CComponentInstanceManager::GetInstance()->SetCurLoadFileId(m_uSwitchSceneFileId);
            m_uSwitchSceneFileId = 0xFFFFFFFF;
            if (m_pCurrentScene != NULL)
            {
                m_pCurrentScene->OnEnter();
            }
        }
    }
    if ( m_pCurrentScene )
    {
        m_pCurrentScene->Update( dtt );
    }
#ifdef EDITOR_MODE
    else
    {
        const std::map<size_t, CComponentProxy*>& componentsInScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
        for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            CComponentProxy* pProxy = iter->second;
            if (pProxy->GetProxyId() == pProxy->GetId() && 
                pProxy->GetHostComponent() &&
                pProxy->GetBeConnectedDependencyLines()->size() == 0)
            {
                CNode* pNode = dynamic_cast<CNode*>(pProxy->GetHostComponent());
                if (pNode && pNode->GetParentNode() == NULL && pNode->GetType() != eNT_NodeGUI )
                {
                    if (!pNode->IsActive())
                    {
                        pNode->Activate();
                    }
                    pNode->Update(dtt);
                }
            }
        }
    }
#endif
}