#ifndef BEYOND_ENGINE_TASK_TASKMANAGER_H__INCLUDE
#define BEYOND_ENGINE_TASK_TASKMANAGER_H__INCLUDE

class CTaskBase;
#include <condition_variable>
#include <queue>
class CTaskManager
{
    BEATS_DECLARE_SINGLETON(CTaskManager)
public:
    void Initialize();
    void Uninitialize();
    bool AddTask(const SharePtr<CTaskBase>& pTask, bool bAsync);
    const std::queue<SharePtr<CTaskBase>>& GetAsyncTaskList() const;
    const SharePtr<CTaskBase>& GetCurrentAsyncTask() const;
    void WaitForAllTask();
    void ExecuteSyncTask(float ddt);
    void Update();

    void AddDelayInitializeComponent(CComponentInstance* pComponent);
    static void ExecuteAsyncTask();
    std::thread::id GetTaskThreadId() const;

private:
    //Async
    SharePtr<CTaskBase> m_pCurrentAsyncTask;
    std::thread m_workThread;
    std::mutex m_workMutex;
    std::condition_variable m_workUpdateNotify;
    std::queue<SharePtr<CTaskBase>> m_asyncTaskList;
    std::mutex m_delayInitializeMutex;
    std::vector<CComponentInstance*> m_delayInitializeComponents;
    //sync
    std::vector<SharePtr<CTaskBase>> m_syncTaskList;
};

#endif