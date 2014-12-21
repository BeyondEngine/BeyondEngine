#include "stdafx.h"
#include "TouchEvent.h"
#include "EventType.h"
#include "Touch.h"

CTouchEvent::CTouchEvent( EEventType type )
    : CBaseEvent( type )
{
    m_touchVector.reserve( TOUCH_MAX_NUM );
    m_fDistance = 0.0f;
    m_iDelta = 0;
}

CTouchEvent::~CTouchEvent()
{

}

uint32_t CTouchEvent::GetTouchNum() const
{
    return (uint32_t)m_touchVector.size();
}

CTouch *CTouchEvent::GetTouch(uint32_t index) const
{
    BEATS_ASSERT(index < m_touchVector.size());
    return m_touchVector[index];
}

CTouch *CTouchEvent::GetTouchByID(size_t ID) const
{
    auto itr = std::find_if(m_touchVector.begin(), m_touchVector.end(),
        [ID](CTouch *pTouch){ return pTouch->ID() == ID; });
    return itr != m_touchVector.end() ? *itr : nullptr;
}

void CTouchEvent::AddTouch( CTouch* pTouch )
{
    m_touchVector.push_back( pTouch );
}

const CVec2& CTouchEvent::GetTouchPoint()
{
    switch ( m_nType )
    {
    case eET_EVENT_TOUCH_BEGAN:
        m_vec2TouchPoint = m_touchVector[ 0 ]->GetCurrentPoint();
        break;
    case eET_EVENT_TOUCH_MOVED:
        m_vec2TouchPoint = m_touchVector[ 0 ]->GetCurrentPoint();
        break;
    case eET_EVENT_TOUCH_ENDED:
        m_vec2TouchPoint = m_touchVector[ 0 ]->GetCurrentPoint();
        break;
    case eET_EVENT_TOUCH_CANCELLED:
        m_vec2TouchPoint = m_touchVector[ 0 ]->GetCurrentPoint();
        break;
    case eET_EVENT_TOUCH_SCORLLED:
        {
            BEATS_ASSERT( m_touchVector.size() != 2, _T("the point num smaller than 2, cant scroll") );
            CVec2 p1 = m_touchVector[ 0 ]->GetCurrentPoint();
            CVec2 p2 = m_touchVector[ 1 ]->GetCurrentPoint();
            float dis = (p1 - p2).Length();
            if ( m_fDistance != 0.0f )
            {
                m_iDelta = m_fDistance > dis ? -1 : 1;
            }
            m_fDistance = dis;
            m_vec2TouchPoint.X() = p1.X() + ( p2.X() - p1.X() ) * 0.5f;
            m_vec2TouchPoint.Y() = p1.Y() + ( p2.Y() - p1.Y() ) * 0.5f;
        }
        break;
    case eET_EVENT_TOUCH_PINCHED:
        m_vec2TouchPoint = m_touchVector[ 0 ]->GetCurrentPoint();
        break;
    default:
        BEATS_ASSERT( false, _T("unknow event type!!") );
        break;
    }

    return m_vec2TouchPoint;
}

int CTouchEvent::GetDelta() const
{
    return m_iDelta;
}
