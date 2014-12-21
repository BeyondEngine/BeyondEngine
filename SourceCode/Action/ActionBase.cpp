#include "stdafx.h"
#include "ActionBase.h"

CActionBase::CActionBase()
    : m_pNextAction(NULL)
    , m_bOperateOnOwn(false)
{

}

CActionBase::~CActionBase()
{

}

bool CActionBase::Execute(SActionContext* pContext)
{
    if (pContext && m_bOperateOnOwn && !pContext->m_bOperatorIncludeSelf)
    {
        if (pContext->m_pRootNode != NULL)
        {
            pContext->m_operator.push_back(pContext->m_pRootNode);
            pContext->m_bOperatorIncludeSelf = true;
        }
    }
    bool bRet = ExecuteImp(pContext);
    if (pContext && pContext->m_bOperatorIncludeSelf)
    {
        BEATS_ASSERT(pContext->m_operator.back() == pContext->m_pRootNode);
        pContext->m_operator.pop_back();
        pContext->m_bOperatorIncludeSelf = false;
    }
    if (bRet && m_pNextAction != NULL)
    {
        m_pNextAction->Execute(pContext);
    }
    return bRet;
}

void CActionBase::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bOperateOnOwn, true, 0xFFFFFFFF, _T("应用于自身"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strName, true, 0xFFFFFFFF, _T("名称"), NULL, NULL, NULL);
    DECLARE_DEPENDENCY(serializer, m_pNextAction, _T("下一步"), eDT_Weak);
}

const TString& CActionBase::GetName() const
{
    return m_strName;
}

CActionBase* CActionBase::CloneWholeActionTree(bool bCallInit) const
{
    CActionBase* pRet = down_cast<CActionBase*>(CloneInstance());
    if (bCallInit)
    {
        pRet->Initialize();
    }
    if (m_pNextAction != nullptr)
    {
        pRet->m_pNextAction = m_pNextAction->CloneWholeActionTree(bCallInit);
    }
    return pRet;
}

CActionBase* CActionBase::GetNextAction() const
{
    return m_pNextAction;
}