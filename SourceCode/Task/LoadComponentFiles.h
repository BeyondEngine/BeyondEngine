#ifndef BEYOND_ENGINE_TASK_LOADCOMPONENTFILES_H__INCLUDE
#define BEYOND_ENGINE_TASK_LOADCOMPONENTFILES_H__INCLUDE

#include "TaskBase.h"

class CLoadComponentFiles : public CTaskBase
{
public:
    CLoadComponentFiles();
    virtual ~CLoadComponentFiles();

    std::vector<CComponentBase*>& GetLoadedComponents();
    std::vector<size_t>& GetFiles();

    virtual size_t GetProgress() override;
    virtual void Execute(float ddt) override;

private:
    size_t m_uCurrentLoadingComponentIndex;
    std::vector<size_t> m_componentFiles;
    std::vector<CComponentBase*> m_loadedComponents;
};
#endif