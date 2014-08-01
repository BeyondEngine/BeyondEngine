#include "stdafx.h"
#include "BoolRenderStateParam.h"
#include "Render/Renderer.h"

CBoolRenderStateParam::CBoolRenderStateParam()
    : m_bValue(true)
    , m_type(eBSP_Blend)
{
}

CBoolRenderStateParam::~CBoolRenderStateParam()
{

}

void CBoolRenderStateParam::SetType(EBoolStateParam type)
{
    m_type = type;
}

CBoolRenderStateParam::EBoolStateParam CBoolRenderStateParam::GetType() const
{
    return m_type;
}

void CBoolRenderStateParam::SetValue(bool bValue)
{
    m_bValue = bValue;
}

bool CBoolRenderStateParam::GetValue() const
{
    return m_bValue;
}

void CBoolRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    m_bValue ? pRender->EnableGL(m_type) : pRender->DisableGL(m_type);
}

ERenderState CBoolRenderStateParam::GetRenderStateType() const
{
    return eRS_BoolMode;
}

bool CBoolRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_BoolMode );
    CBoolRenderStateParam* pOther = (CBoolRenderStateParam*)&other;
    return ( m_type == pOther->m_type && m_bValue == pOther->m_bValue );
}

bool CBoolRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_BoolMode );
    CBoolRenderStateParam* pOther = (CBoolRenderStateParam*)&other;
    return ( m_type != pOther->m_type || m_bValue != pOther->m_bValue );
}

CRenderStateParamBase* CBoolRenderStateParam::Clone()
{
    CBoolRenderStateParam* pSatate = new CBoolRenderStateParam();
    pSatate->m_bValue = m_bValue;
    pSatate->m_type = m_type;
    return pSatate;
}
