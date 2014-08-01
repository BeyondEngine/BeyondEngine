#ifndef BEYOND_ENGINE_EVENT_BASEEVENT_H__INCLUDE
#define BEYOND_ENGINE_EVENT_BASEEVENT_H__INCLUDE

#include "Event/EventType.h"

class CEventDispatcher;

class CBaseEvent
{
public:
    CBaseEvent(EEventType type)
        : m_nType(type)
        , m_bStopPropagation(false)
        , m_pSource(nullptr)
    {}

    virtual ~CBaseEvent(){}

    EEventType GetType() const
    {
        return m_nType;
    }

    void StopPropagation()
    {
        m_bStopPropagation = true;
    }

    bool Stopped() const
    {
        return m_bStopPropagation;
    }

    void SetSource(CEventDispatcher *source)
    {
        m_pSource = source;
    }

    CEventDispatcher *Source() const
    {
        return m_pSource;
    }

protected:
    EEventType m_nType;
    bool m_bStopPropagation;
    CEventDispatcher *m_pSource;
};

#endif