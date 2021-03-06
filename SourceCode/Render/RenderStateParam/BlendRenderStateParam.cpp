﻿#include "stdafx.h"
#include "BlendRenderStateParam.h"
#include "Render/Renderer.h"

CBlendRenderStateParam::CBlendRenderStateParam()
    : m_source(eBPT_SRC_ALPHA)
    , m_dest(eBPT_ONE_MINUS_SRC_ALPHA)
{
}

CBlendRenderStateParam::~CBlendRenderStateParam()
{

}


void CBlendRenderStateParam::Apply()
{
    CRenderer::GetInstance()->EnableGL(CBoolRenderStateParam::EBoolStateParam::eBSP_Blend);
    CRenderer::GetInstance()->BlendFunc(m_source, m_dest);
}

ERenderStateParamType CBlendRenderStateParam::GetRenderStateType() const
{
    return eRSPT_BlendMode;
}

void CBlendRenderStateParam::SetSrcFactor( EBlendParamType src )
{
    m_source = src;
}

void CBlendRenderStateParam::SetTargetFactor( EBlendParamType des )
{
    m_dest = des;
}

CBlendRenderStateParam::EBlendParamType CBlendRenderStateParam::GetSrcFactor() const
{
    return m_source;
}

CBlendRenderStateParam::EBlendParamType CBlendRenderStateParam::GetTargetFactor() const
{
    return m_dest;
}

bool CBlendRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRSPT_BlendMode );
    CBlendRenderStateParam* pOther = (CBlendRenderStateParam*)&other;
    return ( m_source == pOther->m_source && m_dest == pOther->m_dest );
}

bool CBlendRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRSPT_BlendMode );
    CBlendRenderStateParam* pOther = (CBlendRenderStateParam*)&other;
    return ( m_source != pOther->m_source || m_dest != pOther->m_dest );
}

CRenderStateParamBase* CBlendRenderStateParam::Clone()
{
    CBlendRenderStateParam* pState = new CBlendRenderStateParam();
    pState->m_dest = m_dest;
    pState->m_source = m_source;
    return pState;
}

