#include "stdafx.h"
#include "AlphaFunctionRenderStateParam.h"
#include "render/Renderer.h"

CAlphaFunctionRenderParam::CAlphaFunctionRenderParam()
    : m_func(eFT_ALWAYS)
    , m_ref(0.0f)
{
    SetType(eFSP_AlphaFunction); 
}

CAlphaFunctionRenderParam::~CAlphaFunctionRenderParam()
{

}

void CAlphaFunctionRenderParam::Apply()
{
    BEATS_ASSERT( GetType() == eFSP_AlphaFunction );
#ifndef GL_ES_VERSION_2_0
    CRenderer::GetInstance()->AlphaFunc( m_func, m_ref );
#endif
}


void CAlphaFunctionRenderParam::SetFunc( EFunctionType func )
{
    m_func = func;
}

void CAlphaFunctionRenderParam::SetRef( float ref )
{
    m_ref = ref;
}

float CAlphaFunctionRenderParam::GetRef() const
{
    return m_ref;
}

CFunctionRenderStateParam::EFunctionType CAlphaFunctionRenderParam::GetFunc() const
{
     return m_func;
}
