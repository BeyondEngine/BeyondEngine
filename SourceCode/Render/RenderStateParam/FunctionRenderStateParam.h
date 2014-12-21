#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_FUNCTIONRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_FUNCTIONRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CFunctionRenderStateParam : public CRenderStateParamBase
{
public:
    enum EFunctionStateParam
    {
        eFSP_DepthFunction,
        eFSP_StencilFunction,
        eFSP_AlphaFunction,

        eFSP_Count,
        eFSP_Force32Bit = 0xFFFFFFFF,
    };

    enum EFunctionType
    {
        eFT_NEVER = 0x0200, //GL_NEVER
        eFT_LESS = 0x0201, //GL_LESS
        eFT_EQUAL = 0x0202, //GL_EQUAL
        eFT_LESS_EQUAL = 0x0203, //GL_LEQUAL
        eFT_GREATER = 0x0204, //GL_GREATER
        eFT_NOT_EQUAL = 0x0205, //GL_NOTEQUAL
        eFT_GREATER_EQUAL = 0x0206, //GL_GEQUAL
        eFT_ALWAYS = 0x0207, //GL_ALWAYS

        eFT_Count = 8, 
        eFT_Force32Bit = 0xFFFFFFFF
    };

public:
    CFunctionRenderStateParam();
    virtual ~CFunctionRenderStateParam();

    virtual void Apply() override;

    virtual ERenderStateParamType GetRenderStateType() const override;

    virtual bool operator==(const CRenderStateParamBase& other) const override;

    virtual bool operator!=(const CRenderStateParamBase& other) const override;

    void SetType( EFunctionStateParam type );
    EFunctionStateParam GetType() const;

    void SetValue( EFunctionType value );
    EFunctionType GetValue() const;

    virtual CRenderStateParamBase* Clone() override;

private:
    EFunctionStateParam m_type;
    EFunctionType m_nValue;
};

#endif