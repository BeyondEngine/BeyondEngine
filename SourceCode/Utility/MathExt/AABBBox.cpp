#include "stdafx.h"
#include "AABBBox.h"
#include "Render/Camera.h"
#include "Plane.h"

CAABBBox::CAABBBox()
{

}

CAABBBox::~CAABBBox()
{

}

// TODO: We have two ways to intersect aabb with frustum
// 1. Check 8 points with 6 planes
// 2. Check aabbbox with 6 planes.
// I don't know which way is better(fast), please do a profile to choose the better one.
bool CAABBBox::IntersectWithFrustum(const SFrustumPlanes& frustum, uint32_t* pInPointCount /*=nullptr*/) const
{
    bool bRet = false;
    const CVec3 box[8] =
    {
        CVec3(m_minPos.X(), m_minPos.Y(), m_minPos.Z()),
        CVec3(m_maxPos.X(), m_minPos.Y(), m_minPos.Z()),
        CVec3(m_minPos.X(), m_maxPos.Y(), m_minPos.Z()),
        CVec3(m_minPos.X(), m_minPos.Y(), m_maxPos.Z()),
        CVec3(m_maxPos.X(), m_maxPos.Y(), m_maxPos.Z()),
        CVec3(m_minPos.X(), m_maxPos.Y(), m_maxPos.Z()),
        CVec3(m_maxPos.X(), m_minPos.Y(), m_maxPos.Z()),
        CVec3(m_maxPos.X(), m_maxPos.Y(), m_minPos.Z()),
    };

    for (uint32_t j = 0; j < 8; ++j)
    {
        bool bInFrustum = true;
        for (uint32_t i = (uint32_t)EFrustumPlaneType::eFPT_Near; i < (uint32_t)EFrustumPlaneType::eFPT_Count; ++i)
        {
            if (frustum.m_planes[i].PositionTest(box[j]) == EPlaneSide::ePS_Negative)
            {
                bInFrustum = false;
                break;
            }
        }
        if (bInFrustum)
        {
            bRet = true;
            if (pInPointCount != nullptr)
            {
                *pInPointCount += 1;
            }
            else
            {
                break;
            }
        }
    }
    return bRet;
}

bool CAABBBox::IntersectWithRay(const CRay3& ray, float& fDist) const
{
    bool bRet = false;
    CVec3 hitPoint;
    const CVec3& rayOrig = ray.GetStartPos();
    const CVec3& rayDir = ray.GetDirection();

    // InBox
    if (rayOrig > m_minPos && rayOrig < m_maxPos)
    {
        fDist = 0;
        bRet = true;
    }
    else
    {
        float t = 0;
        //check with all 6 faces
        if (rayOrig.X() < m_minPos.X() && rayDir.X() > 0)
        {
            t = (m_minPos.X() - rayOrig.X()) / rayDir.X(); //get the intersect point on the plane.

            //check if the intersect point is in current plane.
            if (t > 0)
            {
                hitPoint = rayOrig + rayDir * t;
                if (hitPoint.Y() >= m_minPos.Y() && hitPoint.Y() <= m_maxPos.Y() && hitPoint.Z() >= m_minPos.Z() && hitPoint.Z() <= m_maxPos.Z() && (!bRet || t<fDist))
                {
                    bRet = true;
                    fDist = t;
                }
            }
        }

        if (rayOrig.X() > m_maxPos.X() && rayDir.X() < 0)
        {
            t = (m_maxPos.X() - rayOrig.X()) / rayDir.X();
            if (t>0)
            {
                hitPoint = rayOrig + rayDir*t;
                if (hitPoint.Y() > m_minPos.Y() && hitPoint.Y() <= m_maxPos.Y() &&
                    hitPoint.Z() >= m_minPos.Z() && hitPoint.Z() <= m_maxPos.Z() &&
                    (!bRet || t < fDist))
                {
                    bRet = true;
                    fDist = t;
                }
            }
        }

        if (rayOrig.Y()<m_minPos.Y() && rayDir.Y()>0)
        {
            t = (m_minPos.Y() - rayOrig.Y()) / rayDir.Y();
            if (t > 0)
            {
                hitPoint = rayOrig + rayDir*t;
                if (hitPoint.X() >= m_minPos.X() && hitPoint.X() <= m_maxPos.X() &&
                    hitPoint.Z() >= m_minPos.Z() && hitPoint.Z() <= m_maxPos.Z() &&
                    (!bRet || t < fDist))
                {
                    bRet = true;
                    fDist = t;
                }
            }
        }

        if (rayOrig.Y() > m_maxPos.Y() && rayDir.Y() < 0)
        {
            t = (m_maxPos.Y() - rayOrig.Y()) / rayDir.Y();
            if (t > 0)
            {
                hitPoint = rayOrig + rayDir * t;
                if (hitPoint.X() >= m_minPos.X() && hitPoint.X() <= m_maxPos.X() &&
                    hitPoint.Z() >= m_minPos.Z() && hitPoint.Z() <= m_maxPos.Z() &&
                    (!bRet || t < fDist))
                {
                    bRet = true;
                    fDist = t;
                }
            }
        }

        if (rayOrig.Z() < m_minPos.Z() && rayDir.Z() > 0)
        {
            t = (m_minPos.Z() - rayOrig.Z()) / rayDir.Z();
            if (t > 0)
            {
                hitPoint = rayOrig + rayDir * t;
                if (hitPoint.X() >= m_minPos.X() && hitPoint.X() <= m_maxPos.X() &&
                    hitPoint.Y() >= m_minPos.Y() && hitPoint.Y() <= m_maxPos.Y() &&
                    (!bRet || t < fDist))
                {
                    bRet = true;
                    fDist = t;
                }
            }
        }

        if (rayOrig.Z() > m_maxPos.Z() && rayDir.Z() < 0)
        {
            t = (m_maxPos.Z() - rayOrig.Z()) / rayDir.Z();
            if (t > 0)
            {
                hitPoint = rayOrig + rayDir * t;
                if (hitPoint.X() >= m_minPos.X() && hitPoint.X() <= m_maxPos.X() &&
                    hitPoint.Y() >= m_minPos.Y() && hitPoint.Y() <= m_maxPos.Y() &&
                    (!bRet || t < fDist))
                {
                    bRet = true;
                    fDist = t;
                }
            }
        }
    }
    return bRet;
}

EPlaneSide CAABBBox::IntersectWithPlane(const CPlane& plane) const
{
    EPlaneSide ret = EPlaneSide::ePS_OnPlane;
    const CVec3& planeNormal = plane.GetNormal();
    CVec3 center = (m_minPos + m_maxPos) * 0.5f;
    float fDistanceFromPlane = planeNormal.Dot(center) + plane.GetDistance();
    CVec3 delta = (m_maxPos - m_minPos) * 0.5f;
    // Draw a line(simulate plane) and a rect(simulate AABBBOX)
    // We just compare the distance from center to line and the the project length of delta on plane normal.
    float maxAbsDist = fabs(planeNormal.X() * delta.X()) + fabs(planeNormal.Y() * delta.Y()) + fabs(planeNormal.Z() * delta.Z());
    if (fDistanceFromPlane < -maxAbsDist)
    {
        ret = EPlaneSide::ePS_Negative;
    }
    else if (fDistanceFromPlane > maxAbsDist)
    {
        ret = EPlaneSide::ePS_Positive;
    }
    return ret;
}

CAABBBox& CAABBBox::operator = (const CAABBBox& rhs)
{
    m_minPos = rhs.m_minPos;
    m_maxPos = rhs.m_maxPos;
    return *this;
}

void CAABBBox::BuildWithPos(const CVec3& pos)
{
    if (pos.X() > m_maxPos.X())
    {
        m_maxPos.X() = pos.X();
    }
    if (pos.Y() > m_maxPos.Y())
    {
        m_maxPos.Y() = pos.Y();
    }
    if (pos.Z() > m_maxPos.Z())
    {
        m_maxPos.Z() = pos.Z();
    }
    if (pos.X() < m_minPos.X())
    {
        m_minPos.X() = pos.X();
    }
    if (pos.Y() < m_minPos.Y())
    {
        m_minPos.Y() = pos.Y();
    }
    if (pos.Z() < m_minPos.Z())
    {
        m_minPos.Z() = pos.Z();
    }

}