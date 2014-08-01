#include "stdafx.h"
#include "PolygonModeRenderStateParam.h"
#include "Render/Renderer.h"

CPolygonModeRenderStateParam::CPolygonModeRenderStateParam()
    : m_type(ePMSP_PolyFrontMode)
    , m_nValue(ePMT_FILL)
{
}

CPolygonModeRenderStateParam::~CPolygonModeRenderStateParam()
{

}


void CPolygonModeRenderStateParam::Apply()
{
    CRenderer::GetInstance()->PolygonMode(m_type, m_nValue);
}

ERenderState CPolygonModeRenderStateParam::GetRenderStateType() const
{
    return eRS_PolygonMode;
}

void CPolygonModeRenderStateParam::SetValue( EPolygonModeStateParam type, EPolygonModeType value )
{
    m_type = type;
    m_nValue = value;
}

void CPolygonModeRenderStateParam::GetValue( EPolygonModeStateParam& type, EPolygonModeType& value ) const
{
    type = m_type;
    value = m_nValue;
}

bool CPolygonModeRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_PolygonMode );
    CPolygonModeRenderStateParam* pOther = ( CPolygonModeRenderStateParam* )&other;
    return ( m_type == pOther->m_type && m_nValue == pOther->m_nValue );
}

bool CPolygonModeRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_PolygonMode );
    CPolygonModeRenderStateParam* pOther = ( CPolygonModeRenderStateParam* )&other;
    return ( m_type != pOther->m_type || m_nValue != pOther->m_nValue );
}

CRenderStateParamBase* CPolygonModeRenderStateParam::Clone()
{
    CPolygonModeRenderStateParam* pState = new CPolygonModeRenderStateParam();
    pState->m_nValue = m_nValue;
    pState->m_type = m_type;
    return pState;
}
