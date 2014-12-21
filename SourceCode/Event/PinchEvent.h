#ifndef BEYOND_ENGINE_EVENT_PINCHEVENT_H__INCLUDE
#define BEYOND_ENGINE_EVENT_PINCHEVENT_H__INCLUDE

#include "BaseEvent.h"
#include "GestureState.h"
class CTouch;

class CPinchEvent : public CBaseEvent
{
public:
    CPinchEvent(EEventType type, EGestureState state, float fPinchScale)
        : CBaseEvent(type)
        , m_state(state)
        , m_fPinchScale(fPinchScale)
    {}
    
    EGestureState State() const
    {
        return m_state;
    }
    
    float Scale() const
    {
        return m_fPinchScale;
    }

    void SetTouch(CTouch* pFirstTouch, CTouch* pSecondTouch)
    {
        m_pFisrtTouch = pFirstTouch;
        m_pSecondTouch = pSecondTouch;
    }

    CTouch* GetFirstTouch() const
    {
        return m_pFisrtTouch;
    }

    CTouch* GetSecondTouch() const
    {
        return m_pSecondTouch;
    }

private:
    EGestureState m_state;
    float m_fPinchScale;
    CTouch* m_pFisrtTouch;
    CTouch* m_pSecondTouch;
};

#endif
