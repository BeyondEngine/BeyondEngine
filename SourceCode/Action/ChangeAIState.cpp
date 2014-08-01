#include "stdafx.h"
#include "ChangeAIState.h"
#include "AI/AIState.h"
#include "AI/AIScheme.h"

CChangeAIState::CChangeAIState()
    : m_pAIState(NULL)
{

}

CChangeAIState::~CChangeAIState()
{

}

bool CChangeAIState::ExecuteImp(SActionContext* pContext)
{
    return true;
}

void CChangeAIState::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_DEPENDENCY(serializer, m_pAIState, _T("×ª»»×´Ì¬µ½"), eDT_Strong);
}