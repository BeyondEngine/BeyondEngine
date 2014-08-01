#ifndef BEYOND_ENGINE_EVENT_EVENTCONNECTION_H__INCLUDE
#define BEYOND_ENGINE_EVENT_EVENTCONNECTION_H__INCLUDE

class CEventSubscription;

//This class is used to unsubscribe event safely
class CEventConnection
{
public:
    CEventConnection();
    CEventConnection(std::shared_ptr<CEventSubscription> pSubscription);
    CEventConnection(CEventConnection &&rhs);
    CEventConnection &operator = (CEventConnection &&rhs);
    ~CEventConnection();

    void Disconnect();

private:
    CEventConnection(const CEventConnection &);
    void operator = (const CEventConnection &);

    std::weak_ptr<CEventSubscription> m_pSubscription;
};

#endif