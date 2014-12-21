#include "stdafx.h"
#include "Touch.h"

CTouch::CTouch(size_t id)
    : m_bStart(false)
    , m_id(id)
    , m_uTapCount(0)
{
}

CTouch::CTouch( CTouch&& other )
{
    *this = std::move( other );
}

CTouch::~CTouch()
{

}

size_t CTouch::ID() const
{
    return m_id;
}

void CTouch::UpdateTouchInfo(size_t id, float x, float y)
{
    BEYONDENGINE_UNUSED_PARAM(id);
    BEATS_ASSERT( m_id == id, _T("Is touch id right?"));
    m_ve2PreviousPoint.Fill(m_ve2Point.X(), m_ve2Point.Y() );
    m_ve2Point.Fill(x, y );
    if ( !m_bStart )
    {
        m_ve2StartPoint.Fill(x, y );
        m_ve2PreviousPoint.Fill( m_ve2Point.X(), m_ve2Point.Y() );
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

const CVec2& CTouch::GetStartPoint() const
{
    return m_ve2StartPoint;
}

const CVec2& CTouch::GetCurrentPoint() const
{
    return m_ve2Point;
}

const CVec2& CTouch::GetPreviousPoint() const
{
    return m_ve2PreviousPoint;
}

float CTouch::GetDelta() const
{
    CVec2 subV;
    subV = m_ve2Point - m_ve2PreviousPoint;
    return subV.Length();
}

CTouch& CTouch::operator=( CTouch&& other )
{
    m_id = other.m_id;
    m_bStart = other.m_bStart;
    m_ve2Point.Fill( other.m_ve2Point.X(), other.m_ve2Point.Y() );
    m_ve2PreviousPoint.Fill( other.m_ve2PreviousPoint.X(), other.m_ve2PreviousPoint.Y() );
    m_ve2StartPoint.Fill( other.m_ve2StartPoint.X(), other.m_ve2StartPoint.Y() );
    return *this;
}

const CVec2& CTouch::GetDeltaXY()
{
    m_deltaXY.Fill(m_ve2Point.X() - m_ve2PreviousPoint.X(), m_ve2Point.Y() - m_ve2PreviousPoint.Y() );
    return m_deltaXY;
}

void CTouch::Reset(size_t uID)
{
    m_bStart = false;
    m_id = uID;
    m_ve2Point.Fill(0.0, 0.0);
    m_ve2PreviousPoint.Fill(0.0, 0.0);
    m_ve2StartPoint.Fill(0.0, 0.0);
    m_deltaXY.Fill(0.0f, 0.0f);
}
