#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_POLYGONMODERENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_POLYGONMODERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CPolygonModeRenderStateParam : public CRenderStateParamBase
{
public:
    enum EPolygonModeType
    {
        ePMT_POINTS = 0x1B00, //GL_POINT
        ePMT_LINES = 0x1B01, //GL_LINE
        ePMT_FILL = 0x1B02, //GL_FILL

        ePMT_Count = 3,
        ePMT_Force32Bit = 0xFFFFFFFF
    };

public:
    CPolygonModeRenderStateParam();
    virtual ~CPolygonModeRenderStateParam();

    virtual void Apply() override;

    virtual ERenderStateParamType GetRenderStateType() const override;

    virtual bool operator==(const CRenderStateParamBase& other) const override;

    virtual bool operator!=(const CRenderStateParamBase& other) const override;

    void SetValue(EPolygonModeType frontType, EPolygonModeType backType);
    void GetValue(EPolygonModeType& frontType, EPolygonModeType& backType) const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EPolygonModeType m_frontValue;
    EPolygonModeType m_backValue;
};
#endif