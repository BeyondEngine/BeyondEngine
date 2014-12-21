#include "stdafx.h"
#include "EventManager.h"

CEventManager* CEventManager::m_pInstance = nullptr;

CEventManager::CEventManager()
{
}

CEventManager::~CEventManager()
{
}

void CEventManager::Update(float /*dtt*/)
{
    if (m_postedEventList.size() > 0)
    {
        for (auto& iter : m_postedEventList)
        {
            HandleEvent(iter);
        }
        m_postedEventList.clear();
    }
}

void CEventManager::RegisterHandler(EGameEventType eType, std::function<void()> handler)
{
    m_handlerMap[eType] = handler;
}

void CEventManager::HandleEvent(EGameEventType eType)
{
    if (m_handlerMap.find(eType) != m_handlerMap.end())
    {
        m_handlerMap[eType]();
    }
}

void CEventManager::PostEvent(EGameEventType eType)
{
    m_postedEventList.push_back(eType);
}
