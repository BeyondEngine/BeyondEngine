#include "stdafx.h"
#include "BlendEquationRenderStateParam.h"
#include "Render/Renderer.h"

CBlendEquationRenderStateParam::CBlendEquationRenderStateParam()
    : m_nValue(EBlendEquationType::eBET_ADD)
{
}

CBlendEquationRenderStateParam::~CBlendEquationRenderStateParam()
{

}

void CBlendEquationRenderStateParam::Apply()
{
    CRenderer::GetInstance()->BlendEquation(m_nValue);
}

ERenderState CBlendEquationRenderStateParam::GetRenderStateType() const
{
    return eRS_BlendEquationMode;
}

void CBlendEquationRenderStateParam::SetBlendEquationType( EBlendEquationType type )
{
    m_nValue = type;
}

CBlendEquationRenderStateParam::EBlendEquationType CBlendEquationRenderStateParam::GetBlendEquationType() const
{
    return m_nValue;
}

bool CBlendEquationRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_BlendEquationMode );
    CBlendEquationRenderStateParam* pOther = (CBlendEquationRenderStateParam*)&other;
    return m_nValue == pOther->m_nValue;
}

bool CBlendEquationRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_BlendEquationMode );
    CBlendEquationRenderStateParam* pOther = (CBlendEquationRenderStateParam*)&other;
    return m_nValue != pOther->m_nValue;
}

CRenderStateParamBase* CBlendEquationRenderStateParam::Clone()
{
    CBlendEquationRenderStateParam* pState = new CBlendEquationRenderStateParam();
    pState->m_nValue = m_nValue;
    return pState;
}
