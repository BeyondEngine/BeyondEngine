#include "stdafx.h"
#include "Ray3.h"
#include "gtx/intersect.hpp"

CRay3::CRay3()
{
}

CRay3::~CRay3()
{

}

CRay3& CRay3::operator = (const CRay3 &rhs)
{
    m_start = rhs.m_start;
    m_dir = rhs.m_dir;
    return *this;
}

void CRay3::FromPointAndDirection(const CVec3& point, const CVec3& direction)
{
    m_start = point;
    m_dir = direction;
    m_dir.Normalize();
}

bool CRay3::IntersectPlane(const CPlane& plane, float& fIntersectDistance) const
{
    bool bRet = false;
    const CVec3& planeNormal = plane.GetNormal();
    float d = planeNormal.Dot(m_dir);
    if (d != 0)
    {
        fIntersectDistance = -(planeNormal.Dot(m_start) + plane.GetDistance()) / d;
        if (fIntersectDistance >= 0)
        {
            bRet = true;
        }
    }
    return bRet;
}

bool CRay3::IntersectSphere(const CVec3& sphereCenter, float fSphereRadius, float& fIntersectLength)
{
    return glm::intersectRaySphere(m_start.m_data, m_dir.m_data, sphereCenter.m_data, fSphereRadius * fSphereRadius, fIntersectLength);
}

bool CRay3::IntersectTriangle(const CVec3& pos1, const CVec3& pos2, const CVec3& pos3, CVec3& out) const
{
    return glm::intersectRayTriangle(m_start.m_data, m_dir.m_data, pos1.m_data, pos2.m_data, pos3.m_data, out.m_data);
}

bool CRay3::IntersectTriangleBothSide(const CVec3& pos1, const CVec3& pos2, const CVec3& pos3, CVec3& out) const
{
    return glm::intersectRayTriangleBothSide(m_start.m_data, m_dir.m_data, pos1.m_data, pos2.m_data, pos3.m_data, out.m_data);
}

const CVec3& CRay3::GetStartPos() const
{
    return m_start;
}

const CVec3& CRay3::GetDirection() const
{
    return m_dir;
}
