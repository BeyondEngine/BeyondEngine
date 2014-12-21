#include "stdafx.h"
#include "Location.h"

CLocation::CLocation()
: m_scale(CVec3(1,1,1))
{

}

CLocation::~CLocation()
{

}

CLocation& CLocation::operator = (const CLocation& rhs)
{
    m_scale = rhs.m_scale;
    m_pos = rhs.m_pos;
    m_rotation = rhs.m_rotation;
    return *this;
}

CLocation CLocation::Lerp(const CLocation& rhs, float fFactor) const
{
    CLocation ret;
    ret.m_scale = m_scale.Lerp(rhs.m_scale, fFactor);
    ret.m_pos = m_pos.Lerp(rhs.m_pos, fFactor);
    ret.m_rotation = m_rotation.Slerp(rhs.m_rotation, fFactor);
    return ret;
}
