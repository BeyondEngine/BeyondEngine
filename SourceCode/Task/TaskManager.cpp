#include "stdafx.h"
#include "TaskManager.h"
#include "TaskBase.h"

//#define DISABLE_MT

CTaskManager* CTaskManager::m_pInstance = NULL;

CTaskManager::CTaskManager()
    : m_pCurrentAsyncTask(NULL)
{
}

CTaskManager::~CTaskManager()
{
}

void CTaskManager::Initialize()
{
    m_workThread = std::thread(ExecuteAsyncTask);
}

void CTaskManager::Uninitialize()
{
    m_workThread.detach();
}

bool CTaskManager::AddTask(const SharePtr<CTaskBase>& pTask, bool bAsync)
{
    if (bAsync)
    {
#ifdef DISABLE_MT
        pTask->Execute();
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
        m_syncTaskList.push_back(pTask);
    }
    return true;
}

const std::queue<SharePtr<CTaskBase>>& CTaskManager::GetAsyncTaskList() const
{
    return m_asyncTaskList;
}

const SharePtr<CTaskBase>& CTaskManager::GetCurrentAsyncTask() const
{
    return m_pCurrentAsyncTask;
}

void CTaskManager::WaitForAllTask()
{
    std::unique_lock<std::mutex> locker(m_workMutex);
    m_workUpdateNotify.wait(locker, [this]{return m_asyncTaskList.empty() && m_pCurrentAsyncTask == NULL;});
}

void CTaskManager::ExecuteAsyncTask()
{
    CTaskManager* pTaskManager = CTaskManager::GetInstance();
    std::queue<SharePtr<CTaskBase>>& taskList = pTaskManager->m_asyncTaskList;
    std::mutex& workMutex = pTaskManager->m_workMutex;

    while (true)
    {
        // Manage the locker's scope.
        {
            std::unique_lock<std::mutex> locker(workMutex);
            pTaskManager->m_workUpdateNotify.wait(locker, [pTaskManager]{return !pTaskManager->m_asyncTaskList.empty();});
            pTaskManager->m_pCurrentAsyncTask = taskList.front();
            BEATS_ASSERT(taskList.size() > 0);
            taskList.pop();
        }
        pTaskManager->m_pCurrentAsyncTask->Execute(0.0f);
        pTaskManager->m_pCurrentAsyncTask = NULL;
        pTaskManager->m_workUpdateNotify.notify_all();
    }
}

void CTaskManager::ExecuteSyncTask(float ddt)
{
    std::vector<SharePtr<CTaskBase>> newTaskList;
    for (size_t i = 0; i < m_syncTaskList.size(); ++i)
    {
        if (m_syncTaskList[i]->GetProgress() != 100)
        {
            //TODO:HACK: Try to figure out why some times the ddt is very big when switch scene.
            if (ddt > 0.016f)
            {
                ddt = 0.016f;
            }
            m_syncTaskList[i]->Execute(ddt);
            newTaskList.push_back(m_syncTaskList[i]);
        }
    }
    m_syncTaskList.swap(newTaskList);
}

void CTaskManager::Update()
{
    if (m_delayInitializeComponents.size() > 0)
    {
        if (m_delayInitializeMutex.try_lock())
        {
            for (size_t i = 0; i < m_delayInitializeComponents.size(); ++i)
            {
                if (!m_delayInitializeComponents[i]->IsInitialized())
                {
                    m_delayInitializeComponents[i]->Initialize();
                    BEATS_ASSERT(m_delayInitializeComponents[i]->IsInitialized());
                }
            }
            m_delayInitializeComponents.clear();
            m_delayInitializeMutex.unlock();
        }
    }
}

void CTaskManager::AddDelayInitializeComponent(CComponentInstance* pComponent)
{
    m_delayInitializeMutex.lock();
    m_delayInitializeComponents.push_back(pComponent);
    m_delayInitializeMutex.unlock();
}

std::thread::id CTaskManager::GetTaskThreadId() const
{
    return m_workThread.get_id();
}
