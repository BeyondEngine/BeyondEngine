#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE

enum ERenderStateParamType
{
    eRSPT_StencilMode = 0,
    eRSPT_ShadeMode,
    eRSPT_CullMode,
    eRSPT_ClockWiseMode,
    eRSPT_BlendMode,
    eRSPT_BlendEquationMode,
    eRSPT_UnitMode,
    eRSPT_PolygonMode,
    eRSPT_IntMode,
    eRSPT_FuncMode,
    eRSPT_FloatMode,
    eRSPT_BoolMode,

    eRSPT_Count,
    eRSPT_Force32Bit = 0xFFFFFFFF
};

class CRenderStateParamBase
{
public:
    CRenderStateParamBase();
    virtual ~CRenderStateParamBase();

    virtual ERenderStateParamType GetRenderStateType() const = 0;

    virtual void Apply() = 0;

    virtual bool operator==( const CRenderStateParamBase& other ) const = 0;

    virtual bool operator!=( const CRenderStateParamBase& other ) const = 0;

    virtual CRenderStateParamBase* Clone() = 0;
};

#endif