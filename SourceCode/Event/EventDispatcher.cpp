#include "stdafx.h"
#include "EventDispatcher.h"
#include "BaseEvent.h"

CEventDispatcher::CEventDispatcher()
    : m_bEnable(true)
    , m_currSubID(1)
{
}

CEventDispatcher::~CEventDispatcher()
{
}

CEventConnection CEventDispatcher::SubscribeEvent( 
    int type, const CEventSubscription::EventHandler &handlerFunc, EHandlerPriority priority)
{
    std::shared_ptr<CEventSubscription> pSubscription(new CEventSubscription(
        priority + m_currSubID++, type, handlerFunc, this));
    m_subscriptions[type].emplace(pSubscription->ID(), pSubscription);
    return pSubscription;
}

void CEventDispatcher::SetEnable(bool bEnable)
{
    m_bEnable = bEnable;
}

void CEventDispatcher::Clear()
{
    m_subscriptions.clear();
}

void CEventDispatcher::DispatchEvent( CBaseEvent *event )
{
    if (m_bEnable)
    {
        auto itr = m_subscriptions.find(event->GetType());
        if(itr != m_subscriptions.end())
        {
            event->SetSource(this);
            for(auto pSubscription : itr->second)
            {
                pSubscription.second->Handler()(event);
                if(event->Stopped())
                    break;
            }
        }
    }
}

void CEventDispatcher::UnsubscribeEvent(const CEventSubscription &subscription)
{
    UnsubscribeEvent(subscription.Type(), subscription.ID());
}

void CEventDispatcher::UnsubscribeEvent( int type, int id )
{
    BEATS_ASSERT(id != 0);
    auto itr = m_subscriptions.find( type );
    if(itr != m_subscriptions.end())
    {
        auto itrSub = itr->second.find(id);
        BEATS_ASSERT(itrSub != itr->second.end());
        itr->second.erase(itrSub);
    }
}
