#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_BOOLRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_BOOLRENDERSTATEPARAM_H__INCLUDE

#include "RenderStateParamBase.h"


class CBoolRenderStateParam : public CRenderStateParamBase
{
public:
    enum EBoolStateParam
    {
        eBSP_Blend = 0x0BE2,//GL_BLEND
        eBSP_DepthTest = 0x0B71,//GL_DEPTH_TEST
        eBSP_CullFace = 0xB44,//GL_CULL_FACE
        eBSP_StencilTest = 0x0B90,//GL_STENCIL_TEST

        eBSP_Count = 5,
        eBSP_Force32Bit = 0xFFFFFFFF
    };


public:
    CBoolRenderStateParam();
    virtual ~CBoolRenderStateParam();

    void SetType(EBoolStateParam type);
    EBoolStateParam GetType() const;
    void SetValue(bool bValue);
    bool GetValue() const;

    virtual void Apply() override;

    virtual ERenderStateParamType GetRenderStateType() const override;

    virtual bool operator==(const CRenderStateParamBase& other) const override;

    virtual bool operator!=(const CRenderStateParamBase& other) const override;

    virtual CRenderStateParamBase* Clone() override;
private:
    bool m_bValue;
    EBoolStateParam m_type;
};

#endif