#ifndef BEYOND_ENGINE_RENDER_COORDINATERENDEROBJECT_H__INCLUDE
#define BEYOND_ENGINE_RENDER_COORDINATERENDEROBJECT_H__INCLUDE

#include "Scene/Node3D.h"

class CRenderBatch;
class CMaterial;

enum ECoordinateTranslateMode
{
    eCTM_X,
    eCTM_Y,
    eCTM_Z,
    eCTM_XY,
    eCTM_YZ,
    eCTM_XZ,

    eCTM_Count,
    eCTM_Force32Bit = 0xFFFFFFFF
};

enum class ECoordinateRenderType
{
    eCRT_Local,
    eCRT_Parent,
    eCRT_World,

    eCRT_Count
};

class CCoordinateRenderObject : public CNode3D
{
public:
    CCoordinateRenderObject();
    virtual ~CCoordinateRenderObject();

    void SetRenderNode(CNode* pNode);
    CNode* GetRenderNode() const;
    void RenderMatrix(const CMat4& mat, bool b3DOr2D);
    bool RefreshTranslateMode(float x, float y);
    bool GetOperateFlag() const;
    void SetOperateFlag(bool bFlag);
    ECoordinateTranslateMode GetTranslateMode() const;
    void TranslateObject(float x, float y);
    virtual void DoRender() override;
    CColor GetAxisColorX() const;
    CColor GetAxisColorY() const;
    CColor GetAxisColorZ() const;
    CColor GetAxisSelectColor() const;
    CColor GetPanelSelectColor() const;
    float GetAxisLengthFactor() const;
    float GetConeHeightFactor() const;
    float GetConeBottomColorFactor() const;
    float GetConeAngle() const;
    float GetTranslatePanelStartPosRate() const;
    void SetAxisColorX(CColor color);
    void SetAxisColorY(CColor color);
    void SetAxisColorZ(CColor color);
    void SetAxisSelectColor(CColor color);
    void SetPanelSelectColor(CColor color);
    void SetAxisLengthFactor(float factor);
    void SetConeHeightFactor(float factor);
    void SetConeBottomColorFactor(float factor);
    void SetConeAngle(float angle);
    void SetTranslatePanelStartPosRate(float rate);
    void SetRenderType(ECoordinateRenderType type);

private:
    ECoordinateRenderType m_renderType = ECoordinateRenderType::eCRT_Local;
    ECoordinateTranslateMode m_translateMode;
    CNode* m_pNode;
    CVec3 m_originPos;
    bool m_bCanOperate = false;
    float m_fAxisLength = 0;
    float m_fPanelSize = 0;
    CVec3 m_pickPosOffset;
    CColor m_axisColorX = 0xFF0000FF;
    CColor m_axisColorY = 0x00FF00FF;
    CColor m_axisColorZ = 0x0000FFFF;
    CColor m_axisSelectColor = 0xFFFF00FF;
    CColor  m_panelSelectColor = 0xFFFF0099;
    float m_fAxisLengthFactor = 0.35f;// The axis length is always take fAxisLengthFactor of view port height.
    float m_fConeHeightFactor = 0.2f;
    float m_fConeBottomColorFactor = 0.5f;
    float m_fConeAngle = 15.f;
    float m_fTranslatePanelStartPosRate = 0.4f;
};

#endif