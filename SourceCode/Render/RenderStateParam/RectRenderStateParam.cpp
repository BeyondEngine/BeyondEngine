#include "stdafx.h"
#include "RectRenderStateParam.h"
#include "Render/Renderer.h"

CRectRenderStateParam::CRectRenderStateParam()
    : m_type(eRSP_VIEWPORT)
{
    kmVec4Fill( &m_value , 0.0f, 0.0f, 0.0f, 1.0f );
}

CRectRenderStateParam::~CRectRenderStateParam()
{

}


void CRectRenderStateParam::SetType(ERectStateParam type)
{
    m_type = type;
}

CRectRenderStateParam::ERectStateParam CRectRenderStateParam::GetType() const
{
    return m_type;
}

void CRectRenderStateParam::SetValue(kmScalar x, kmScalar y, kmScalar width, kmScalar height)
{
    kmVec4Fill(&m_value, x, y, width, height);
}

kmVec4 CRectRenderStateParam::GetValue() const
{
    return m_value;
}

void CRectRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    switch (m_type)
    {
    case eRSP_VIEWPORT:
        pRender->Viewport((GLsizei)m_value.x, (GLsizei)m_value.y, (GLsizei)m_value.z, (GLsizei)m_value.w);
        break;
    case eRSP_SCISSOR:
        pRender->SetScissorRect((GLsizei)m_value.x, (GLsizei)m_value.y, (GLsizei)m_value.z, (GLsizei)m_value.w);
        break;
    }
}

ERenderState CRectRenderStateParam::GetRenderStateType() const
{
    return eRS_RectMode;
}

bool CRectRenderStateParam::operator == (const CRenderStateParamBase &other) const
{
    BEATS_ASSERT(other.GetRenderStateType() == eRS_RectMode);
    const CRectRenderStateParam &otherParam = static_cast<const CRectRenderStateParam &>(other);
    kmVec4 otherRect = otherParam.GetValue();
    return otherRect.x == m_value.x && otherRect.y == m_value.y 
        && otherRect.z == m_value.z && otherRect.w == m_value.w;
}

bool CRectRenderStateParam::operator != (const CRenderStateParamBase &other) const
{
    return !(*this == other);
}

CRenderStateParamBase* CRectRenderStateParam::Clone()
{
    CRectRenderStateParam* pState = new CRectRenderStateParam();
    pState->m_type = m_type;
    pState->m_value = m_value;
    return pState;
}
