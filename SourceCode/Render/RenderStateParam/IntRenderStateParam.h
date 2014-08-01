#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_INTRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_INTRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"



class CIntRenderStateParam : public CRenderStateParamBase
{
public:
    enum EIntStateParam
    {
        eISP_ClearStencil,
        eISP_StencilReference,
        eISP_StencilValueMask,

        eISP_Count = 3,
        eISP_Force32Bit = 0xFFFFFFFF
    };
public:
    CIntRenderStateParam();
    virtual ~CIntRenderStateParam();

    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    virtual bool operator!=( const CRenderStateParamBase& other ) const;

    void SetType( EIntStateParam type );
    EIntStateParam GetType() const;

    void SetValue( int value );
    int GetValue() const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EIntStateParam m_type;
    int m_nValue;
};
#endif