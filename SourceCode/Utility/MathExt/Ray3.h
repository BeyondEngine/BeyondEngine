#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_RAY3_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_RAY3_H__INCLUDE
class CVec3;
class CPlane;

class CRay3
{   
public:
    CRay3();
    ~CRay3();
    CRay3& operator = (const CRay3 &rhs);
    void FromPointAndDirection(const CVec3& point, const CVec3& direction);
    bool IntersectPlane(const CPlane& plane, float& fIntersectDistance) const;
    bool IntersectSphere(const CVec3& sphereCenter, float fSphereRadius, float& fIntersectLength);
    bool IntersectTriangle(const CVec3& pos1, const CVec3& pos2, const CVec3& pos3, CVec3& out) const;
    bool IntersectTriangleBothSide(const CVec3& pos1, const CVec3& pos2, const CVec3& pos3, CVec3& out) const;
    const CVec3& GetStartPos() const;
    const CVec3& GetDirection() const;

private:
    CVec3 m_start;
    CVec3 m_dir;
};
#endif