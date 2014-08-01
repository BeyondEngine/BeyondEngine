#ifndef BEYOND_ENGINE_RENDER_GRIDRENDEROBJECT_H__INCLUDE
#define BEYOND_ENGINE_RENDER_GRIDRENDEROBJECT_H__INCLUDE

#include "Scene/Node3D.h"

class CRenderBatch;
class CMaterial;

class CGridRenderObject : public CNode3D
{
public:
    CGridRenderObject();
    virtual ~CGridRenderObject();

    virtual void DoRender() override;
    void GetGridStartPos(int& x, int& y) const;
    size_t GetGridWidth() const;
    size_t GetGridHeight() const;
    float GetGridDistance() const;
    float GetLineWidth() const;
    void SetGridStartPos(int x, int y);
    void SetGridWidth(size_t uWidth);
    void SetGridHeight(size_t uHeight);
    void SetGridDistance(float fDis);
    void SetLineWidth(float fLineWidth);
    void SetPositiveLineLength(float fLineLength);

    void SetLineColor(const CColor& color);
    void SetAxisXColor(const CColor& color);
    void SetAxisZColor(const CColor& color);

private:
    void InitGridData();
    void CreateVertexData();

private:
    int m_nStartGridPosX;
    int m_nStartGridPosZ;
    size_t m_uGridWidth;
    size_t m_uGridHeight;
    float m_fGridDistance;
    float m_fLineWidth;
    float m_fPositiveLineLength;
    CColor m_lineColor;
    CColor m_axisXColor;
    CColor m_axisZColor;
    CRenderBatch *m_pGridBatch;
    SharePtr<CMaterial> m_pMaterial;
};

#endif