#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_BLENDRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_BLENDRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CBlendRenderStateParam : public CRenderStateParamBase
{
public:
    enum EBlendParamType
    {
        eBPT_ZERO = 0, //GL_ZERO
        eBPT_ONE = 1, //GL_ONE
        eBPT_SRC_COLOR = 0x0300,//GL_SRC_COLOR
        eBPT_INV_SRC_COLOR = 0x0301, //GL_ONE_MINUS_SRC_COLOR
        eBPT_DEST_COLOR = 0x0306, //GL_DST_COLOR
        eBPT_INV_DEST_COLOR = 0x0307, //GL_ONE_MINUS_DST_COLOR
        eBPT_SRC_ALPHA = 0x0302, //GL_SRC_ALPHA
        eBPT_INV_SRC_ALPHA = 0x0303, //GL_ONE_MINUS_SRC_ALPHA
        eBPT_DEST_ALPHA = 0x0304, //GL_DST_ALPHA
        eBPT_INV_DEST_ALPHA = 0x0305, //GL_ONE_MINUS_DST_ALPHA
        eBPT_CONST_COLOR = 0x8001, //GL_CONSTANT_COLOR
        eBPT_INV_CONST_COLOR = 0x8002, //GL_ONE_MINUS_CONSTANT_COLOR
        eBPT_CONST_ALPHA = 0x8003, //GL_CONSTANT_ALPHA
        eBPT_INV_CONST_ALPHA = 0x8004, //GL_ONE_MINUS_CONSTANT_ALPHA
        eBPT_SRC_ALPHA_SATURATE = 0x0308, //GL_SRC_ALPHA_SATURATE

        eBPT_Count = 15,
        eBPT_Force32Bit = 0xFFFFFFFF
    };

public:
    CBlendRenderStateParam();
    virtual ~CBlendRenderStateParam();

    virtual void Apply() override;
    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    virtual bool operator!=( const CRenderStateParamBase& other ) const;

    void SetSrcFactor( EBlendParamType src );
    EBlendParamType GetSrcFactor() const;

    void SetTargetFactor( EBlendParamType des );
    EBlendParamType GetTargetFactor() const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EBlendParamType m_source;
    EBlendParamType m_dest;
};
#endif