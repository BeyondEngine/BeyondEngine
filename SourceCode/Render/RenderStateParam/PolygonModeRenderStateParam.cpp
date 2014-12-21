#include "stdafx.h"
#include "PolygonModeRenderStateParam.h"
#include "Render/Renderer.h"

CPolygonModeRenderStateParam::CPolygonModeRenderStateParam()
: m_frontValue(ePMT_FILL)
, m_backValue(ePMT_FILL)
{
}

CPolygonModeRenderStateParam::~CPolygonModeRenderStateParam()
{

}


void CPolygonModeRenderStateParam::Apply()
{
    // TODO: disable this function in DEVELOP_VERSION so we can set the polygon render mode without impacting from CMaterial::Use.
#ifndef DEVELOP_VERSION
    CRenderer::GetInstance()->PolygonMode(m_frontValue, m_backValue);
#endif
}

ERenderStateParamType CPolygonModeRenderStateParam::GetRenderStateType() const
{
    return eRSPT_PolygonMode;
}

void CPolygonModeRenderStateParam::SetValue(EPolygonModeType frontType, EPolygonModeType backType)
{
    m_frontValue = frontType;
    m_backValue = backType;
}

void CPolygonModeRenderStateParam::GetValue(EPolygonModeType& frontType, EPolygonModeType& backType) const
{
    frontType = m_frontValue;
    backType = m_backValue;
}

bool CPolygonModeRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRSPT_PolygonMode );
    CPolygonModeRenderStateParam* pOther = ( CPolygonModeRenderStateParam* )&other;
    return (m_frontValue == pOther->m_frontValue && m_backValue == pOther->m_backValue);
}

bool CPolygonModeRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRSPT_PolygonMode );
    return !(*this == other);
}

CRenderStateParamBase* CPolygonModeRenderStateParam::Clone()
{
    CPolygonModeRenderStateParam* pState = new CPolygonModeRenderStateParam();
    pState->m_frontValue = m_frontValue;
    pState->m_backValue = m_backValue;
    return pState;
}
