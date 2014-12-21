#include "stdafx.h"
#include "SwitchSceneTask.h"
#include "LamdaTask.h"
#include "Scene/SceneManager.h"
#include "Task/TaskManager.h"

CSwitchSceneTask::CSwitchSceneTask()
: m_bUnloadBeforeLoad(true)
, m_uTargetSceneFileId(0xFFFFFFFF)
, m_fProgress(0)
{
}

CSwitchSceneTask::~CSwitchSceneTask()
{

}

void CSwitchSceneTask::SetUnloadBeforeLoad(bool bFlag)
{
    m_bUnloadBeforeLoad = bFlag;
}

void CSwitchSceneTask::SetTargetSceneFileId(uint32_t uId)
{
    m_uTargetSceneFileId = uId;
}

uint32_t CSwitchSceneTask::GetTargetSceneFileId() const
{
    return m_uTargetSceneFileId;
}

uint32_t CSwitchSceneTask::GetProgress()
{
    return (uint32_t)m_fProgress;
}

void CSwitchSceneTask::Execute(float /*ddt*/)
{
#ifdef DEVELOP_VERSION
    uint32_t uStartTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
    BEATS_PRINT("Start switch scene async at time %u\n", uStartTimeMS);
#endif
    BEATS_ASSERT(m_fProgress == 0);
    BEATS_ASSERT(CSceneManager::GetInstance()->GetSwitchSceneState() == false);
    CSceneManager::GetInstance()->SetSwitchSceneState(true);
    BEATS_ASSERT(m_uTargetSceneFileId != 0xFFFFFFFF, _T("target scene file id can't be 0xFFFFFFFF"));
    std::vector<uint32_t> loadFiles;
    std::vector<uint32_t> unloadFiles;
    const std::map< uint32_t, CScene* >& alreadyLoadScene = CSceneManager::GetInstance()->GetLoadedScene();
    auto sceneIter = alreadyLoadScene.find(m_uTargetSceneFileId);
    if (sceneIter == alreadyLoadScene.end())
    {
        CComponentManagerBase* pComponentManager = CEngineCenter::GetInstance()->GetComponentManager();
        bool bNewAddFile = false;
        pComponentManager->CalcSwitchFile(m_uTargetSceneFileId, loadFiles, unloadFiles, bNewAddFile);
        if (m_bUnloadBeforeLoad)
        {
            CSceneManager::GetInstance()->SetRenderSwitcher(false);
            CSceneManager::GetInstance()->SetUpdateSwitcher(false);
            if (std::this_thread::get_id() != CEngineCenter::GetInstance()->GetMainThreadId())
            {
                // wait one frame of main thread to avoid the scene is rendering in main thread.
                uint32_t uFrameCounter = CEngineCenter::GetInstance()->GetFrameCounter();
                while (uFrameCounter == CEngineCenter::GetInstance()->GetFrameCounter())
                {
                    BEYONDENGINE_SLEEP(0);
                }
            }
            UnloadLastScene(unloadFiles);
            CSceneManager::GetInstance()->SetUpdateSwitcher(true);
        }
        LoadNewScene(loadFiles);
        if (!m_bUnloadBeforeLoad)
        {
            UnloadLastScene(unloadFiles);
        }
        BEATS_ASSERT(m_fProgress < 100);
    }
    else
    {
        // Because we has rebuilt the scene, so we need to call the same scene's onEnter
        // set current scene to null then restore, the onEnter will be trigger.
        CSceneManager::GetInstance()->SetCurrentScene(nullptr); 
        CSceneManager::GetInstance()->SetCurrentScene(sceneIter->second);
        CSceneManager::GetInstance()->SetUpdateSwitcher(true);
    }
    if (!m_bStopFlag)
    {
        HandleLaunchBattle();
    }
    m_fProgress = 100;
    BEYONDENGINE_CHECK_HEAP;
#ifdef DEVELOP_VERSION
    uint32_t uEndTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
    BEATS_PRINT("Finish switch scene async at time %u, elapsed time :%u\n", uEndTimeMS, uEndTimeMS - uStartTimeMS);
#endif
}

void CSwitchSceneTask::Reset()
{
    m_uTargetSceneFileId = 0xFFFFFFFF;
    m_fProgress = 0;
    m_components.clear();
    BEATS_ASSERT(CSceneManager::GetInstance()->GetSwitchSceneState());
    CSceneManager::GetInstance()->SetSwitchSceneState(false);
    super::Reset();
    BEYONDENGINE_CHECK_HEAP;
}

void CSwitchSceneTask::UnloadLastScene(const std::vector<uint32_t>& unloadFileList)
{
    BEATS_ASSERT(m_components.size() == 0);
    CComponentManagerBase* pComponentManager = CEngineCenter::GetInstance()->GetComponentManager();
    // 1. Unload all files
    for (size_t i = 0; i < unloadFileList.size(); ++i)
    {
        if (!m_bStopFlag)
        {
            pComponentManager->UnloadFile(unloadFileList[i], &m_components);
        }
    }
    if (m_components.size() > 0)
    {
        // 2.  Wait for the main thread to call their un-initialize
        SharePtr<CLamdaTask> deleteTask = new CLamdaTask;
        std::function<void(float)> pDeleteExecuteFunc = [this](float)
        {
            for (size_t i = 0; i < m_components.size(); ++i)
            {
#ifdef EDITOR_MODE
                CComponentProxy* pProxy = down_cast<CComponentProxy*>(m_components[i]);
                CComponentInstance* pInstance = pProxy->GetHostComponent();
                pInstance->Uninitialize();
#else
                m_components[i]->Uninitialize();
#endif
        }
            // 3. After all components are uninitialized, we delete them.
            for (size_t i = 0; i < m_components.size(); ++i)
            {
#ifdef EDITOR_MODE
                CComponentProxy* pProxy = down_cast<CComponentProxy*>(m_components[i]);
                CComponentInstance* pInstance = pProxy->GetHostComponent();
                BEATS_SAFE_DELETE(pInstance);
#else
                BEATS_SAFE_DELETE(m_components[i]);
#endif
    }
            m_components.clear();
};
        std::function<uint32_t()> pDeleteProgressFunc = [this]()
        {
            return m_components.size() == 0 ? 100 : 0;
        };
        std::function<void()> pDeleteRestFunc = [this]()
        {
            std::unique_lock<std::mutex> locker(m_waitMutex);
            m_waitCondition.notify_all();
        };
        deleteTask->BuildLamdaTask(pDeleteExecuteFunc, pDeleteProgressFunc, pDeleteRestFunc);
#ifdef DISABLE_MULTI_THREAD
        deleteTask->Execute(0);
#else
        std::unique_lock<std::mutex> locker(m_waitMutex);
        CTaskManager::GetInstance()->AddTask(deleteTask, false);
        m_waitCondition.wait(locker);
#endif
        BEATS_ASSERT(m_components.size() == 0);
        BEYONDENGINE_CHECK_HEAP;
    }
}

void CSwitchSceneTask::LoadNewScene(const std::vector<uint32_t>& loadFileList)
{
    BEATS_ASSERT(m_components.size() == 0);
    CComponentManagerBase* pComponentManager = CEngineCenter::GetInstance()->GetComponentManager();
    const std::vector<uint32_t>& alreadyLoadedFiles = pComponentManager->GetLoadedFiles();
    // 1. start to load the new components.
    for (size_t i = 0; i < loadFileList.size(); ++i)
    {
        if (!m_bStopFlag && std::find(alreadyLoadedFiles.begin(), alreadyLoadedFiles.end(), loadFileList[i]) == alreadyLoadedFiles.end())
        {
#ifdef _DEBUG
            uint32_t uOriginCount = m_components.size();
#endif
            pComponentManager->LoadFile(loadFileList[i], &m_components);
#ifdef _DEBUG
            for (size_t j = uOriginCount; j < m_components.size(); ++j)
            {
                CComponentBase* pComponent = m_components[j];
                CComponentProxy* pProxy = dynamic_cast<CComponentProxy*>(m_components[j]);
                if (pProxy != nullptr)
                {
                    pComponent = pProxy->GetHostComponent();
                }
                BEATS_ASSERT(pComponent->IsLoaded() && !pComponent->IsInitialized());
            }
#endif
        }
        m_fProgress += 98.f / loadFileList.size();
        BEATS_ASSERT(m_fProgress < 100);
    }
#ifdef EDITOR_MODE
    BEATS_ASSERT(CComponentProxyManager::GetInstance() == pComponentManager);
    CComponentProxyManager::GetInstance()->SetCurrentViewFileId(m_uTargetSceneFileId);
#endif
    //2. Wait for all components to initialize
    bool bInMainThread = std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId();
    for (size_t i = 0; i < m_components.size(); ++i)
    {
        CComponentInstance* pInstance = nullptr;
#ifdef EDITOR_MODE
        pInstance = down_cast<CComponentProxy*>(m_components[i])->GetHostComponent();
#else
        pInstance = down_cast<CComponentInstance*>(m_components[i]);
#endif
#ifdef DISABLE_MULTI_THREAD
        pInstance->Initialize();
#else
        if (bInMainThread)
        {
            pInstance->Initialize();
        }
        else
        {
            CTaskManager::GetInstance()->AddDelayInitializeComponent(pInstance);
        }
#endif
    }
    if (!bInMainThread)
    {
        CTaskManager::GetInstance()->FlushDelayInitializeComponent();
    }

    if (!m_bStopFlag)
    {
        SharePtr<CLamdaTask> setCurrentSceneTask = new CLamdaTask;
        std::function<void(float)> pSetCurrentSceneExecuteFunc = [this](float)
        {
            if (!m_bStopFlag)
            {
                const std::map< uint32_t, CScene* >& loadedScene = CSceneManager::GetInstance()->GetLoadedScene();
                auto sceneIter = loadedScene.find(m_uTargetSceneFileId);
                BEATS_ASSERT(sceneIter != loadedScene.end(), "Can't find scene component in file %d", m_uTargetSceneFileId);
                CSceneManager::GetInstance()->SetCurrentScene(sceneIter->second);
            }
        };
        std::function<void()> pSetCurrentSceneRestFunc = [this]()
        {
            std::unique_lock<std::mutex> locker(m_waitMutex);
            m_waitCondition.notify_all();
        };
        setCurrentSceneTask->BuildLamdaTask(pSetCurrentSceneExecuteFunc, nullptr, pSetCurrentSceneRestFunc);
#ifdef DISABLE_MULTI_THREAD
        setCurrentSceneTask->Execute(0);
#else
        std::unique_lock<std::mutex> locker(m_waitMutex);
        CTaskManager::GetInstance()->AddTask(setCurrentSceneTask, false);
        m_waitCondition.wait(locker);
#endif
    }
    m_components.clear();
    pComponentManager->SetCurrLoadFileId(m_uTargetSceneFileId); // so when we call CalcSwitchFile next time, we will base on the currLoadFile.
    BEYONDENGINE_CHECK_HEAP;
}

void CSwitchSceneTask::HandleLaunchBattle()
{
}
