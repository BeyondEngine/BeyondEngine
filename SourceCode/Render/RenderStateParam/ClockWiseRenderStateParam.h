#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_CLOCKWISERENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_CLOCKWISERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CClockWiseRenderStateParam : public CRenderStateParamBase
{
public:
    enum EClockWiseType
    {
        eCWT_CCW = 0x0901, //GL_CCW
        eCWT_CW = 0x0900, //GL_CW

        eCWT_Count = 2,
        eCWT_Force32Bit = 0xFFFFFFFF
    };

public:
    CClockWiseRenderStateParam();
    virtual ~CClockWiseRenderStateParam();

    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    void SetClockWiseType( EClockWiseType type );
    EClockWiseType GetClockWiseType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    virtual bool operator!=( const CRenderStateParamBase& other ) const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EClockWiseType m_nValue;
};
#endif