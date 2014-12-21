#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Node.h"
#include "Component/Component/ComponentProject.h"
#include "Task/TaskManager.h"
#include "Task/SwitchSceneTask.h"
#include "ParticleSystem/ParticleEmitter.h"
CSceneManager* CSceneManager::m_pInstance = nullptr;

CSceneManager::CSceneManager()
    : m_bSwitchingSceneState(false)
    , m_bRenderSwitcher(true)
    , m_bUpdateSwitcher(true)
    , m_bAutoTriggerOnEnter(true)
    , m_uSwitchSceneCallbackIndex(0)
    , m_pCurrentScene(NULL)
    , m_pSwitchSceneTask(new CSwitchSceneTask)
{
    BEATS_ASSERT(!CApplication::GetInstance()->IsDestructing(), "Should not create singleton when exit the program!");
    m_pSwitchSceneTask->Initialize();
}

CSceneManager::~CSceneManager()
{
    m_pSwitchSceneTask->Uninitialize();
}

CScene* CSceneManager::GetCurrentScene()
{
    return m_pCurrentScene;
}

bool CSceneManager::SetCurrentScene( CScene* pScene )
{
    bool bRet = true;
    if (m_pCurrentScene != pScene)
    {
        if (m_pCurrentScene)
        {
            BEATS_ASSERT(m_pCurrentScene->IsInitialized());
            m_pCurrentScene->Deactivate();
            m_pCurrentScene->OnLeave();
        }
        m_pCurrentScene = pScene;
        if (m_pCurrentScene != NULL)
        {
#ifdef _DEBUG
            CComponentProject* pProject = CEngineCenter::GetInstance()->GetComponentManager()->GetProject();
            uint32_t uFileId = pProject->QueryFileId(m_pCurrentScene->GetId(), true);
            std::map< uint32_t, CScene* >::iterator iter = m_loadedScene.find(uFileId);
            BEATS_ASSERT(iter != m_loadedScene.end());
#endif
            m_pCurrentScene->Activate();
            BEATS_ASSERT(std::find(m_onEnterScene.begin(), m_onEnterScene.end(), m_pCurrentScene) == m_onEnterScene.end());
            m_onEnterScene.push_back(m_pCurrentScene);
            TriggerSwitchSceneAction();
        }
    }
    return bRet;
}

void CSceneManager::AddScene( CScene* pScene )
{
    CComponentProject* pProject = CEngineCenter::GetInstance()->GetComponentManager()->GetProject();
    uint32_t uFileId = pProject->QueryFileId(pScene->GetId(), true);
    std::map< uint32_t, CScene* >::iterator iter = m_loadedScene.find(uFileId);
    BEATS_ASSERT(iter == m_loadedScene.end(), _T("Only one scene component can exist in a file"));
    m_loadedScene[uFileId] = pScene;
    if (m_pCurrentScene == NULL)
    {
        SetCurrentScene(pScene);
    }
}

bool CSceneManager::RemoveScene( CScene* pScene )
{
    bool ret = false;
    CComponentProject* pProject = CEngineCenter::GetInstance()->GetComponentManager()->GetProject();
    uint32_t uFileId = pProject->QueryFileId(pScene->GetId(), true);
    std::map< uint32_t, CScene* >::iterator iter = m_loadedScene.find(uFileId);
    if ( iter != m_loadedScene.end() )
    {
        m_loadedScene.erase( iter );
        ret = true;
    }

    if ( m_pCurrentScene == pScene )
    {
        SetCurrentScene(nullptr);
    }
    return ret;
}

void CSceneManager::SceneSwithNotify()
{
}

void CSceneManager::SwitchScene(const TString& strFileName)
{
    SceneSwithNotify();
#ifdef DEVELOP_VERSION
    uint32_t uStartTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
    BEATS_PRINT("Start switch scene at time %u\n", uStartTimeMS);
#endif
    BEATS_ASSERT(GetSwitchSceneState() == false);
    SetSwitchSceneState(true);
    uint32_t uNewSceneFileId = GetSceneFileId(strFileName);
    if (m_loadedScene.find(uNewSceneFileId) == m_loadedScene.end())
    {
        LoadScene(strFileName);
        BEYONDENGINE_CHECK_HEAP;
    }
    SetCurrentScene(m_loadedScene[uNewSceneFileId]);
    SetSwitchSceneState(false);
#ifdef DEVELOP_VERSION
    uint32_t uEndTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
    BEATS_PRINT("Finish switch scene at time %u, elapsed time :%u\n", uEndTimeMS, uEndTimeMS - uStartTimeMS);
#endif
}

bool CSceneManager::SwitchSceneAsync(const TString& strFileName, bool UnloadBeforeLoad)
{
    SceneSwithNotify();
    BEATS_ASSERT(!strFileName.empty(), "File name can't be empty.");
    BEATS_ASSERT(m_bSwitchingSceneState == false);
    uint32_t uFileId = GetSceneFileId(strFileName);
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    m_pSwitchSceneTask->SetUnloadBeforeLoad(UnloadBeforeLoad);
    m_pSwitchSceneTask->SetTargetSceneFileId(uFileId);
    CTaskManager::GetInstance()->AddTask(m_pSwitchSceneTask, true);
    return true;
}

uint32_t CSceneManager::RegisterSwitchSceneCallBack(std::function<void()> fn, bool bAutoRemove)
{
    BEATS_ASSERT(fn != nullptr);
    uint32_t uRet = m_uSwitchSceneCallbackIndex;
    m_switchSceneCallbackMap[m_uSwitchSceneCallbackIndex] = fn;
    if (bAutoRemove)
    {
        BEATS_ASSERT(m_autoRemoveList.find(m_uSwitchSceneCallbackIndex) == m_autoRemoveList.end());
        m_autoRemoveList.insert(m_uSwitchSceneCallbackIndex);
    }
    m_uSwitchSceneCallbackIndex++;
    return uRet;
}

void CSceneManager::UnregisterSwitchSceneCallBack(uint32_t uId)
{
    BEATS_ASSERT(m_switchSceneCallbackMap.find(uId) != m_switchSceneCallbackMap.end());
    m_switchSceneCallbackMap.erase(uId);
}

uint32_t CSceneManager::GetLoadingProgress() const
{
    return m_pSwitchSceneTask->GetProgress();
}

const std::map< uint32_t, CScene* >& CSceneManager::GetLoadedScene()
{
    return m_loadedScene;
}

bool CSceneManager::RenderScene()
{
    bool bRet = false;
    if (m_bRenderSwitcher)
    {
        if (m_pCurrentScene)
        {
            m_pCurrentScene->Render();
            bRet = true;
        }
    }
    return bRet;
}

void CSceneManager::UpdateScene(float dtt)
{
    if (GetSwitchSceneState())
    {
        if (m_pSwitchSceneTask->GetProgress() == 100)
        {
            //HACK: some scene may be loaded when load new scene, so we call on enter here to avoid delay.
            if (m_bAutoTriggerOnEnter)
            {
                TriggerOnEnterAction();
            }
            m_pSwitchSceneTask->Reset();
        }
    }
    else
    {
        if (m_bAutoTriggerOnEnter)
        {
            TriggerOnEnterAction();
        }
    }

    if (m_bUpdateSwitcher)
    {
        if (m_pCurrentScene && m_pCurrentScene->IsLoaded() && m_pCurrentScene->IsActive())
        {
            m_pCurrentScene->Update(dtt);
        }
#ifdef EDITOR_MODE
        else if (!GetSwitchSceneState())
        {
            const std::map<uint32_t, CComponentProxy*>& componentsInScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
            for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
            {
                CComponentProxy* pProxy = iter->second;
                if (pProxy->GetHostComponent() &&
                    pProxy->GetBeConnectedDependencyLines()->size() == 0)
                {
                    CNode* pNode = dynamic_cast<CNode*>(pProxy->GetHostComponent());
                    if (pNode)
                    {
                        if (pNode->GetParentNode() == NULL && dynamic_cast<CParticleEmitter*>(pNode) == nullptr)
                        {
                            pNode->Update(dtt);
                        }
                    }
                }
            }
        }
#endif
    }
}

uint32_t CSceneManager::GetSceneFileId(const TString& strFileName)
{
    uint32_t uRet = 0xFFFFFFFF;
    if (strFileName.length() > 0)
    {
        CComponentProject* pProject = CEngineCenter::GetInstance()->GetComponentManager()->GetProject();
        TString strFullFileName = strFileName;
#ifdef EDITOR_MODE
        const std::vector<TString>* pFileList = pProject->GetFileList();
        for (size_t i = 0; i < pFileList->size(); ++i)
        {
            const TString& strFilePath = pFileList->at(i);
            int pos = strFilePath.rfind(strFileName);
            if (pos != -1 && strFilePath.length() - pos == strFileName.length())
            {
                strFullFileName = pFileList->at(i);
                break;
            }
        }
#endif
        BEATS_ASSERT(!strFullFileName.empty());
        strFullFileName = CStringHelper::GetInstance()->ToLower(strFullFileName);
        uRet = pProject->GetComponentFileId(strFullFileName);
    }
    return uRet;
}

void CSceneManager::SetAutoTriggerOnEnterFlag(bool bAuto)
{
    m_bAutoTriggerOnEnter = bAuto;
}

bool CSceneManager::GetAutoTriggerOnEnterFlag() const
{
    return m_bAutoTriggerOnEnter;
}

void CSceneManager::SetRenderSwitcher(bool bSwitcher)
{
    m_bRenderSwitcher = bSwitcher;
}

void CSceneManager::SetUpdateSwitcher(bool bSwitcher)
{
    m_bUpdateSwitcher = bSwitcher;
}

bool CSceneManager::GetRenderSwitcher() const
{
    return m_bRenderSwitcher;
}

bool CSceneManager::GetUpdateSwitcher() const
{
    return m_bUpdateSwitcher;
}

void CSceneManager::SetSwitchSceneState(bool bIsSwitching)
{
    BEATS_ASSERT(!bIsSwitching || !m_bSwitchingSceneState);
    m_bSwitchingSceneState = bIsSwitching;
}

bool CSceneManager::GetSwitchSceneState() const
{
    return m_bSwitchingSceneState;
}

void CSceneManager::LoadScene(const TString& strFileName)
{
    uint32_t uNewSceneFileId = GetSceneFileId(strFileName);
#ifdef _DEBUG
    if (m_pCurrentScene != NULL)
    {
        uint32_t uCurrentSceneFileId = CEngineCenter::GetInstance()->GetComponentManager()->GetProject()->QueryFileId(m_pCurrentScene->GetId(), true);
        BEATS_ASSERT(uCurrentSceneFileId != 0xFFFFFFFF);
        BEATS_ASSERT(uNewSceneFileId != 0xFFFFFFFF);
        BEATS_ASSERT(uCurrentSceneFileId != uNewSceneFileId, _T("Switch scene failed! Trying to switch to the same scene!"));
    }
#endif
    if (m_pCurrentScene != NULL)
    {
        SetCurrentScene(NULL); // Manual call this to trigger OnLeave before any components un-initialize.
    }
#ifdef EDITOR_MODE
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    CComponentProxyManager::GetInstance()->OpenFile(pProject->GetComponentFileName(uNewSceneFileId).c_str());
#else
    CComponentInstanceManager::GetInstance()->SwitchFile(uNewSceneFileId);
#endif
}

void CSceneManager::TriggerOnEnterAction()
{
    if (m_bUpdateSwitcher)
    {
        if (m_onEnterScene.size() > 0)
        {
            BEYONDENGINE_CHECK_HEAP;
            for (uint32_t i = 0; i < m_onEnterScene.size(); ++i)
            {
                if (m_onEnterScene[i] == GetCurrentScene())
                {
                    // We can safely call OnEnter here because all components loaded are initialized.
                    m_onEnterScene[i]->OnEnter();
                    break;
                }
            }
            m_onEnterScene.clear();
            // Because OnEnter may cause much time, to avoid delta time too much, we manually clear the delta time.
            CApplication::GetInstance()->GetUpdateTimeMeter().Tick();
            BEYONDENGINE_CHECK_HEAP;
        }
    }
}

void CSceneManager::TriggerSwitchSceneAction()
{
    for (auto iter = m_switchSceneCallbackMap.begin(); iter != m_switchSceneCallbackMap.end(); )
    {
        (iter->second)();
        auto subIter = m_autoRemoveList.find(iter->first);
        if (subIter != m_autoRemoveList.end())
        {
            m_autoRemoveList.erase(subIter);
            iter = m_switchSceneCallbackMap.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void CSceneManager::Reset()
{
    if (m_bSwitchingSceneState)
    {
        m_pSwitchSceneTask->Reset();
    }
    m_bRenderSwitcher = true;
    m_bUpdateSwitcher = true;
    m_bAutoTriggerOnEnter = true;
    m_uSwitchSceneCallbackIndex = 0;
    m_pCurrentScene = nullptr;
    m_loadedScene.clear();
    m_switchSceneCallbackMap.clear();
    m_autoRemoveList.clear();
    m_onEnterScene.clear();
}