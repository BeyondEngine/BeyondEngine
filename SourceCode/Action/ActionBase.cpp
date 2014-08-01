#include "stdafx.h"
#include "ActionBase.h"

CActionBase::CActionBase()
    : m_pNextAction(NULL)
{

}

CActionBase::~CActionBase()
{

}

bool CActionBase::Execute(SActionContext* pContext)
{
    bool bRet = ExecuteImp(pContext);
    if (bRet && m_pNextAction != NULL)
    {
        m_pNextAction->Execute(pContext);
    }
    return bRet;
}

void CActionBase::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_DEPENDENCY(serializer, m_pNextAction, _T("обр╩╡╫"), eDT_Weak);
}

