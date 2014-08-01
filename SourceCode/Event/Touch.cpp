#include "stdafx.h"
#include "Touch.h"

CTouch::CTouch( int id )
    : m_bStart(false)
    , m_id(id)
    , m_uTapCount(0)
{
    kmVec2Fill( &m_ve2Point, 0.0, 0.0 );
    kmVec2Fill( &m_ve2PreviousPoint, 0.0, 0.0 );
    kmVec2Fill( &m_ve2StartPoint, 0.0, 0.0 );
    kmVec2Fill( &m_deltaXY, 0.0f, 0.0f );
}

CTouch::CTouch( CTouch&& other )
{
    *this = std::move( other );
}

CTouch::~CTouch()
{

}

int CTouch::ID() const
{
    return m_id;
}

void CTouch::UpdateTouchInfo( int id, float x, float y )
{
    BEATS_ASSERT( m_id == id, _T("Is touch id right?"));
    kmVec2Fill( &m_ve2PreviousPoint, m_ve2Point.x, m_ve2Point.y );
    kmVec2Fill( &m_ve2Point, x, y );
    if ( !m_bStart )
    {
        kmVec2Fill( &m_ve2StartPoint, x, y );
        kmVec2Fill( &m_ve2PreviousPoint, m_ve2Point.x, m_ve2Point.y );
        m_bStart = true;
    }
}

void CTouch::SetTapCount(unsigned int uTapCount)
{
    m_uTapCount = uTapCount;
}

unsigned int CTouch::GetTapCount() const
{
    return m_uTapCount;
}

const kmVec2& CTouch::GetStartPoint() const
{
    return m_ve2StartPoint;
}

const kmVec2& CTouch::GetCurrentPoint() const
{
    return m_ve2Point;
}

const kmVec2& CTouch::GetPreviousPoint() const
{
    return m_ve2PreviousPoint;
}

float CTouch::GetDelta() const
{
    kmVec2 subV;
    kmVec2Subtract( &subV, &m_ve2Point, &m_ve2PreviousPoint );
    return kmVec2Length( &subV );
}

CTouch& CTouch::operator=( CTouch&& other )
{
    m_id = other.m_id;
    m_bStart = other.m_bStart;
    kmVec2Fill( &m_ve2Point, other.m_ve2Point.x, other.m_ve2Point.y );
    kmVec2Fill( &m_ve2PreviousPoint, other.m_ve2PreviousPoint.x, other.m_ve2PreviousPoint.y );
    kmVec2Fill( &m_ve2StartPoint, other.m_ve2StartPoint.x, other.m_ve2StartPoint.y );
    return *this;
}

const kmVec2& CTouch::GetDeltaXY()
{
    kmVec2Fill( &m_deltaXY, m_ve2Point.x - m_ve2PreviousPoint.x, m_ve2Point.y - m_ve2PreviousPoint.y );
    return m_deltaXY;
}
