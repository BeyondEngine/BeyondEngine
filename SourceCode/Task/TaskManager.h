#ifndef BEYOND_ENGINE_TASK_TASKMANAGER_H__INCLUDE
#define BEYOND_ENGINE_TASK_TASKMANAGER_H__INCLUDE

class CTaskBase;
class CScene;
class CTaskManager
{
    BEATS_DECLARE_SINGLETON(CTaskManager)
public:
    void Initialize();
    bool AddTask(const SharePtr<CTaskBase>& pTask, bool bAsync);
    const std::queue<SharePtr<CTaskBase>>& GetAsyncTaskList() const;
    const std::vector<SharePtr<CTaskBase>>& GetSyncTaskList() const;
    const SharePtr<CTaskBase>& GetCurrentAsyncTask() const;
    void ClearAllAsyncTask();
    bool IsAllAyncTaskFinished() const;

    void Update(float dt);
    void CancelSceneTask(CScene* pScene);
    void Reset();

    void AddDelayInitializeComponent(CComponentInstance* pComponent);
    void AddDelayDeleteComponent(CComponentInstance* pComponent);
    void FlushDelayInitializeComponent();
    static void ExecuteAsyncTask();
    std::thread::id GetTaskThreadId() const;
    bool GetFlushDelayFlag() const;

private:
    void ExecuteSyncTask(float ddt);

private:
    //Async
    bool m_bFlushDelayInitFlag = false;
    SharePtr<CTaskBase> m_pCurrentAsyncTask;
    std::thread m_workThread;
    std::mutex m_workMutex;
    std::condition_variable m_workUpdateNotify;
    std::queue<SharePtr<CTaskBase>> m_asyncTaskList;
    std::mutex m_waitDelayInitMutex;
    std::condition_variable m_waitDelayInitCondition;
    std::vector<CComponentInstance*> m_delayInitializeComponents;
    std::vector<CComponentInstance*> m_delayDeleteComponents;
    //sync
    std::vector<SharePtr<CTaskBase>> m_syncTaskList;
};

#endif