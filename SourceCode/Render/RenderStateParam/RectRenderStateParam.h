#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_RECTRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_RECTRENDERSTATEPARAM_H__INCLUDE

#include "RenderStateParamBase.h"


class CRectRenderStateParam : public CRenderStateParamBase
{
public:
    enum ERectStateParam
    {
        eRSP_VIEWPORT,
        eRSP_SCISSOR,

        eBSP_Count,
        eBSP_Force32Bit = 0xFFFFFFFF
    };

public:
    CRectRenderStateParam();
    virtual ~CRectRenderStateParam();
    void SetType(ERectStateParam type);
    ERectStateParam GetType() const;
    void SetValue(kmScalar x, kmScalar y, kmScalar width, kmScalar height);
    kmVec4 GetValue() const;

    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const override;

    virtual bool operator ==( const CRenderStateParamBase& other ) const override;

    virtual bool operator !=( const CRenderStateParamBase& other ) const override;

    virtual CRenderStateParamBase* Clone() override;

private:
    ERectStateParam m_type;
    kmVec4 m_value;
};

#endif