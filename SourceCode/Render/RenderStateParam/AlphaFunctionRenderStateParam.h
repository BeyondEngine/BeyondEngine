#ifndef BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_ALPHAFUNCTIONRENDERSTATEPARAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATEPARAM_ALPHAFUNCTIONRENDERSTATEPARAM_H__INCLUDE

#include "FunctionRenderStateParam.h"

class CAlphaFunctionRenderParam : public CFunctionRenderStateParam
{
public:
    CAlphaFunctionRenderParam();
    virtual ~CAlphaFunctionRenderParam();

    virtual void Apply()override;
    void SetFunc( EFunctionType func );
    EFunctionType GetFunc() const;
    void SetRef( float ref );
    float GetRef() const;

private:
    EFunctionType m_func;
    float m_ref;
};
#endif//RENDER_RENDERSTATEPARAM_ALPHAFUNCTIONRENDERSTATEPARAM_H__INCLUDE