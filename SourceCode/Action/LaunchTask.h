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
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet) override;
#endif
    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;
    const std::vector< SharePtr<CTaskBase> >& GetTaskList() const;

private:
    std::vector<CTaskBase*> m_taskList;
    // Raw pointer in m_taskList only for reflection.
    std::vector<SharePtr<CTaskBase> > m_taskShareList;
};

#endif