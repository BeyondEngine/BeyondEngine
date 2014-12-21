#include "stdafx.h"
#include "Plane.h"

CPlane::CPlane(float a, float b, float c, float d)
{
    Fill(a, b, c, d);
}

void CPlane::Fill(float a, float b, float c, float d)
{
    m_normal.Fill(a, b, c);
    m_fDistance = d;
}

void CPlane::FromPointNormal(const CVec3& point, const CVec3& normal)
{
    m_normal = normal;
    m_fDistance = -normal.Dot(point);
}

const CVec3& CPlane::GetNormal() const
{
    return m_normal;
}

float CPlane::GetDistance() const
{
    return m_fDistance;
}

void CPlane::FromPoints(const CVec3& point1, const CVec3& point2, const CVec3& point3)
{
    CVec3 n, v1, v2;
    v1 = point2 - point1;
    v2 = point3 - point1;
    n = v1.Cross(v2);
    n.Normalize();
    FromPointNormal(point1, n);
}

void CPlane::FromPointNormalAndDistance(const CVec3& normal, float d)
{
    BEATS_ASSERT(normal.IsNormalized());
    m_normal = normal;
    m_fDistance = d;
}

EPlaneSide CPlane::PositionTest(const CVec3& pos) const
{
    EPlaneSide ret = EPlaneSide::ePS_OnPlane;
    float fRet = pos.X() * m_normal.X() + pos.Y() * m_normal.Y() + pos.Z() * m_normal.Z() + m_fDistance;
    if (!BEATS_FLOAT_EQUAL(fRet, 0))
    {
        ret = fRet > 0 ? EPlaneSide::ePS_Positive : EPlaneSide::ePS_Negative;
    }
    return ret;
}
