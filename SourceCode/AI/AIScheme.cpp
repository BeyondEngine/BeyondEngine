#include "stdafx.h"
#include "AIScheme.h"
#include "AIState.h"
#include "AIManager.h"

CAIScheme::CAIScheme()
    : m_pInitState(NULL)
{

}

CAIScheme::~CAIScheme()
{

}

void CAIScheme::Initialize()
{
    super::Initialize();
#ifndef EDITOR_MODE
    BEATS_ASSERT(m_states.size() > 0, _T("AI Scheme must have at least one state!"));
    BEATS_ASSERT(m_pInitState != NULL, _T("Init state can not be null!"));
#endif
    if (!m_strName.empty())
    {
        CAIManager::GetInstance()->RegisterScheme(this);
    }
}

void CAIScheme::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strName, true, 0xFFFFFFFF, _T("AI名称"), NULL, NULL, NULL);
    DECLARE_DEPENDENCY(serializer, m_pInitState, _T("初始状态"), eDT_Strong);
    DECLARE_DEPENDENCY_LIST(serializer, m_states, _T("状态集合"), eDT_Strong);
}

bool CAIScheme::OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pNewValueToBeSet);
    if (!bRet)
    {
        if (pVariableAddr == &m_strName)
        {
            TString strNewName;
            DeserializeVariable(strNewName, pNewValueToBeSet);
            if (!m_strName.empty())
            {
                CAIManager::GetInstance()->UnregisterScheme(this);
            }
            m_strName = strNewName;
            CAIManager::GetInstance()->RegisterScheme(this);
            bRet = true;
        }
    }
    return bRet;
}

const TString& CAIScheme::GetName() const
{
    return m_strName;
}

CAIState* CAIScheme::GetInitState() const
{
    return m_pInitState;
}

bool CAIScheme::HasState(CAIState* pState) const
{
    bool bRet = false;
    for (size_t i = 0; i < m_states.size(); ++i)
    {
        if (m_states[i] == pState)
        {
            bRet = true;
            break;
        }
    }
    return bRet;
}
