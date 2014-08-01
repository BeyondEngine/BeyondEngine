#ifndef BEYOND_ENGINE_TASK_TASKBASE_H__INCLUDE
#define BEYOND_ENGINE_TASK_TASKBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

class CTaskBase : public CComponentInstance
{
    DECLARE_REFLECT_GUID_ABSTRACT(CTaskBase, 0x9A7B0041, CComponentInstance)

public:
    CTaskBase();
    virtual ~CTaskBase();

    virtual size_t GetProgress() = 0;
    virtual void Execute(float ddt) = 0;
};

#endif