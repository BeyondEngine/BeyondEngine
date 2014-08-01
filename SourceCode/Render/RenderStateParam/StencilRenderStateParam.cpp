#include "stdafx.h"
#include "StencilRenderStateParam.h"
#include "Render/Renderer.h"

CStencilRenderStateParam::CStencilRenderStateParam()
    : m_nFail(eST_KEEP)
    , m_nZFail(eST_KEEP)
    , m_nZPass(eST_REPLACE)
{
}

CStencilRenderStateParam::~CStencilRenderStateParam()
{

}

void CStencilRenderStateParam::Apply()
{
    CRenderer::GetInstance()->StencilOp(m_nFail, m_nZFail, m_nZPass);
}

ERenderState CStencilRenderStateParam::GetRenderStateType() const
{
    return eRS_StencilMode;
}

bool CStencilRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_StencilMode );
    CStencilRenderStateParam* pOther = ( CStencilRenderStateParam* )&other;
    return ( m_nZFail == pOther->m_nZFail && m_nFail == pOther->m_nFail && m_nZPass == pOther->m_nZPass );
}

void CStencilRenderStateParam::SetValue( EStencilType fail, EStencilType zFali, EStencilType zPass )
{
    m_nFail = fail;
    m_nZFail = zFali;
    m_nZPass = zPass;
}

void CStencilRenderStateParam::GetValue( EStencilType& fail, EStencilType& zFali, EStencilType& zPass ) const
{
    fail = m_nFail;
    zFali = m_nZFail;
    zPass = m_nZPass;
}

bool CStencilRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_StencilMode );
    CStencilRenderStateParam* pOther = ( CStencilRenderStateParam* )&other;
    return ( m_nZFail != pOther->m_nZFail || m_nFail != pOther->m_nFail || m_nZPass != pOther->m_nZPass );
}

CRenderStateParamBase* CStencilRenderStateParam::Clone()
{
    CStencilRenderStateParam* pState = new CStencilRenderStateParam();
    pState->m_nFail = m_nFail;
    pState->m_nZFail = m_nZFail;
    pState->m_nZPass = m_nZPass;
    return pState;
}
