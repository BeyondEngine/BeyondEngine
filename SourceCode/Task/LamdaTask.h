#ifndef BEYOND_ENGINE_TASK_LAMDATASK_H__INCLUDE
#define BEYOND_ENGINE_TASK_LAMDATASK_H__INCLUDE

#include "TaskBase.h"
class CLamdaTask : public CTaskBase
{
    typedef CTaskBase super;
public:
    CLamdaTask();
    virtual ~CLamdaTask();

    void BuildLamdaTask(std::function<void(float)> executeFunc, std::function<uint32_t()> progressFunc, std::function<void()> resetFunc);
    virtual uint32_t GetProgress() override;
    virtual void Execute(float ddt) override;
    virtual void Reset() override;
    void SetDefaultProgress(uint32_t uProgress);
private:
    uint32_t m_uDefaultProgress = 0;
    std::function<void(float)> m_pExecuteFunc;
    std::function<uint32_t()> m_pProgressFunc;
};

#endif