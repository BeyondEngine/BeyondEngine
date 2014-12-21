#ifndef BEYOND_ENGINE_EVENT_TOUCHEVENT_H__INCLUDE
#define BEYOND_ENGINE_EVENT_TOUCHEVENT_H__INCLUDE

#include "Event/BaseEvent.h"

class CTouch;

class CTouchEvent : public CBaseEvent
{
    typedef std::vector< CTouch* > TTouchVector;
public:
    CTouchEvent( EEventType type );
    ~CTouchEvent();

    CTouchEvent( CTouchEvent&& other );
    CTouchEvent& operator=( CTouchEvent&& other );

    uint32_t GetTouchNum() const;
    CTouch *GetTouch(uint32_t index) const;
    CTouch *GetTouchByID(size_t ID) const;

    void AddTouch( CTouch* pTouch );

    const CVec2& GetTouchPoint();
    int GetDelta() const;

protected:
    TTouchVector m_touchVector;
    CVec2 m_vec2TouchPoint;
    float m_fDistance;
    int m_iDelta;
};

#endif//TOUCHEVENT_H_INCLUDE