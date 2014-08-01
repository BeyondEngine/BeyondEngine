#ifndef BEYOND_ENGINE_EVENT_TOUCHDISPATHER_H__INCLUDE
#define BEYOND_ENGINE_EVENT_TOUCHDISPATHER_H__INCLUDE

#include "Event/EventDispatcher.h"
#include "Event/TouchEvent.h"
#include "Event/EventType.h"

class CTouchDispather : public CEventDispatcher
{
public:
    CTouchDispather();
    virtual ~CTouchDispather();

    void AddTouch( CTouch* pTouch );
    void DispatchEvent( EEventType type );    
    void Remove( CTouch* pTouch );

private:
    std::vector< CTouch* > m_vecTouch;
};

#endif//EVENT_TOUCH_INCLUDE__H