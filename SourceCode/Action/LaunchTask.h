#ifndef BEYOND_ENGINE_ACTION_LAUNCHTASK_H__INCLUDE
#define BEYOND_ENGINE_ACTION_LAUNCHTASK_H__INCLUDE

#include "ActionBase.h"
class CTaskBase;

class CLaunchTask : public CActionBase
{
    DECLARE_REFLECT_GUID( CLaunchTask, 0x479A1CC4, CActionBase )

public:
    CLaunchTask();
    virtual ~CLaunchTask();
    virtual void Initialize() override;

    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;

private:
    std::vector<CTaskBase*> m_taskList;
    // TODO: because the task manager will delete the task, so we hold a reference.
    std::vector<SharePtr<CTaskBase>> m_taskShareList;
};

#endif