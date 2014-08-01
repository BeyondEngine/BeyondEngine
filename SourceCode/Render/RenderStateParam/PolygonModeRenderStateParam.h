#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_POLYGONMODERENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_POLYGONMODERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CPolygonModeRenderStateParam : public CRenderStateParamBase
{
public:
    enum EPolygonModeStateParam
    {
        ePMSP_PolyFrontMode = 0x0404,//GL_FRONT
        ePMSP_PolyBackMode,

        eSMSP_Count,
        eSMSP_Force32Bit = 0xFFFFFFFF,
    };

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

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    virtual bool operator!=( const CRenderStateParamBase& other ) const;

    void SetValue( EPolygonModeStateParam type, EPolygonModeType value );
    void GetValue( EPolygonModeStateParam& type, EPolygonModeType& value ) const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EPolygonModeStateParam m_type;
    EPolygonModeType m_nValue;
};
#endif