#ifndef BEYOND_ENGINE_EVENT_EVENTSLOT_H__INCLUDE
#define BEYOND_ENGINE_EVENT_EVENTSLOT_H__INCLUDE

#include "EventConnection.h"

//This class is used to store event connection, and disconnect it automatically
class CEventSlot : public CEventConnection
{
public:
    CEventSlot();
    CEventSlot(CEventSlot &&rhs);
    CEventSlot(CEventConnection &&rhs);
    CEventSlot &operator = (CEventConnection &&rhs);
    ~CEventSlot();
private:
    CEventSlot(const CEventSlot &rhs);
    CEventSlot(const CEventConnection &rhs);
    CEventSlot &operator = (const CEventConnection &rhs);
};

#endif