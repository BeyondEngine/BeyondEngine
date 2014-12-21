#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_PLANE_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_PLANE_H__INCLUDE
class CVec3;
enum class EPlaneSide
{
    ePS_OnPlane,
    ePS_Positive,
    ePS_Negative,

    ePS_Count,
    ePS_Force32bit = -1
};
class CPlane
{
public:

    explicit CPlane(float a = 0.f, float b = 0.f, float c = 0.f, float d = 0.f);
    void Fill(float a, float b, float c, float d);
    void FromPointNormal(const CVec3& point, const CVec3& normal);
    const CVec3& GetNormal() const;
    float GetDistance() const;
    void FromPoints(const CVec3& point1, const CVec3& point2, const CVec3& point3);
    void FromPointNormalAndDistance(const CVec3& normal, float d);
    EPlaneSide PositionTest(const CVec3& pos) const;

private:
    float m_fDistance = 0;
    CVec3 m_normal;
};
#endif