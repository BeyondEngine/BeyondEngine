#include "stdafx.h"
#include "LaunchTask.h"
#include "Task/TaskManager.h"
#include "Task/TaskBase.h"

CLaunchTask::CLaunchTask()
{

}

CLaunchTask::~CLaunchTask()
{

}

void CLaunchTask::Initialize()
{
    super::Initialize();
    for (uint32_t i = 0; i < m_taskList.size(); ++i)
    {
        ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_taskList[i] != nullptr)
            m_taskShareList.push_back(m_taskList[i]);
            UnregisterReflectComponent(m_taskList[i]); // So the m_taskShareList will take the responsibility to delete the task property.
        ASSUME_VARIABLE_IN_EDITOR_END
    }
}
#ifdef EDITOR_MODE
bool CLaunchTask::OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pNewValueToBeSet);
    if (!bRet)
    {
        if (pVariableAddr == &m_taskList)
        {
            DeserializeVariable(m_taskList, pNewValueToBeSet, this);
            std::vector<CTaskBase*> bak = m_taskList;
            for (auto iter = m_taskShareList.begin(); iter != m_taskShareList.end();)
            {
                auto subIter = std::find(bak.begin(), bak.end(), iter->Get());
                if (subIter == m_taskList.end())
                {
                    iter = m_taskShareList.erase(iter);
                }
                else
                {
                    bak.erase(subIter);
                    ++iter;
                }
                for (size_t i = 0; i < bak.size(); ++i)
                {
                    m_taskShareList.push_back(bak[i]);
                    UnregisterReflectComponent(bak[i]); // So the m_taskShareList will take the responsibility to delete the task property.
                }
            }
            bRet = true;
        }
    }
    return bRet;
}
#endif
bool CLaunchTask::ExecuteImp(SActionContext* /*pContext*/)
{
    BEATS_ASSERT(m_taskShareList.size() > 0, _T("No task to launch!"));
    for (uint32_t i = 0; i < m_taskShareList.size(); ++i)
    {
        CTaskManager::GetInstance()->AddTask(m_taskShareList[i], false);
    }
    return true;
}

void CLaunchTask::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_taskList, true, 0xFFFFFFFF, _T("要添加的任务"), NULL, NULL, NULL);
}

const std::vector< SharePtr<CTaskBase> >& CLaunchTask::GetTaskList() const
{
    return m_taskShareList;
}