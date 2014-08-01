#include "stdafx.h"
#include "EventSubscription.h"
#include "EventDispatcher.h"

CEventSubscription::CEventSubscription(
    int id, int type, const EventHandler &handler, CEventDispatcher *pDispatcher)
    : m_id(id)
    , m_type(type)
    , m_handler(handler)
    , m_pDispatcher(pDispatcher)
{
    BEATS_ASSERT(m_pDispatcher);
}

int CEventSubscription::ID() const
{
    return m_id;
}

int CEventSubscription::Type() const
{
    return m_type;
}

const CEventSubscription::EventHandler &CEventSubscription::Handler() const
{
    return m_handler;
}

void CEventSubscription::Unsubscribe()
{
    if(m_id != 0)
    {
        m_pDispatcher->UnsubscribeEvent(*this);
        m_id = 0;
    }
}
