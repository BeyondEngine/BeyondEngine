#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_FLOATRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_FLOATRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CFloatRenderStateParam : public CRenderStateParamBase
{
public:
    enum EFloatStateParam
    {
        eFSP_ClearDepth,
        eFSP_DepthFar,
        eFSP_DepthNear,
        eFSP_LineWidth,
        eFSP_PointSize,

        eFSP_Count,
        eFSP_Force32Bit = 0xFFFFFFFF
    };

public:
    CFloatRenderStateParam();
    virtual ~CFloatRenderStateParam();
    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    virtual bool operator!=( const CRenderStateParamBase& other ) const;

    void SetFloatParamType( EFloatStateParam type );
    EFloatStateParam GetFloatParamType() const;

    void SetValue( float value );
    float GetValue() const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EFloatStateParam m_type;
    float m_fValue;
};

#endif