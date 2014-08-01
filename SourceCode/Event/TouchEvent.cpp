#include "stdafx.h"
#include "TouchEvent.h"
#include "EventType.h"
#include "Touch.h"

CTouchEvent::CTouchEvent( EEventType type )
    : CBaseEvent( type )
{
    m_touchVector.reserve( TOUCH_MAX_NUM );
    m_fDistance = 0.0f;
    kmVec2Fill( &m_vec2TouchPoint, 0.0f, 0.0f );
    m_iDelta = 0;
}

CTouchEvent::~CTouchEvent()
{

}

size_t CTouchEvent::GetTouchNum() const
{
    return m_touchVector.size();
}

CTouch *CTouchEvent::GetTouch(size_t index) const
{
    BEATS_ASSERT(index < m_touchVector.size());
    return m_touchVector[index];
}

CTouch *CTouchEvent::GetTouchByID(int ID) const
{
    auto itr = std::find_if(m_touchVector.begin(), m_touchVector.end(),
        [ID](CTouch *pTouch){ return pTouch->ID() == ID; });
    return itr != m_touchVector.end() ? *itr : nullptr;
}

void CTouchEvent::AddTouch( CTouch* pTouch )
{
    m_touchVector.push_back( pTouch );
}

const kmVec2& CTouchEvent::GetTouchPoint()
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
    case eET_EVENT_TOUCH_DOUBLE_TAPPED:
        m_vec2TouchPoint = m_touchVector[ 0 ]->GetCurrentPoint();
        break;
    case eET_EVENT_TOUCH_SINGLE_TAPPED:
        m_vec2TouchPoint = m_touchVector[ 0 ]->GetCurrentPoint();
        break;
    case eET_EVENT_TOUCH_SCORLLED:
        {
            BEATS_ASSERT( m_touchVector.size() != 2, _T("the point num smaller than 2, cant scroll") );
            kmVec2 p1 = m_touchVector[ 0 ]->GetCurrentPoint();
            kmVec2 p2 = m_touchVector[ 1 ]->GetCurrentPoint();
            kmVec2 temp;
            kmVec2Subtract( &p1, &p2, &temp );
            float dis = kmVec2Length( &temp );
            if ( m_fDistance != 0.0f )
            {
                m_iDelta = m_fDistance > dis ? -1 : 1;
            }
            m_fDistance = dis;
            m_vec2TouchPoint.x = p1.x + ( p2.x - p1.x ) * 0.5f;
            m_vec2TouchPoint.y = p1.y + ( p2.y - p1.y ) * 0.5f;
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
