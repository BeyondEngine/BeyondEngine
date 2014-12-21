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
    DECLARE_DEPENDENCY(serializer, m_pSuccessAction, _T("成功"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pFailedAction, _T("失败"), eDT_Weak);
}

bool CConditionActionBase::ExecuteImp(SActionContext* pContext)
{
    std::vector<CNode*> originOperator = pContext->m_operator;
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
    pContext->m_operator = originOperator;// restore the operator which may be changed in success and failed action.
    return true;
}
bool CConditionActionBase::Exam(SActionContext* /*pContext*/)
{
    return false;
}