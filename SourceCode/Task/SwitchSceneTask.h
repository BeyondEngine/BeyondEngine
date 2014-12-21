#ifndef BEYOND_ENGINE_TASK_SWITCHSCENETASK_H__INCLUDE
#define BEYOND_ENGINE_TASK_SWITCHSCENETASK_H__INCLUDE

#include "TaskBase.h"

class CSwitchSceneTask : public CTaskBase
{
    typedef CTaskBase super;
public:
    CSwitchSceneTask();
    virtual ~CSwitchSceneTask();

    void SetUnloadBeforeLoad(bool bFlag);
    void SetTargetSceneFileId(uint32_t uId);
    uint32_t GetTargetSceneFileId() const;
    virtual uint32_t GetProgress() override;
    virtual void Execute(float ddt) override;
    virtual void Reset() override;

private:
    void UnloadLastScene(const std::vector<uint32_t>& unloadFileList);
    void LoadNewScene(const std::vector<uint32_t>& loadFileList);
    void HandleLaunchBattle();

private:
    bool m_bUnloadBeforeLoad;
    uint32_t m_uTargetSceneFileId;
    float m_fProgress;
    std::mutex m_waitMutex;
    std::condition_variable m_waitCondition;
    std::vector<CComponentBase*> m_components;
};
#endif