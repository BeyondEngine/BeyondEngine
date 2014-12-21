#include "stdafx.h"
#include "EventConnection.h"
#include "EventSubscription.h"

CEventConnection::CEventConnection()
{
}

CEventConnection::CEventConnection(std::shared_ptr<CEventSubscription> pSubscription)
    : m_pSubscription(pSubscription)
{
}

CEventConnection::CEventConnection(CEventConnection &&rhs)
{
    m_pSubscription.swap(rhs.m_pSubscription);
}

CEventConnection &CEventConnection::operator = (CEventConnection &&rhs)
{
    m_pSubscription.reset();
    m_pSubscription.swap(rhs.m_pSubscription);
    return *this;
}

CEventConnection::~CEventConnection()
{
}

void CEventConnection::Disconnect()
{
    if(!m_pSubscription.expired())
    {
        m_pSubscription.lock()->Unsubscribe();
        m_pSubscription.reset();
    }
}
