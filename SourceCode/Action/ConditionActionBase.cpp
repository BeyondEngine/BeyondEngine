#include "stdafx.h"
#include "ConditionActionBase.h"

CConditionActionBase::CConditionActionBase()
    : m_pSuccessAction(NULL)
    , m_pFailedAction(NULL)
{

}

CConditionActionBase::~CConditionActionBase()
{

}

void CConditionActionBase::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_DEPENDENCY(serializer, m_pSuccessAction, _T("³É¹¦"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pFailedAction, _T("Ê§°Ü"), eDT_Weak);
}

bool CConditionActionBase::ExecuteImp(SActionContext* pContext)
{
    if (Exam(pContext))
    {
        if (m_pSuccessAction != NULL)
        {
            m_pSuccessAction->Execute(pContext);
        }
    }
    else
    {
        if (m_pFailedAction != NULL)
        {
            m_pFailedAction->Execute(pContext);
        }
    }
    return true;
}
