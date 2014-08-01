#include "stdafx.h"
#include "AIState.h"
#include "Action/ActionBase.h"

CAIState::CAIState()
    : m_pEnterAction(NULL)
    , m_pUpdateAction(NULL)
    , m_pLeaveAction(NULL)
    , m_pOwnerScheme(NULL)
{

}

CAIState::~CAIState()
{

}

void CAIState::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_DEPENDENCY(serializer, m_pEnterAction, _T("初始动作"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pUpdateAction, _T("更新动作"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pLeaveAction, _T("离开动作"), eDT_Weak);
}

void CAIState::Enter(SActionContext* pActionContext)
{
    if (m_pEnterAction != NULL)
    {
        m_pEnterAction->Execute(pActionContext);
    }
}

void CAIState::Update(SActionContext* pActionContext)
{
    if (m_pUpdateAction != NULL)
    {
        m_pUpdateAction->Execute(pActionContext);
    }
}

void CAIState::Leave(SActionContext* pActionContext)
{
    if (m_pLeaveAction != NULL)
    {
        m_pLeaveAction->Execute(pActionContext);
    }
}

void CAIState::SetScheme(CAIScheme* pAIScheme)
{
    BEATS_ASSERT(m_pOwnerScheme == NULL);
    m_pOwnerScheme = pAIScheme;
}

CAIScheme* CAIState::GetScheme() const
{
    return m_pOwnerScheme;
}