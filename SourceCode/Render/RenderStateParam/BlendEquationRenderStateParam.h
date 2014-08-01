#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_BLENDEQUATIONRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_BLENDEQUATIONRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CBlendEquationRenderStateParam : public CRenderStateParamBase
{
public:
    enum EBlendEquationType
    {
        eBET_ADD = 0x8006, //GL_FUNC_ADD
        eBET_SUBTRACT = 0x800A,//GL_FUNC_SUBTRACT
        eBET_REV_SUBTRACT = 0x800B, //GL_FUNC_REVERSE_SUBTRACT
        eBET_MIN = 0x8007,//GL_MIN
        eBET_MAX = 0x8008,//GL_MAX

        eBET_Count = 5,
        eBET_Force32Bit = 0xFFFFFFFF
    };

public:
    CBlendEquationRenderStateParam();
    virtual ~CBlendEquationRenderStateParam();

    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    virtual bool operator!=( const CRenderStateParamBase& other ) const;

    void SetBlendEquationType( EBlendEquationType type );
    EBlendEquationType GetBlendEquationType() const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EBlendEquationType m_nValue;
};
#endif