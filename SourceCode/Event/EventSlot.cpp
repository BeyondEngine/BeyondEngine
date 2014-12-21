#include "stdafx.h"
#include "EventSlot.h"
#include "EventConnection.h"

CEventSlot::CEventSlot()
{
}

CEventSlot::CEventSlot(CEventSlot &&rhs)
    : CEventConnection(std::move(rhs))
{
}

CEventSlot::CEventSlot(CEventConnection &&rhs)
    : CEventConnection(std::move(rhs))
{
}

CEventSlot &CEventSlot::operator = (CEventConnection &&rhs)
{
    Disconnect();
    CEventConnection::operator = (std::move(rhs));
    return *this;
}

CEventSlot::~CEventSlot()
{
    Disconnect();
}
