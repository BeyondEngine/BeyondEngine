#ifndef BEYOND_ENGINE_GUI_EVENT_WINDOWEVENT_H__INCLUDE
#define BEYOND_ENGINE_GUI_EVENT_WINDOWEVENT_H__INCLUDE

#include "Event/BaseEvent.h"

class CControl;

class CWindowEvent : public CBaseEvent
{
public:
    CWindowEvent(EEventType type, CControl *operand = nullptr);

    CControl *SourceWindow() const;

    CControl *OperandWindow() const;

protected:
    CControl *m_pOperand;
};

#endif