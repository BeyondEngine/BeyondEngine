#include "stdafx.h"
#include "ShadeModeRenderStateParam.h"
#include "Render/Renderer.h"

CShadeModeRenderStateParam::CShadeModeRenderStateParam()
    : m_nValue(eSMT_SMOOTH_SHADE)
{
}

CShadeModeRenderStateParam::~CShadeModeRenderStateParam()
{

}


void CShadeModeRenderStateParam::Apply()
{
#ifndef GL_ES_VERSION_2_0
    CRenderer::GetInstance()->ShadeModel(m_nValue);
#endif
}

ERenderState CShadeModeRenderStateParam::GetRenderStateType() const
{
    return eRS_ShadeMode;
}

void CShadeModeRenderStateParam::SetValue( EShadeModeType value )
{
    m_nValue = value;
}

CShadeModeRenderStateParam::EShadeModeType CShadeModeRenderStateParam::GetValue() const
{
    return m_nValue;
}

bool CShadeModeRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_ShadeMode );
    CShadeModeRenderStateParam* pOther = ( CShadeModeRenderStateParam* )&other;
    return m_nValue == pOther->m_nValue;
}

bool CShadeModeRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_ShadeMode );
    CShadeModeRenderStateParam* pOther = ( CShadeModeRenderStateParam* )&other;
    return m_nValue != pOther->m_nValue;
}

CRenderStateParamBase* CShadeModeRenderStateParam::Clone()
{
    CShadeModeRenderStateParam* pState = new CShadeModeRenderStateParam();
    pState->m_nValue = m_nValue;
    return pState;
}
