#include "stdafx.h"
#include "LaunchTask.h"
#include "Task\TaskManager.h"
#include "Task\TaskBase.h"

CLaunchTask::CLaunchTask()
{

}

CLaunchTask::~CLaunchTask()
{

}

void CLaunchTask::Initialize()
{
    super::Initialize();
    for (size_t i = 0; i < m_taskList.size(); ++i)
    {
        m_taskShareList.push_back(m_taskList.at(i));
    }
}

bool CLaunchTask::ExecuteImp(SActionContext* /*pContext*/)
{
    BEATS_ASSERT(m_taskShareList.size() > 0, _T("No task to launch!"));
    for (size_t i = 0; i < m_taskShareList.size(); ++i)
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