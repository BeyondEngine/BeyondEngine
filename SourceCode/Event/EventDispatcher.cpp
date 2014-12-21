#include "stdafx.h"
#include "EventDispatcher.h"
#include "BaseEvent.h"
#ifdef EDITOR_MODE
#include "Scene/SceneManager.h"
#include "Scene/Node.h"
#include "TouchEvent.h"
#endif

CEventDispatcher::CEventDispatcher()
    : m_bEnable(true)
    , m_currSubID(1)
{
}

CEventDispatcher::~CEventDispatcher()
{
}

CEventConnection CEventDispatcher::SubscribeEvent(int type, const CEventSubscription::EventHandler &handlerFunc, EHandlerPriority priority)
{
    std::shared_ptr<CEventSubscription> pSubscription(new CEventSubscription(priority + m_currSubID++, type, handlerFunc, this));
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
            // To avoid clear m_subscriptions in the callback, so we got a bak.
            EventSubscriptionMap callbackBak = itr->second;
            for (auto pSubscription : callbackBak)
            {
                pSubscription.second->Handler()(event);
                if(event->Stopped())
                    break;
            }
        }
#ifdef EDITOR_MODE
        if (!CSceneManager::GetInstance()->GetSwitchSceneState())
        {
            const std::map<uint32_t, CComponentProxy*>& componentsInScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
            for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
            {
                CComponentProxy* pProxy = iter->second;
                if (pProxy->GetHostComponent() &&
                    pProxy->GetBeConnectedDependencyLines()->size() == 0)
                {
                    CNode* pNode = dynamic_cast<CNode*>(pProxy->GetHostComponent());
                    if (pNode && pNode->GetParentNode() == NULL)
                    {
                        CTouchEvent* touchEvent = dynamic_cast<CTouchEvent*>(event);
                        if (touchEvent)
                        {
                            pNode->OnTouchEvent(touchEvent);
                        }
                    }
                }
            }
        }
#endif
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
