#include "stdafx.h"
#include "RenderState.h"
#include "Renderer.h"
#include "Render/RenderStateParam/BoolRenderStateParam.h"
#include "Render/RenderStateParam/UintRenderStateParam.h"
#include "Render/RenderStateParam/IntRenderStateParam.h"
#include "Render/RenderStateParam/FloatRenderStateParam.h"
#include "Render/RenderStateParam/FunctionRenderStateParam.h"
#include "Render/RenderStateParam/BlendRenderStateParam.h"
#include "Render/RenderStateParam/BlendEquationRenderStateParam.h"
#include "Render/RenderStateParam/CullModeRenderStateParam.h"
#include "Render/RenderStateParam/ClockWiseRenderStateParam.h"
#include "Render/RenderStateParam/PolygonModeRenderStateParam.h"
#include "Render/RenderStateParam/ShadeModeRenderStateParam.h"
#include "Render/RenderStateParam/StencilRenderStateParam.h"
#include "Render/RenderStateParam/AlphaFunctionRenderStateParam.h"
#include "RenderManager.h"

CRenderState::CRenderState()
    : m_bDepthMark(true)
    , m_uCurrShaderProgram(0)
    , m_uCurrActiveTexture(0)
    , m_uVAO(0)
    , m_uVBO(0)
    , m_uEBO(0)
{
}

CRenderState::~CRenderState()
{
    for (auto iter = m_renderStateParamMap.begin(); iter != m_renderStateParamMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            BEATS_SAFE_DELETE(subIter->second);
        }
    }
    m_renderStateParamMap.clear();
}

void CRenderState::SetBoolState(CBoolRenderStateParam::EBoolStateParam state, bool bEnable)
{
    CBoolRenderStateParam* pParam = (CBoolRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_BoolMode, state);
    if ( NULL == pParam )
    {
        pParam = new CBoolRenderStateParam();
        pParam->SetType(state);
        m_renderStateParamMap[eRSPT_BoolMode][state] = pParam;
    }
    pParam->SetValue(bEnable);
}

bool CRenderState::GetBoolState(CBoolRenderStateParam::EBoolStateParam state)
{
    bool bRet = false;
    CBoolRenderStateParam* pParam = (CBoolRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_BoolMode, state);
    if ( NULL == pParam )
    {
        bRet = CRenderer::GetInstance()->IsEnable( state );
        pParam = new CBoolRenderStateParam();
        pParam->SetType(state);
        pParam->SetValue(bRet);
        m_renderStateParamMap[eRSPT_BoolMode][state] = pParam;
    }
    else
    {
        bRet = pParam->GetValue();
    }
    return bRet;
}

void CRenderState::SetDepthMask(bool bWriteable)
{
    m_bDepthMark = bWriteable;
}

bool CRenderState::GetDepthMask() const
{
    return m_bDepthMark;
}

void CRenderState::SetScissorTest(bool bEnable)
{
    m_bScissorTest = bEnable;
}

bool CRenderState::GetScissorTest() const
{
    return m_bScissorTest;
}

void CRenderState::SetActiveTexture(uint32_t uChannel)
{
    BEATS_ASSERT(uChannel < 8);
    m_uCurrActiveTexture = uChannel;
}

uint32_t CRenderState::GetActiveTexture() const
{
    return m_uCurrActiveTexture;
}

uint32_t CRenderState::GetCurrBindingTexture() const
{
    uint32_t uRet = 0;
    BEATS_ASSERT(m_uCurrActiveTexture < 8)
    auto iter = m_bindingTexture.find((unsigned char)m_uCurrActiveTexture);
    if (iter != m_bindingTexture.end())
    {
        uRet = iter->second;
    }
    return uRet;
}

void CRenderState::SetCurrBindingTexture(uint32_t uTextureId)
{
    BEATS_ASSERT(m_uCurrActiveTexture < 8)
    m_bindingTexture[(unsigned char)m_uCurrActiveTexture] = uTextureId;
}

std::map<unsigned char, uint32_t>& CRenderState::GetBindingTextureMap()
{
    return m_bindingTexture;
}

void CRenderState::SetBlendSrcFactor(GLenum src)
{
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_BlendMode, 0);
    if ( NULL == pParam )
    {
        pParam = new CBlendRenderStateParam();
        m_renderStateParamMap[eRSPT_BlendMode][0] = pParam;
    }
    pParam->SetSrcFactor( (CBlendRenderStateParam::EBlendParamType)src );
}

void CRenderState::SetBlendTargetFactor(GLenum target)
{
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_BlendMode, 0);
    if ( NULL == pParam )
    {
        pParam = new CBlendRenderStateParam();
        m_renderStateParamMap[eRSPT_BlendMode][0] = pParam;
    }
    pParam->SetTargetFactor( (CBlendRenderStateParam::EBlendParamType)target );
}

#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
GLenum CRenderState::GetBlendSrcFactor()
{
    GLint retEnum = 0;
    CBlendRenderStateParam* pParam = (CBlendRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_BlendMode, 0);
    if (NULL == pParam)
    {
        CRenderer::GetInstance()->GetIntegerV(GL_BLEND_SRC, &retEnum);
        pParam = new CBlendRenderStateParam();
        pParam->SetSrcFactor((CBlendRenderStateParam::EBlendParamType)retEnum);
        m_renderStateParamMap[eRSPT_BlendMode][0] = pParam;
    }
    else
    {
        retEnum = (GLint)pParam->GetSrcFactor();
    }
    return retEnum;
}

GLenum CRenderState::GetBlendTargetFactor()
{
    GLint retEnum = 0;
    CBlendRenderStateParam* pParam = (CBlendRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_BlendMode, 0);
    if (NULL == pParam)
    {
        CRenderer::GetInstance()->GetIntegerV(GL_BLEND_DST, &retEnum);
        pParam = new CBlendRenderStateParam();
        pParam->SetTargetFactor((CBlendRenderStateParam::EBlendParamType)retEnum);
        m_renderStateParamMap[eRSPT_BlendMode][0] = pParam;
    }
    else
    {
        retEnum = (GLenum)pParam->GetTargetFactor();
    }
    return retEnum;
}

void CRenderState::SetAlphaFunc(GLenum func)
{
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_AlphaFunction);
    if (NULL == pParam)
    {
        pParam = new CAlphaFunctionRenderParam();
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_AlphaFunction] = pParam;
    }
    pParam->SetFunc((CFunctionRenderStateParam::EFunctionType)func);
}

GLenum CRenderState::GetAlphaFunc()
{
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_AlphaFunction);
    if (NULL == pParam)
    {
        GLint func;
        CRenderer::GetInstance()->GetIntegerV(GL_ALPHA_TEST_FUNC, &func);
        pParam = new CAlphaFunctionRenderParam();
        pParam->SetFunc((CFunctionRenderStateParam::EFunctionType)func);
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_AlphaFunction] = pParam;
    }
    return pParam->GetFunc();
}

void CRenderState::SetAlphaRef(float fRef)
{
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_AlphaFunction);
    if (NULL == pParam)
    {
        pParam = new CAlphaFunctionRenderParam();
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_AlphaFunction] = pParam;
    }
    pParam->SetRef(fRef);
}

float CRenderState::GetAlphaRef()
{
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_AlphaFunction);
    if (NULL == pParam)
    {
        float ref = 0.0f;
        CRenderer::GetInstance()->GetFloatV(GL_ALPHA_TEST_REF, &ref);
        pParam = new CAlphaFunctionRenderParam();
        pParam->SetRef(ref);
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_AlphaFunction] = pParam;
    }
    return pParam->GetRef();
}

void CRenderState::SetPointSize(float fPointSize)
{
    CFloatRenderStateParam* pParam = (CFloatRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_FloatMode, CFloatRenderStateParam::eFSP_PointSize);
    if (NULL == pParam)
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType(CFloatRenderStateParam::eFSP_PointSize);
        m_renderStateParamMap[eRSPT_FloatMode][CFloatRenderStateParam::eFSP_PointSize] = pParam;
    }
    pParam->SetValue(fPointSize);
}

float CRenderState::GetPointSize()
{
    float fPointSize = 0;
    CFloatRenderStateParam* pParam = (CFloatRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_FloatMode, CFloatRenderStateParam::eFSP_PointSize);
    if (NULL == pParam)
    {
        CRenderer::GetInstance()->GetFloatV(GL_POINT_SIZE, &fPointSize);
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType(CFloatRenderStateParam::eFSP_PointSize);
        m_renderStateParamMap[eRSPT_FloatMode][CFloatRenderStateParam::eFSP_PointSize] = pParam;
        pParam->SetValue(fPointSize);
    }
    return pParam->GetValue();
}

void CRenderState::SetShadeModel(GLenum shadeModel)
{
    CShadeModeRenderStateParam* pParam = (CShadeModeRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_ShadeMode, 0);
    if (NULL == pParam)
    {
        pParam = new CShadeModeRenderStateParam();
        m_renderStateParamMap[eRSPT_ShadeMode][0] = pParam;
    }
    pParam->SetValue((CShadeModeRenderStateParam::EShadeModeType)shadeModel);
}

GLenum CRenderState::GetShadeModel()
{
    CShadeModeRenderStateParam* pParam = (CShadeModeRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_ShadeMode, 0);
    if (NULL == pParam)
    {
        GLint retEnum = 0;
        CRenderer::GetInstance()->GetIntegerV(GL_SHADE_MODEL, &retEnum);
        pParam = new CShadeModeRenderStateParam();
        pParam->SetValue((CShadeModeRenderStateParam::EShadeModeType)retEnum);
        m_renderStateParamMap[eRSPT_ShadeMode][0] = pParam;
    }
    return pParam->GetValue();
}

void CRenderState::SetPolygonMode(GLenum frontMode, GLenum backMode)
{
    CPolygonModeRenderStateParam* pParam = (CPolygonModeRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_PolygonMode, 0);
    if (NULL == pParam)
    {
        pParam = new CPolygonModeRenderStateParam();
        m_renderStateParamMap[eRSPT_PolygonMode][0] = pParam;
    }
    pParam->SetValue((CPolygonModeRenderStateParam::EPolygonModeType)frontMode, (CPolygonModeRenderStateParam::EPolygonModeType)backMode);
}

void CRenderState::GetPolygonMode(CPolygonModeRenderStateParam::EPolygonModeType& frontType, CPolygonModeRenderStateParam::EPolygonModeType& backType)
{
    CPolygonModeRenderStateParam* pParam = (CPolygonModeRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_PolygonMode, 0);
    if (NULL == pParam)
    {
        GLint value[2];
        CRenderer::GetInstance()->GetIntegerV(GL_POLYGON_MODE, value);
        pParam = new CPolygonModeRenderStateParam();
        m_renderStateParamMap[eRSPT_PolygonMode][0] = pParam;
        pParam->SetValue((CPolygonModeRenderStateParam::EPolygonModeType)value[0], (CPolygonModeRenderStateParam::EPolygonModeType)value[1]);
    }
    pParam->GetValue(frontType, backType);
}
#endif

void CRenderState::SetBlendEquation(GLenum func)
{
    CBlendEquationRenderStateParam* pParam = ( CBlendEquationRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_BlendEquationMode, 0 );
    if ( NULL == pParam )
    {
        pParam = new CBlendEquationRenderStateParam();
        m_renderStateParamMap[eRSPT_BlendEquationMode][0] = pParam;
    }
    pParam->SetBlendEquationType( (CBlendEquationRenderStateParam::EBlendEquationType)func );
}

GLenum CRenderState::GetBlendEquation()
{
    GLint retEnum = 0;
    CBlendEquationRenderStateParam* pParam = ( CBlendEquationRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_BlendEquationMode, 0 );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_BLEND_EQUATION_RGB, &retEnum );
        pParam = new CBlendEquationRenderStateParam();
        m_renderStateParamMap[eRSPT_BlendEquationMode][0] = pParam;
        pParam->SetBlendEquationType( (CBlendEquationRenderStateParam::EBlendEquationType)retEnum );
    }
    return (GLint)pParam->GetBlendEquationType();
}

void CRenderState::SetShaderProgram(GLuint program)
{
    BEATS_ASSERT(program != m_uCurrShaderProgram);
    m_uCurrShaderProgram = program;
}

GLuint CRenderState::GetShaderProgram()const
{
    return  m_uCurrShaderProgram;
}

void CRenderState::SetFrontFace(GLenum frontFace)
{
    CClockWiseRenderStateParam* pParam = ( CClockWiseRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_ClockWiseMode, 0 );
    if ( NULL == pParam )
    {
        pParam = new CClockWiseRenderStateParam();
        m_renderStateParamMap[eRSPT_ClockWiseMode][0] = pParam;
    }
    pParam->SetClockWiseType( (CClockWiseRenderStateParam::EClockWiseType)frontFace );
}

GLenum CRenderState::GetFrontFace()
{
    GLint retEunm = 0;
    CClockWiseRenderStateParam* pParam = ( CClockWiseRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_ClockWiseMode, 0 );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_FRONT_FACE, &retEunm );
        pParam = new CClockWiseRenderStateParam();
        m_renderStateParamMap[eRSPT_ClockWiseMode][0] = pParam;
        pParam->SetClockWiseType((CClockWiseRenderStateParam::EClockWiseType)retEunm);
    }
    return (GLint)pParam->GetClockWiseType();
}

void CRenderState::SetCullFace(GLenum cullFace)
{
    CCullModeRenderStateParam* pParam = ( CCullModeRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_CullMode, 0 );
    if ( NULL == pParam )
    {
        pParam = new CCullModeRenderStateParam();
        m_renderStateParamMap[eRSPT_CullMode][0] = pParam;
    }
    pParam->SetCullModeType( (CCullModeRenderStateParam::ECullModeType)cullFace );
}

GLenum CRenderState::GetCullFace()
{
    CCullModeRenderStateParam* pParam = ( CCullModeRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_CullMode, 0 );
    if ( NULL == pParam )
    {
        GLint retEnum = 0;
        CRenderer::GetInstance()->GetIntegerV( GL_CULL_FACE_MODE, &retEnum);
        pParam = new CCullModeRenderStateParam();
        pParam->SetCullModeType((CCullModeRenderStateParam::ECullModeType)retEnum);
        m_renderStateParamMap[eRSPT_CullMode][0] = pParam;
    }
    return pParam->GetCullModeType();
}

void CRenderState::SetLineWidth(float fLineWidth)
{
    CFloatRenderStateParam* pParam = (CFloatRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_FloatMode, CFloatRenderStateParam::eFSP_LineWidth);
    if ( NULL == pParam )
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_LineWidth );
        m_renderStateParamMap[eRSPT_FloatMode][CFloatRenderStateParam::eFSP_LineWidth] = pParam;
    }
    pParam->SetValue( fLineWidth );
}

float CRenderState::GetLineWidth()
{
    CFloatRenderStateParam* pParam = (CFloatRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_FloatMode, CFloatRenderStateParam::eFSP_LineWidth);
    if ( NULL == pParam )
    {
        float fLineWidth = 0;
        CRenderer::GetInstance()->GetFloatV(GL_LINE_WIDTH, &fLineWidth);
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_LineWidth);
        pParam->SetValue(fLineWidth);
        m_renderStateParamMap[eRSPT_FloatMode][CFloatRenderStateParam::eFSP_LineWidth] = pParam;
    }
    return pParam->GetValue();
}

void CRenderState::SetDepthFunc(GLenum func)
{
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_DepthFunction );
    if ( NULL == pParam )
    {
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_DepthFunction );
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_DepthFunction] = pParam;
    }
    pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)func );
}

GLenum CRenderState::GetDepthFunc()
{
    CFunctionRenderStateParam* pParam = (CFunctionRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_DepthFunction);
    if ( NULL == pParam )
    {
        GLint retEnum = 0;
        CRenderer::GetInstance()->GetIntegerV( GL_DEPTH_FUNC, &retEnum );
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_DepthFunction );
        pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)retEnum );
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_DepthFunction] = pParam;
    }
    return pParam->GetValue();
}

void CRenderState::SetStencilFunc(GLenum func)
{
    CFunctionRenderStateParam* pParam = (CFunctionRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_StencilFunction);
    if ( NULL == pParam )
    {
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_StencilFunction );
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_StencilFunction] = pParam;
    }
    pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)func );
}

GLenum CRenderState::GetStencilFunc()
{
    CFunctionRenderStateParam* pParam = (CFunctionRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_FuncMode, CFunctionRenderStateParam::eFSP_StencilFunction);
    if ( NULL == pParam )
    {
        GLint retEnum = 0;
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_FUNC, &retEnum );
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_StencilFunction );
        pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)retEnum );
        m_renderStateParamMap[eRSPT_FuncMode][CFunctionRenderStateParam::eFSP_StencilFunction] = pParam;
    }
    return pParam->GetValue();
}

void CRenderState::SetStencilReference(GLint nRef)
{
    CIntRenderStateParam* pParam = (CIntRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_IntMode, CIntRenderStateParam::eISP_StencilReference);
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilReference );
        m_renderStateParamMap[eRSPT_IntMode][CIntRenderStateParam::eISP_StencilReference] = pParam;
    }
    pParam->SetValue( nRef );
}

GLint CRenderState::GetStencilReference()
{
    CIntRenderStateParam* pParam = (CIntRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_IntMode, CIntRenderStateParam::eISP_StencilReference);
    if ( NULL == pParam )
    {
        GLint retInt = 0;
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_REF, &retInt );
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilReference );
        pParam->SetValue( retInt );
        m_renderStateParamMap[eRSPT_IntMode][CIntRenderStateParam::eISP_StencilReference] = pParam;
    }
    return pParam->GetValue();
}

void CRenderState::SetStencilValueMask(GLint nValueMask)
{
    CIntRenderStateParam* pParam = (CIntRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_IntMode, CIntRenderStateParam::eISP_StencilValueMask);
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilValueMask );
        m_renderStateParamMap[eRSPT_IntMode][CIntRenderStateParam::eISP_StencilValueMask] = pParam;
    }
    pParam->SetValue( nValueMask );
}

GLint CRenderState::GetStencilValueMask()
{
    CIntRenderStateParam* pParam = (CIntRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_IntMode, CIntRenderStateParam::eISP_StencilValueMask);
    if ( NULL == pParam )
    {
        GLint retInt = 0;
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_VALUE_MASK, &retInt );
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilValueMask );
        pParam->SetValue( retInt );
        m_renderStateParamMap[eRSPT_IntMode][CIntRenderStateParam::eISP_StencilValueMask] = pParam;
    }
    return pParam->GetValue();
}

void CRenderState::SetClearStencil(GLint nClearValue)
{
    CIntRenderStateParam* pParam = (CIntRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_IntMode, CIntRenderStateParam::eISP_ClearStencil);
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_ClearStencil );
        m_renderStateParamMap[eRSPT_IntMode][CIntRenderStateParam::eISP_ClearStencil] = pParam;
    }
    pParam->SetValue( nClearValue );
}

GLint CRenderState::GetClearStencil()
{
    CIntRenderStateParam* pParam = (CIntRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_IntMode, CIntRenderStateParam::eISP_ClearStencil);
    if ( NULL == pParam )
    {
        GLint retInt = 0;
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_CLEAR_VALUE, &retInt );
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_ClearStencil );
        pParam->SetValue( retInt );
        m_renderStateParamMap[eRSPT_IntMode][CIntRenderStateParam::eISP_ClearStencil] = pParam;
    }
    return pParam->GetValue();
}

void CRenderState::SetScissorRect(float x, float y, float width, float height)
{
    m_scissorRect.Fill(x, y, width, height);
}

const CVec4& CRenderState::GetScissorRect() const
{
    return m_scissorRect;
}

void CRenderState::SetViewport(float x, float y, float width, float height)
{
    m_viewport.Fill(x, y, width, height);
}

const CVec4& CRenderState::GetViewport() const
{
    return m_viewport;
}

void CRenderState::Restore()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    if (pRenderer->UseProgram(m_uCurrShaderProgram) && m_uCurrShaderProgram != 0)
    {
        CRenderManager::GetInstance()->InitDefaultShaderUniform();
    }
    for (auto iter = m_renderStateParamMap.begin(); iter != m_renderStateParamMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            subIter->second->Apply();
            BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        }
    }
    pRenderer->DepthMask(m_bDepthMark);
}

CRenderStateParamBase* CRenderState::GetRenderStateParamBasePtr( ERenderStateParamType state, int nSubType ) const
{
    CRenderStateParamBase* pParam = NULL;
    auto iter = m_renderStateParamMap.find(state);
    if (iter != m_renderStateParamMap.end())
    {
        auto subIter = iter->second.find(nSubType);
        if (subIter != iter->second.end())
        {
            pParam = subIter->second;
        }
    }
    return pParam;
}

void CRenderState::SetColor( CUintRenderStateParam::EUintStateParam type, GLclampf r, GLclampf g, GLclampf b, GLclampf a )
{
    CUintRenderStateParam* pParam = (CUintRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_UnitMode, type);
    if ( NULL == pParam )
    {
        pParam = new CUintRenderStateParam();
        m_renderStateParamMap[eRSPT_UnitMode][type] = pParam;
    }
    CColor value( r, g, b, a );
    pParam->SetValue( value );
}

void CRenderState::GetColor( CUintRenderStateParam::EUintStateParam type , GLclampf& r, GLclampf& g, GLclampf& b, GLclampf& a )
{
    CUintRenderStateParam* pParam = (CUintRenderStateParam*)GetRenderStateParamBasePtr(eRSPT_UnitMode, type);
    if ( NULL == pParam )
    {
        pParam = new CUintRenderStateParam();
        pParam->SetType(type);
        float value[4];
        switch ( type )
        {
        case CUintRenderStateParam::eUSP_ClearColor:
            {
                CRenderer::GetInstance()->GetFloatV(GL_COLOR_CLEAR_VALUE, value);
            }
            break;
        case CUintRenderStateParam::eUSP_BlendColor:
            {
                CRenderer::GetInstance()->GetFloatV(GL_BLEND_COLOR, value);
            }
            break;
        default:
            BEATS_ASSERT( "Invalid type of GetColor" );
            break;
        }
        r = value[ 0 ];
        g = value[ 1 ];
        b = value[ 2 ];
        a = value[ 3 ];
        pParam->SetValue( CColor( r, g, b, a ) );
        m_renderStateParamMap[eRSPT_UnitMode][type] = pParam;
    }
    else
    {
        const CColor& color = pParam->GetValue();
        r = (GLclampf)color.r;
        g = (GLclampf)color.g;
        b = (GLclampf)color.b;
        a = (GLclampf)color.a;
    }
}

void CRenderState::SetStencilOp( GLenum fail, GLenum zFail, GLenum zPass )
{
    CStencilRenderStateParam* pParam = ( CStencilRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_StencilMode, 0 );
    if ( NULL == pParam )
    {
        pParam = new CStencilRenderStateParam();
        m_renderStateParamMap[eRSPT_StencilMode][0] = pParam;
    }
    pParam->SetValue( ( CStencilRenderStateParam::EStencilType )fail,
        ( CStencilRenderStateParam::EStencilType )zFail,
        ( CStencilRenderStateParam::EStencilType )zPass );
}

void CRenderState::GetStencilOp( GLenum& fail, GLenum& zFail, GLenum& zPass )
{
    CStencilRenderStateParam* pParam = ( CStencilRenderStateParam* )GetRenderStateParamBasePtr( eRSPT_StencilMode, 0 );
    if ( NULL == pParam )
    {
        GLint failTemp, zFailTemp, zPassTemp;
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_FAIL, &failTemp );
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_PASS_DEPTH_FAIL, &zFailTemp );
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_PASS_DEPTH_PASS, &zPassTemp );
        fail = failTemp;
        zFail = zFailTemp;
        zPass = zPassTemp;
        pParam = new CStencilRenderStateParam();
        m_renderStateParamMap[eRSPT_StencilMode][0] = pParam;
        pParam->SetValue( (CStencilRenderStateParam::EStencilType)fail,
            (CStencilRenderStateParam::EStencilType)zFail, (CStencilRenderStateParam::EStencilType)zPass );
    }
    else
    {
        pParam->GetValue( ( CStencilRenderStateParam::EStencilType& )fail,
            ( CStencilRenderStateParam::EStencilType& )zFail,
            ( CStencilRenderStateParam::EStencilType& )zPass );
    }
}

CRenderState* CRenderState::Clone()
{
    CRenderState* pRenderState = new CRenderState();
    pRenderState->m_bDepthMark = m_bDepthMark;
    pRenderState->m_uCurrShaderProgram = m_uCurrShaderProgram;
    pRenderState->m_uCurrActiveTexture = m_uCurrActiveTexture;
    for (auto iter = m_renderStateParamMap.begin(); iter != m_renderStateParamMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            CRenderStateParamBase* pState = subIter->second->Clone();
            pRenderState->m_renderStateParamMap[iter->first][subIter->first] = pState;
        }
    }
    return pRenderState;
}

uint32_t CRenderState::GetBindingVAO() const
{
    return m_uVAO;
}

uint32_t CRenderState::GetBindingVBO() const
{
    return m_uVBO;
}

uint32_t CRenderState::GetBindingEBO() const
{
    return m_uEBO;
}

void CRenderState::SetBindingVAO(uint32_t uVAO)
{
    m_uVAO = uVAO;
}

void CRenderState::SetBindingVBO(uint32_t uVBO)
{
    m_uVBO = uVBO;
}

void CRenderState::SetBindingEBO(uint32_t uEBO)
{
    m_uEBO = uEBO;
}
