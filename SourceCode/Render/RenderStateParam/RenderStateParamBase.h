#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE


enum ERenderState
{
    eRS_StencilMode = 0,
    eRS_ShadeMode,
    eRS_CullMode,
    eRS_ClockWiseMode,
    eRS_BlendMode,
    eRS_BlendEquationMode,
    eRS_UnitMode,
    eRS_PolygonMode,
    eRS_IntMode,
    eRS_FuncMode,
    eRS_FloatMode,
    eRS_BoolMode,
    eRS_RectMode,
};

class CRenderStateParamBase
{
public:

    CRenderStateParamBase();

    virtual ERenderState GetRenderStateType() const = 0;

    virtual void Apply() = 0;

    virtual bool operator==( const CRenderStateParamBase& other ) const = 0;

    virtual bool operator!=( const CRenderStateParamBase& other ) const = 0;

    virtual CRenderStateParamBase* Clone() = 0;
};

#endif