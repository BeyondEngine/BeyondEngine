#include "stdafx.h"
#include "CullModeRenderStateParam.h"
#include "Render/Renderer.h"

CCullModeRenderStateParam::CCullModeRenderStateParam()
    : m_nValue(eCMT_FRONT_AND_BACK)
{
}

CCullModeRenderStateParam::~CCullModeRenderStateParam()
{

}


void CCullModeRenderStateParam::Apply()
{
    CRenderer::GetInstance()->CullFace(m_nValue);
}

ERenderState CCullModeRenderStateParam::GetRenderStateType() const
{
    return eRS_CullMode;
}

void CCullModeRenderStateParam::SetCullModeType( ECullModeType type )
{
    m_nValue = type;
}

CCullModeRenderStateParam::ECullModeType CCullModeRenderStateParam::GetCullModeType() const
{
    return m_nValue;
}

bool CCullModeRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_CullMode );
    CCullModeRenderStateParam* pOther = (CCullModeRenderStateParam*)&other;
    return m_nValue == pOther->m_nValue;
}

bool CCullModeRenderStateParam::operator!=( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_CullMode );
    CCullModeRenderStateParam* pOther = (CCullModeRenderStateParam*)&other;
    return m_nValue != pOther->m_nValue;
}

CRenderStateParamBase* CCullModeRenderStateParam::Clone()
{
    CCullModeRenderStateParam* pState = new CCullModeRenderStateParam();
    pState->m_nValue = m_nValue;
    return pState;
}
