#ifndef BEYOND_ENGINE_EVENT_EVENTDISPATCHER_H__INCLUDE
#define BEYOND_ENGINE_EVENT_EVENTDISPATCHER_H__INCLUDE

#include "EventSubscription.h"
#include "EventConnection.h"

//forward declaration
class CBaseEvent;

class CEventDispatcher
{
public:
    enum EHandlerPriority
    {
        eHP_HIGHEST = 0,
        eHP_HIGHER = 1 << 24,
        eHP_DEFAULT = 2 << 24,
        eHP_LOWER = 3 << 24,
        eHP_LOWEST = 4 << 24,
    };
    typedef std::map<int, std::shared_ptr<CEventSubscription>> EventSubscriptionMap;
    typedef std::map<int, EventSubscriptionMap> EventSubscriptionTypeMap;

public:
    CEventDispatcher();
    virtual ~CEventDispatcher();

    virtual CEventConnection SubscribeEvent(int type, 
        const CEventSubscription::EventHandler &handlerFunc,
        EHandlerPriority priority = eHP_DEFAULT);

    template <typename MemberFunc, typename ObjectType>
    CEventConnection SubscribeEvent(int type, const MemberFunc &func, ObjectType *obj,
        EHandlerPriority priority = eHP_DEFAULT);

    void UnsubscribeEvent(const CEventSubscription &subscription);
    void UnsubscribeEvent( int type, int id );

    void SetEnable(bool bEnable);
    void Clear();

    virtual void DispatchEvent(CBaseEvent *event);

private:
    bool m_bEnable;
    int m_currSubID;
protected:
    EventSubscriptionTypeMap m_subscriptions;
};

template <typename MemberFunc, typename ObjectType>
CEventConnection CEventDispatcher::SubscribeEvent(int type, const MemberFunc &func,
                                                  ObjectType *obj, EHandlerPriority priority)
{
    return SubscribeEvent(type, std::bind(func, obj, std::placeholders::_1), priority);
}

#endif