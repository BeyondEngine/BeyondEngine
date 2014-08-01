#include "stdafx.h"
#include "UintRenderStateParam.h"
#include "Render/Renderer.h"

CUintRenderStateParam::CUintRenderStateParam()
    : m_type(eUSP_ClearColor)
    , m_uValue(0x00000000)
{
}

CUintRenderStateParam::~CUintRenderStateParam()
{

}

void CUintRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    CColor colorTmp(m_uValue);

    switch (m_type)
    {
    case eUSP_BlendColor:
        pRender->BlendColor((float)colorTmp.r / 0xFF, (float)colorTmp.g / 0xFF, (float)colorTmp.b / 0xFF, (float)colorTmp.a / 0xFF);
        break;
    case eUSP_ClearColor:
        pRender->ClearColor((float)colorTmp.r / 0xFF, (float)colorTmp.g / 0xFF, (float)colorTmp.b / 0xFF, (float)colorTmp.a / 0xFF);
        break;
    default:
        BEATS_ASSERT(false, _T("Unkonwn type of CUintRenderStateParam"));
        break;
    }
}

ERenderState CUintRenderStateParam::GetRenderStateType() const
{
    return eRS_UnitMode;
}

bool CUintRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_UnitMode );
    CUintRenderStateParam* pOther = ( CUintRenderStateParam* )&other;
    return ( m_type == pOther->m_type && m_uValue == pOther->m_uValue );
}

CUintRenderStateParam::EUintStateParam CUintRenderStateParam::GetType() const
{
    return m_type;
}

void CUintRenderStateParam::SetType( const EUintStateParam& type )
{
    m_type = type;
}

const CColor& CUintRenderStateParam::GetValue() const
{
    return m_uValue;
}

void CUintRenderStateParam::SetValue( const CColor& value )
{
    m_uValue.a = value.a;
    m_uValue.r = value.r;
    m_uValue.g = value.g;
    m_uValue.b = value.b;
}

bool CUintRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_UnitMode );
    CUintRenderStateParam* pOther = ( CUintRenderStateParam* )&other;
    return ( m_type != pOther->m_type || m_uValue != pOther->m_uValue );
}

CRenderStateParamBase* CUintRenderStateParam::Clone()
{
    CUintRenderStateParam* pState = new CUintRenderStateParam();
    pState->m_type = m_type;
    pState->m_uValue = m_uValue;
    return pState;
}

