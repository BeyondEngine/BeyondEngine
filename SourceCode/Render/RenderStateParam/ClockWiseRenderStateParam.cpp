#include "stdafx.h"
#include "ClockWiseRenderStateParam.h"
#include "Render/Renderer.h"

CClockWiseRenderStateParam::CClockWiseRenderStateParam()
    : m_nValue(eCWT_CCW)
{
}

CClockWiseRenderStateParam::~CClockWiseRenderStateParam()
{

}


void CClockWiseRenderStateParam::Apply()
{
    CRenderer::GetInstance()->FrontFace(m_nValue);
}

ERenderState CClockWiseRenderStateParam::GetRenderStateType() const
{
    return eRS_ClockWiseMode;
}

void CClockWiseRenderStateParam::SetClockWiseType( EClockWiseType type )
{
    m_nValue = type;
}

CClockWiseRenderStateParam::EClockWiseType CClockWiseRenderStateParam::GetClockWiseType() const
{
    return m_nValue;
}

bool CClockWiseRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_ClockWiseMode );
    CClockWiseRenderStateParam* pOther = (CClockWiseRenderStateParam*)&other;
    return m_nValue == pOther->m_nValue;
}

bool CClockWiseRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_ClockWiseMode );
    CClockWiseRenderStateParam* pOther = (CClockWiseRenderStateParam*)&other;
    return m_nValue != pOther->m_nValue;
}

CRenderStateParamBase* CClockWiseRenderStateParam::Clone()
{
    CClockWiseRenderStateParam* pState = new CClockWiseRenderStateParam();
    pState->m_nValue = m_nValue;
    return pState;
}
