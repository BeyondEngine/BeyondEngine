#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_SHADEMODERENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_SHADEMODERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CShadeModeRenderStateParam : public CRenderStateParamBase
{
public:
    enum EShadeModeType
    {
        eSMT_FLAT_SHADE = 0x1D00, //GL_FLAT
        eSMT_SMOOTH_SHADE = 0x1D01, //GL_SMOOTH

        eSMT_Count = 2,
        eSMT_Force32Bit = 0xFFFFFFFF
    };

public:
    CShadeModeRenderStateParam();
    virtual ~CShadeModeRenderStateParam();

    virtual void Apply() override;

    virtual ERenderStateParamType GetRenderStateType() const override;

    virtual bool operator==(const CRenderStateParamBase& other) const override;

    virtual bool operator!=(const CRenderStateParamBase& other) const override;

    void SetValue( EShadeModeType value );
    EShadeModeType GetValue() const;

    virtual CRenderStateParamBase* Clone() override;
private:
    EShadeModeType m_nValue;
};
#endif