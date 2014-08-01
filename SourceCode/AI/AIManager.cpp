#include "stdafx.h"
#include "AIManager.h"
#include "AIScheme.h"

CAIManager* CAIManager::m_pInstance = NULL;

CAIManager::CAIManager()
{

}

CAIManager::~CAIManager()
{

}

void CAIManager::RegisterScheme(CAIScheme* pScheme)
{
    BEATS_ASSERT(m_schemeMap.find(pScheme->GetName()) == m_schemeMap.end());
    m_schemeMap[pScheme->GetName()] = pScheme;
}

void CAIManager::UnregisterScheme(CAIScheme* pScheme)
{
    BEATS_ASSERT(m_schemeMap.find(pScheme->GetName()) != m_schemeMap.end());
    m_schemeMap.erase(pScheme->GetName());
}

CAIScheme* CAIManager::GetScheme(const TString& strSchemeName)
{
    CAIScheme* pRet = NULL;
    if (m_schemeMap.find(strSchemeName) != m_schemeMap.end())
    {
        pRet = m_schemeMap[strSchemeName];
    }
    return pRet;
}
