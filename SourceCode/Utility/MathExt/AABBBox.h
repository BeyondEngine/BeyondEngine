#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_AABBBOX_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_AABBBOX_H__INCLUDE
#include "Vec3.h"
struct SFrustumPlanes;
class CAABBBox
{
public:
    CAABBBox();
    ~CAABBBox();
    bool IntersectWithFrustum(const SFrustumPlanes& frustum, uint32_t* pInPointCount = nullptr) const;
    bool IntersectWithRay(const CRay3&ray, float& fDist) const;
    EPlaneSide IntersectWithPlane(const CPlane& plane) const;
    CAABBBox& operator = (const CAABBBox& rhs);
    void BuildWithPos(const CVec3& pos);
public:
    CVec3 m_minPos;
    CVec3 m_maxPos;
};
#endif