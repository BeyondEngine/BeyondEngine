#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_CULLMODERENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_CULLMODERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CCullModeRenderStateParam : public CRenderStateParamBase
{
public:
    enum ECullModeType
    {
        eCMT_FRONT = 0x0404, //GL_FRONT
        eCMT_BACK = 0x0405, //GL_BACK
        eCMT_FRONT_AND_BACK = 0x0408, //GL_FRONT_AND_BACK

        eCMT_Count = 4, 
        eCMT_Force32Bit = 0xFFFFFFFF
    };
public:
    CCullModeRenderStateParam();
    virtual ~CCullModeRenderStateParam();

    virtual void Apply() override;

    virtual ERenderStateParamType GetRenderStateType() const override;

    void SetCullModeType( ECullModeType type );
    ECullModeType GetCullModeType() const;

    virtual bool operator==(const CRenderStateParamBase& other) const override;

    virtual bool operator!=(const CRenderStateParamBase& other) const override;

    virtual CRenderStateParamBase* Clone() override;
private:
    ECullModeType m_nValue;
};

#endif