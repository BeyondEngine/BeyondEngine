#include "stdafx.h"
#include "TaskManager.h"
#include "TaskBase.h"
#include "Scene/SceneManager.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#include "ExtractAndroidAssets.h"
#endif

CTaskManager* CTaskManager::m_pInstance = NULL;

CTaskManager::CTaskManager()
    : m_pCurrentAsyncTask(NULL)
{
    BEATS_ASSERT(!CApplication::GetInstance()->IsDestructing(), "Should not create singleton when exit the program!");
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    CExtractAndroidAssets forceExportSymbol;
#endif
}

CTaskManager::~CTaskManager()
{
#ifndef DISABLE_MULTI_THREAD
    m_workUpdateNotify.notify_all();
    m_workThread.join();
#endif
}

void CTaskManager::Initialize()
{
    BEATS_PRINT("Start Initialize Task Manager!\n");
#ifndef DISABLE_MULTI_THREAD
    m_workThread = std::thread(ExecuteAsyncTask);
#endif
#ifdef _DEBUG
    std::stringstream strm;
    strm << m_workThread.get_id();
    BEATS_PRINT("Async task thread launched with id %s\n", strm.str().c_str());
#endif
}

bool CTaskManager::AddTask(const SharePtr<CTaskBase>& pTask, bool bAsync)
{
    pTask->SetAddToMgrTimeStamp(CApplication::GetInstance()->GetFrameTimeMS());
    if (bAsync)
    {
#ifdef DISABLE_MULTI_THREAD
        pTask->Execute(0.016f);
#else
        BEATS_ASSERT(pTask->GetProgress() < 100, _T("Can't add a task which is finished!"));
        m_workMutex.lock();
        m_asyncTaskList.push(pTask);
        m_workMutex.unlock();
        m_workUpdateNotify.notify_all();
#endif
    }
    else
    {
        if (pTask->IsBindToScene())
        {
            CScene* pCurrScene = CSceneManager::GetInstance()->GetCurrentScene();
            pTask->SetOwnerScene(pCurrScene);
        }
        if (pTask->IsImmediateCall())
        {
            BEATS_ASSERT(pTask->GetDelayExecuteTime() == 0);
            pTask->Execute(0);
        }
        m_syncTaskList.push_back(pTask);
    }
    return true;
}

const std::queue<SharePtr<CTaskBase>>& CTaskManager::GetAsyncTaskList() const
{
    return m_asyncTaskList;
}

const std::vector<SharePtr<CTaskBase>>& CTaskManager::GetSyncTaskList() const
{
    return m_syncTaskList;
}

const SharePtr<CTaskBase>& CTaskManager::GetCurrentAsyncTask() const
{
    return m_pCurrentAsyncTask;
}

void CTaskManager::ClearAllAsyncTask()
{
    std::unique_lock<std::mutex> locker(m_workMutex);
    if (m_asyncTaskList.size() > 0)
    {
        std::queue<SharePtr<CTaskBase>> emptyQueue;
        m_asyncTaskList.swap(emptyQueue);
    }
}

bool CTaskManager::IsAllAyncTaskFinished() const
{
    return m_pCurrentAsyncTask == nullptr && m_asyncTaskList.size() == 0 && m_delayInitializeComponents.size() == 0 && m_delayDeleteComponents.size() == 0 && m_bFlushDelayInitFlag == false;
}

void CTaskManager::ExecuteAsyncTask()
{
    CTaskManager* pTaskManager = CTaskManager::GetInstance();
    std::queue<SharePtr<CTaskBase>>& taskList = pTaskManager->m_asyncTaskList;
    std::mutex& workMutex = pTaskManager->m_workMutex;
    BEATS_PRINT("Start to enter in work thread's dead loop!\n");
    while (true)
    {
        // Manage the locker's scope.
        {
            std::unique_lock<std::mutex> locker(workMutex);
            pTaskManager->m_workUpdateNotify.wait(locker, [pTaskManager]{return (!pTaskManager->m_asyncTaskList.empty()) || CApplication::GetInstance()->IsDestructing(); });
            if (CApplication::GetInstance()->IsDestructing())
            {
                break;
            }
#ifdef DISABLE_MULTI_THREAD
            BEATS_ASSERT(false, "Never reach here when multi-thread is disabled!");
#endif
            pTaskManager->m_pCurrentAsyncTask = taskList.front();
            BEATS_ASSERT(pTaskManager->m_pCurrentAsyncTask->GetDelayExecuteTime() == 0, "Async task doesn't support delay time!");
            BEATS_ASSERT(taskList.size() > 0);
            taskList.pop();
        }
        BEATS_PRINT("Start execute async task!\n");
        pTaskManager->m_pCurrentAsyncTask->Execute(0.0f);
        pTaskManager->FlushDelayInitializeComponent();
        pTaskManager->m_pCurrentAsyncTask = NULL;
        BEATS_PRINT("End execute async task!\n");
    }
#ifdef _DEBUG
    std::stringstream strm;
    strm << std::this_thread::get_id();
    BEATS_PRINT("Async task thread exited with id %s\n", strm.str().c_str());
#endif

}

void CTaskManager::ExecuteSyncTask(float ddt)
{
    //To avoid change the m_syncTaskList in the callback , we iterate a copy.
    std::vector<SharePtr<CTaskBase>> taskListbak = m_syncTaskList;
    uint32_t uFrameTime = CApplication::GetInstance()->GetFrameTimeMS();
    for (auto iter = taskListbak.begin(); iter != taskListbak.end(); ++iter)
    {
        SharePtr<CTaskBase> pTask = *iter;
        BEATS_ASSERT(pTask->GetAddToMgrTimeStamp() != 0);
        if (pTask->GetDelayExecuteTime() == 0 || pTask->GetDelayExecuteTime() <= uFrameTime - pTask->GetAddToMgrTimeStamp())
        {
            uint32_t uProgress = (*iter)->GetProgress();
            BEATS_ASSERT(uProgress <= 100);
            if (uProgress < 100)
            {
                (*iter)->Execute(ddt);
                uProgress = (*iter)->GetProgress();
                BEATS_ASSERT(uProgress <= 100, "task %s guid %d progress should <= 100, current progress is %d", (*iter)->GetClassStr(), (*iter)->GetGuid(), uProgress);
            }
            if (uProgress >= 100)
            {
                bool bFind = false;
                for (auto subIter = m_syncTaskList.begin(); subIter != m_syncTaskList.end(); ++subIter)
                {
                    if (subIter->Get() == iter->Get())
                    {
                        bFind = true;
                        m_syncTaskList.erase(subIter);
                        break;
                    }
                }
                BEATS_ASSERT(bFind, "Try to find a sync task in list, but failed!");
                (*iter)->Reset();
            }
        }
    }
}

void CTaskManager::Update(float dt)
{
    if (m_bFlushDelayInitFlag)
    {
        std::unique_lock<std::mutex> locker(m_waitDelayInitMutex);
        if (!CApplication::GetInstance()->IsDestructing())
        {
            for (uint32_t i = 0; i < m_delayInitializeComponents.size(); ++i)
            {
                BEATS_ASSERT(!m_delayInitializeComponents[i]->IsInitialized());
                m_delayInitializeComponents[i]->Initialize();
                BEATS_ASSERT(m_delayInitializeComponents[i]->IsInitialized());
            }
        }
        m_delayInitializeComponents.clear();
        for (uint32_t i = 0; i < m_delayDeleteComponents.size(); ++i)
        {
            if (m_delayDeleteComponents[i]->IsInitialized())
            {
                m_delayDeleteComponents[i]->Uninitialize();
            }
            BEATS_SAFE_DELETE(m_delayDeleteComponents[i]);
        }
        m_delayDeleteComponents.clear();
        m_bFlushDelayInitFlag = false;
        m_waitDelayInitCondition.notify_all();
    }
    ExecuteSyncTask(dt);
}

void CTaskManager::CancelSceneTask(CScene* pScene)
{
    BEATS_ASSERT(pScene != NULL);
    for (auto iter = m_syncTaskList.begin(); iter != m_syncTaskList.end();)
    {
        if ((*iter)->GetOwnerScene() == pScene)
        {
            (*iter)->Reset();
            iter = m_syncTaskList.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void CTaskManager::Reset()
{
    m_pCurrentAsyncTask = nullptr;
    m_delayInitializeComponents.clear();
    m_syncTaskList.clear();
    {
        ClearAllAsyncTask();
    }
}

void CTaskManager::AddDelayInitializeComponent(CComponentInstance* pComponent)
{
#ifdef DISABLE_MULTI_THREAD
    pComponent->Initialize();
#else
    BEATS_ASSERT(pComponent->IsLoaded());
    BEATS_ASSERT(!pComponent->IsInitialized());
    BEATS_ASSERT(std::this_thread::get_id() != CEngineCenter::GetInstance()->GetMainThreadId());
    m_delayInitializeComponents.push_back(pComponent);
#endif
}

void CTaskManager::AddDelayDeleteComponent(CComponentInstance* pComponent)
{
#ifdef DISABLE_MULTI_THREAD
    BEATS_SAFE_DELETE_COMPONENT(pComponent);
#else
    BEATS_ASSERT(std::this_thread::get_id() != CEngineCenter::GetInstance()->GetMainThreadId());
    if (pComponent->IsLoaded())
    {
        pComponent->Unload();
    }
    m_delayDeleteComponents.push_back(pComponent);
#endif
}

void CTaskManager::FlushDelayInitializeComponent()
{
#ifndef DISABLE_MULTI_THREAD
    BEATS_ASSERT(std::this_thread::get_id() != CEngineCenter::GetInstance()->GetMainThreadId(), "Can't call FlushDelayInitializeComponent in main thread, or it will blocked!");
    if (m_delayInitializeComponents.size() > 0 || m_delayDeleteComponents.size() > 0)
    {
        std::unique_lock<std::mutex> locker(m_waitDelayInitMutex);
        m_bFlushDelayInitFlag = true;
        m_waitDelayInitCondition.wait(locker);
    }
#endif
}

std::thread::id CTaskManager::GetTaskThreadId() const
{
    return m_workThread.get_id();
}

bool CTaskManager::GetFlushDelayFlag() const
{
    return m_bFlushDelayInitFlag;
}