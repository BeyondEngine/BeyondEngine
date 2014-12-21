#include "stdafx.h"
#include "LamdaTask.h"

CLamdaTask::CLamdaTask()
: m_pExecuteFunc(nullptr)
, m_pProgressFunc(nullptr)
{

}

CLamdaTask::~CLamdaTask()
{

}

void CLamdaTask::BuildLamdaTask(std::function<void(float)> executeFunc, std::function<uint32_t()> progressFunc, std::function<void()> resetFunc)
{
    m_pExecuteFunc = executeFunc;
    m_pProgressFunc = progressFunc;
    SetFinishedCallback(resetFunc);
    BEATS_ASSERT(m_pExecuteFunc != nullptr);
}

uint32_t CLamdaTask::GetProgress()
{
    uint32_t uRet = m_uDefaultProgress;
    if (m_pProgressFunc != nullptr)
    {
        uRet = m_pProgressFunc();
    }
    return uRet;
}

void CLamdaTask::Execute(float ddt)
{
    m_pExecuteFunc(ddt);
    if (m_pProgressFunc == nullptr)
    {
        m_uDefaultProgress = 100;
    }
}

void CLamdaTask::Reset()
{
    m_uDefaultProgress = 0;
    super::Reset();
}

void CLamdaTask::SetDefaultProgress(uint32_t uProgress)
{
    m_uDefaultProgress = uProgress;
}