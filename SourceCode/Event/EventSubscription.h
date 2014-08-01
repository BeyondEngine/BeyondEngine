#ifndef BEYOND_ENGINE_EVENT_EVENTSUBSCRIPTION_H__INCLUDE
#define BEYOND_ENGINE_EVENT_EVENTSUBSCRIPTION_H__INCLUDE

class CEventDispatcher;
class CBaseEvent;

class CEventSubscription
{
public:
    typedef std::function<void(CBaseEvent *)> EventHandler;

    CEventSubscription(int id, int type, 
        const EventHandler &handler, CEventDispatcher *pDispatcher);
    int ID() const;
    int Type() const;
    const EventHandler &Handler() const;
    void Unsubscribe();

private:
    int m_id;
    int m_type;
    EventHandler m_handler;
    CEventDispatcher *m_pDispatcher;
};

#endif