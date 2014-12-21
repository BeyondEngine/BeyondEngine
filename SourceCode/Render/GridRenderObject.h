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
    uint32_t GetGridWidth() const;
    uint32_t GetGridHeight() const;
    float GetPositiveLineLength() const;
    float GetGridDistance() const;
    float GetLineWidth() const;
    void SetGridStartPos(int x, int y);
    void SetGridWidth(uint32_t uWidth);
    void SetGridHeight(uint32_t uHeight);
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
    uint32_t m_uGridWidth;
    uint32_t m_uGridHeight;
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