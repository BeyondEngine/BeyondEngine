#include "stdafx.h"
#include "FloatRenderStateParam.h"
#include "Render/Renderer.h"
#include "Render/RenderState.h"

CFloatRenderStateParam::CFloatRenderStateParam()
    : m_type(eFSP_ClearDepth)
    , m_fValue(0.0f)
{
}

CFloatRenderStateParam::~CFloatRenderStateParam()
{

}


void CFloatRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    switch (m_type)
    {
    case eFSP_ClearDepth:
        pRender->ClearDepth(m_fValue);
        break;
    case eFSP_LineWidth:
        pRender->LineWidth(m_fValue);
        break;
    case eFSP_PointSize:
#ifndef GL_ES_VERSION_2_0
        pRender->PointSize(m_fValue);
#endif
        break;
    default:
        BEATS_ASSERT(false, _T("Unkonwn type of CFloatRenderStateParam"));
        break;
    }
}

ERenderStateParamType CFloatRenderStateParam::GetRenderStateType() const
{
    return eRSPT_FloatMode;
}

CFloatRenderStateParam::EFloatStateParam CFloatRenderStateParam::GetFloatParamType() const
{
    return m_type;
}

void CFloatRenderStateParam::SetFloatParamType( EFloatStateParam type )
{
    m_type = type;
}

void CFloatRenderStateParam::SetValue( float value )
{
    m_fValue = value;
}

float CFloatRenderStateParam::GetValue() const
{
    return m_fValue;
}

bool CFloatRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRSPT_FloatMode );
    CFloatRenderStateParam* pOther = ( CFloatRenderStateParam* )&other;
    return ( m_fValue == pOther->m_fValue && m_type == pOther->m_type );
}

bool CFloatRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRSPT_FloatMode );
    CFloatRenderStateParam* pOther = ( CFloatRenderStateParam* )&other;
    return ( m_fValue != pOther->m_fValue || m_type != pOther->m_type );
}

CRenderStateParamBase* CFloatRenderStateParam::Clone()
{
    CFloatRenderStateParam* pState = new CFloatRenderStateParam();
    pState->m_fValue = m_fValue;
    pState->m_type = m_type;
    return pState;
}
