#ifndef BEYOND_ENGINE_RENDER_SHAPERENDERER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SHAPERENDERER_H__INCLUDE

struct SCurveData;
class CAABBBox;
class CShapeRenderer
{
    BEATS_DECLARE_SINGLETON(CShapeRenderer);
public:
    void DrawCircle(const CMat4& mat, float fRadius, CColor color, bool bSolid, float fRate = 1.0f) const;
    void DrawCone(const CMat4& mat, float fBottomRadius, float fTopRadius, float fHeight, CColor coneColor, CColor bottomColor, CColor topColor, bool bSolid) const;
    void DrawSphere(const CVec3& pos, float fRadius, float fElliptical, CColor color, uint32_t uGridCnt = 30, bool bWireFrame = false) const;
    void DrawBox(const CMat4& mat, float fX, float fY, float fZ, CColor color, bool bSolid);
    void DrawCurve(const SCurveData& curveData, CColor color);
    void DrawAABB(const CAABBBox& aabb, CColor color);
};

#endif