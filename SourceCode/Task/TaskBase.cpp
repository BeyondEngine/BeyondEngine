#include "stdafx.h"
#include "TaskBase.h"

CTaskBase::CTaskBase()
: m_pSceneOwner(NULL)
, m_bBindToCurrScene(false)
{

}

CTaskBase::~CTaskBase()
{

}

void CTaskBase::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_uDelayExecuteMS, true, 0xFFFFFFFF, _T("延迟调用时间（毫秒）"), NULL, _T("当任务开始的时候，延迟一段时间再执行"), NULL);
    DECLARE_PROPERTY(serializer, m_bBindToCurrScene, true, 0xFFFFFFFF, _T("绑定到当前场景"), NULL, _T("如果为真，那么离开当前场景时，该任务中断"), NULL);
    DECLARE_PROPERTY(serializer, m_bImmediateCall, true, 0xFFFFFFFF, _T("立即调用"), NULL, _T("如果为真，该任务被添加到管理器的时候立刻调用"), "DefaultValue:true, VisibleWhen:m_uDelayExecuteMS != 0");
}

void CTaskBase::Stop()
{
    m_bStopFlag = true;
}

void CTaskBase::Reset()
{
    m_bStopFlag = false;
    m_uAddToMgrTimeStamp = 0;
    if (m_pFinishedCallback != nullptr)
    {
        m_pFinishedCallback();
        m_pFinishedCallback = nullptr;
    }
}

void CTaskBase::SetDelayExecuteTimeMS(uint32_t uDelayTime)
{
    m_uDelayExecuteMS = uDelayTime;
}

uint32_t CTaskBase::GetDelayExecuteTime() const
{
    return m_uDelayExecuteMS;
}

void CTaskBase::SetAddToMgrTimeStamp(uint32_t uTimeStamp)
{
    m_uAddToMgrTimeStamp = uTimeStamp;
}

uint32_t CTaskBase::GetAddToMgrTimeStamp() const
{
    return m_uAddToMgrTimeStamp;
}

CScene* CTaskBase::GetOwnerScene() const
{
    return m_pSceneOwner;
}

void CTaskBase::SetOwnerScene(CScene* pScene)
{
    m_pSceneOwner = pScene;
}

bool CTaskBase::IsBindToScene() const
{
    return m_bBindToCurrScene;
}

bool CTaskBase::IsImmediateCall() const
{
    return m_bImmediateCall;
}

void CTaskBase::SetBindToScene(bool bBindToScene)
{
    m_bBindToCurrScene = bBindToScene;
}

void CTaskBase::SetFinishedCallback(std::function<void()> pFunc)
{
    m_pFinishedCallback = pFunc;
}

std::function<void()> CTaskBase::GetFinishedCallback() const
{
    return m_pFinishedCallback;
}
