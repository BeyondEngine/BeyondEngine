#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_UINTRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_UINTRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CUintRenderStateParam : public CRenderStateParamBase
{
public:
    enum EUintStateParam
    {
        eUSP_BlendColor,
        eUSP_ClearColor,

        eUSP_Count = 2,
        eUSP_Force32Bit =0xFFFFFFFF
    };

public:
    CUintRenderStateParam();
    virtual ~CUintRenderStateParam();

    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    virtual bool operator!=( const CRenderStateParamBase& other ) const;

    EUintStateParam GetType( ) const;
    void SetType( const EUintStateParam& type );

    const CColor& GetValue( ) const;
    void SetValue( const CColor& value );

    virtual CRenderStateParamBase* Clone() override;
private:
    EUintStateParam m_type;
    CColor m_uValue;
};

#endif