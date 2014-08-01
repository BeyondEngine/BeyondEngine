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

CRenderState::CRenderState()
    :m_uCurrShaderProgram(0)
    , m_uCurrActiveTexture(0)
    , m_bDepthMark(true)
    , m_bEdgeFlag(false)
{
}

CRenderState::~CRenderState()
{
    for ( auto iter : m_pRenderStateParams )
    {
        BEATS_SAFE_DELETE( iter );
    }
    m_pRenderStateParams.clear();
    m_boolRenderStateVector.clear();
    m_intRenderStateVector.clear();
    m_funcRenderStateVector.clear();
    m_floatRenderStateVector.clear();
    m_unitRenderStateVector.clear();
}

void CRenderState::SetBoolState(CBoolRenderStateParam::EBoolStateParam state, bool bEnable)
{
    CBoolRenderStateParam* pParam = ( CBoolRenderStateParam* )GetBoolRenderStateParamPtr(state);
    if ( NULL == pParam )
    {
        pParam = new CBoolRenderStateParam();
        pParam->SetType(state);
        m_pRenderStateParams.push_back( pParam );
        m_boolRenderStateVector.push_back( pParam );
    }
    pParam->SetValue(bEnable);
}

bool CRenderState::GetBoolState(CBoolRenderStateParam::EBoolStateParam state)
{
    bool bRet = false;
    CBoolRenderStateParam* pParam = ( CBoolRenderStateParam* )GetBoolRenderStateParamPtr(state);
    if ( NULL == pParam )
    {
        bRet = CRenderer::GetInstance()->IsEnable( state );
        pParam = new CBoolRenderStateParam();
        pParam->SetType(state);
        m_pRenderStateParams.push_back( pParam );
        m_boolRenderStateVector.push_back( pParam );
        pParam->SetValue( bRet );
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

void CRenderState::SetEdgeFlag(bool bEdgeFlag)
{
    m_bEdgeFlag = bEdgeFlag;
}

bool CRenderState::GetEdgetFlag() const
{
    return m_bEdgeFlag;
}

void CRenderState::SetActiveTexture(GLenum activeTexture)
{
    m_uCurrActiveTexture = activeTexture;
}

GLenum CRenderState::GetActiveTexture() const
{
    return m_uCurrActiveTexture;
}

void CRenderState::SetBlendFuncSrcFactor(GLenum src)
{
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        pParam = new CBlendRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetSrcFactor( (CBlendRenderStateParam::EBlendParamType)src );
}

void CRenderState::SetBlendFuncTargetFactor(GLenum target)
{
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        pParam = new CBlendRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetTargetFactor( (CBlendRenderStateParam::EBlendParamType)target );
}
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
GLenum CRenderState::GetBlendSrcFactor()
{
    GLint retEnum = 0;
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_BLEND_SRC, &retEnum );
        pParam = new CBlendRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        pParam->SetSrcFactor( (CBlendRenderStateParam::EBlendParamType)retEnum );
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
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_BLEND_DST, &retEnum );
        pParam = new CBlendRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        pParam->SetTargetFactor( (CBlendRenderStateParam::EBlendParamType)retEnum );
    }
    else
    {
        retEnum = (GLenum)pParam->GetTargetFactor();
    }
    return retEnum;
}
#endif

void CRenderState::SetBlendEquation(GLenum func)
{
    CBlendEquationRenderStateParam* pParam = ( CBlendEquationRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendEquationMode );
    if ( NULL == pParam )
    {
        pParam = new CBlendEquationRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetBlendEquationType( (CBlendEquationRenderStateParam::EBlendEquationType)func );
}

GLenum CRenderState::GetBlendEquation()
{
    GLint retEnum = 0;
    CBlendEquationRenderStateParam* pParam = ( CBlendEquationRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendEquationMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_BLEND_EQUATION_RGB, &retEnum );
        pParam = new CBlendEquationRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        pParam->SetBlendEquationType( (CBlendEquationRenderStateParam::EBlendEquationType)retEnum );
    }
    else
    {
        retEnum = (GLint)pParam->GetBlendEquationType();
    }
    return retEnum;
}

void CRenderState::SetAlphaFunc(GLenum func)
{
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_AlphaFunction );
    if ( NULL == pParam )
    {
        pParam = new CAlphaFunctionRenderParam();
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    pParam->SetFunc( (CFunctionRenderStateParam::EFunctionType)func );
}
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
GLenum CRenderState::GetAlphaFunc()
{
    GLint func;
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_AlphaFunction );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_ALPHA_TEST_FUNC , &func );
        pParam = new CAlphaFunctionRenderParam();
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    else
    {
        func = pParam->GetFunc();
    }
    return func;
}
#endif

void CRenderState::SetAlphaRef(float fRef)
{
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_AlphaFunction );
    if ( NULL == pParam )
    {
        pParam = new CAlphaFunctionRenderParam();
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    pParam->SetRef( fRef );
}
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
float CRenderState::GetAlphaRef()
{
    float ref = 0.0f;
    CAlphaFunctionRenderParam* pParam = (CAlphaFunctionRenderParam*)GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_AlphaFunction );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetFloatV( GL_ALPHA_TEST_REF , &ref );
        pParam = new CAlphaFunctionRenderParam();
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    else
    {
        ref = pParam->GetRef();
    }
    return ref;
}
#endif
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
    CClockWiseRenderStateParam* pParam = ( CClockWiseRenderStateParam* )GetRenderStateParamBasePtr( eRS_ClockWiseMode );
    if ( NULL == pParam )
    {
        pParam = new CClockWiseRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetClockWiseType( (CClockWiseRenderStateParam::EClockWiseType)frontFace );
}

GLenum CRenderState::GetFrontFace()
{
    GLint retEunm = 0;
    CClockWiseRenderStateParam* pParam = ( CClockWiseRenderStateParam* )GetRenderStateParamBasePtr( eRS_ClockWiseMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_FRONT_FACE, &retEunm );
        pParam = new CClockWiseRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        pParam->SetClockWiseType( (CClockWiseRenderStateParam::EClockWiseType)retEunm );
    }
    else
    {
        retEunm = (GLint)pParam->GetClockWiseType();
    }
    
    return retEunm;
}

void CRenderState::SetCullFace(GLenum cullFace)
{
    CCullModeRenderStateParam* pParam = ( CCullModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_CullMode );
    if ( NULL == pParam )
    {
        pParam = new CCullModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetCullModeType( (CCullModeRenderStateParam::ECullModeType)cullFace );
}

GLenum CRenderState::GetCullFace()
{
    GLint retEnum = 0;
    CCullModeRenderStateParam* pParam = ( CCullModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_CullMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_CULL_FACE_MODE, &retEnum);
        pParam = new CCullModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        pParam->SetCullModeType( (CCullModeRenderStateParam::ECullModeType)retEnum );
    }
    else
    {
        retEnum = (CCullModeRenderStateParam::ECullModeType)pParam->GetCullModeType();
    }
    
    return retEnum;
}

void CRenderState::SetLineWidth(float fLineWidth)
{
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_LineWidth );
    if ( NULL == pParam )
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_LineWidth );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( fLineWidth );
}

float CRenderState::GetLineWidth()
{
    float fLineWidth = 0;
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_LineWidth );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetFloatV(GL_LINE_WIDTH, &fLineWidth);
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_LineWidth);
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
        pParam->SetValue( fLineWidth );
    }
    else
    {
        fLineWidth = pParam->GetValue();
    }
    return fLineWidth;
}

void CRenderState::SetPointSize(float fPointSize)
{
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_PointSize );
    if ( NULL == pParam )
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_PointSize );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( fPointSize );
}
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
float CRenderState::GetPointSize()
{
    float fPointSize = 0;
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_PointSize );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetFloatV(GL_POINT_SIZE, &fPointSize);
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_PointSize);
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
        pParam->SetValue( fPointSize );
    }
    else
    {
        fPointSize = pParam->GetValue();
    }
    return fPointSize;
}
#endif
void CRenderState::SetDepthNear(float fDepthNear)
{
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthNear );
    if ( NULL == pParam )
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_DepthNear );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( fDepthNear );
}

void CRenderState::SetDepthFar(float fDepthFar)
{
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthFar );
    if ( NULL == pParam )
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_DepthFar );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( fDepthFar );
}

float CRenderState::GetDepthNear()
{
    float renFloat = 0;
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthNear );
    if ( NULL == pParam )
    {
        float range[2];
        CRenderer::GetInstance()->GetFloatV(GL_DEPTH_RANGE, range );
        renFloat = range[0];
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_DepthFar );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
        pParam->SetValue( renFloat );
    }
    else
    {
        renFloat = pParam->GetValue();
    }
    return renFloat;
}

float CRenderState::GetDepthFar()
{
    float renFloat = 0;
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthFar );
    if ( NULL == pParam )
    {
        float range[2];
        CRenderer::GetInstance()->GetFloatV(GL_DEPTH_RANGE, range );
        renFloat = range[1];
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_DepthFar );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
        pParam->SetValue( renFloat );
    }
    else
    {
        renFloat = pParam->GetValue();
    }
    return renFloat;
}

void CRenderState::SetDepthFunc(GLenum func)
{
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_DepthFunction );
    if ( NULL == pParam )
    {
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_DepthFunction );
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)func );
}

GLenum CRenderState::GetDepthFunc()
{
    GLint retEnum = 0;
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_DepthFunction );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_DEPTH_FUNC, &retEnum );
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_DepthFunction );
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
        pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)retEnum );
    }
    else
    {
        retEnum = pParam->GetValue();
    }
    return retEnum;
}

void CRenderState::SetStencilFunc(GLenum func)
{
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_StencilFunction );
    if ( NULL == pParam )
    {
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_StencilFunction );
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)func );
}

GLenum CRenderState::GetStencilFunc()
{
    GLint retEnum = 0;
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_StencilFunction );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_FUNC, &retEnum );
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_StencilFunction );
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
        pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)retEnum );
    }
    else
    {
        retEnum = pParam->GetValue();
    }
    return retEnum;
}

void CRenderState::SetStencilReference(GLint nRef)
{
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilReference );
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilReference );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( nRef );
}

GLint CRenderState::GetStencilReference()
{
    GLint retInt = 0;
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilReference );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_REF, &retInt );
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilReference );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
        pParam->SetValue( retInt );
    }
    else
    {
        retInt = pParam->GetValue();
    }
    return retInt;
}

void CRenderState::SetStencilValueMask(GLint nValueMask)
{
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilValueMask );
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilValueMask );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( nValueMask );
}

GLint CRenderState::GetStencilValueMask()
{
    GLint retInt = 0;
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilValueMask );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_VALUE_MASK, &retInt );
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilValueMask );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
        pParam->SetValue( retInt );
    }
    else
    {
        retInt = pParam->GetValue();
    }
    return retInt;
}

void CRenderState::SetClearStencil(GLint nClearValue)
{
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_ClearStencil );
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_ClearStencil );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( nClearValue );
}

GLint CRenderState::GetClearStencil()
{
    GLint retInt = 0;
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_ClearStencil );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_CLEAR_VALUE, &retInt );
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_ClearStencil );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
        pParam->SetValue( retInt );
    }
    else
    {
        retInt = pParam->GetValue();
    }
    return retInt;
}

void CRenderState::SetScissorRect(kmScalar x, kmScalar y, kmScalar width, kmScalar height)
{
    CRectRenderStateParam* pParam = ( CRectRenderStateParam* )GetRectRenderStateParamPtr( CRectRenderStateParam::eRSP_SCISSOR );
    if ( NULL == pParam )
    {
        pParam = new CRectRenderStateParam();
        pParam->SetType( CRectRenderStateParam::eRSP_SCISSOR );
        m_pRenderStateParams.push_back( pParam );
        m_rectRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( x, y, width, height );
}

void CRenderState::GetScissorRect(kmVec4 &rect) const
{
    CRectRenderStateParam* pParam = (CRectRenderStateParam* )GetRectRenderStateParamPtr(CRectRenderStateParam::eRSP_SCISSOR);
    if( NULL == pParam )
    {
        CRenderer::GetInstance()->GetFloatV(GL_SCISSOR_BOX, (GLfloat *)&rect);
    }
    else
    {
        rect = pParam->GetValue();
    }
}

void CRenderState::SetShadeModel(GLenum shadeModel)
{
    CShadeModeRenderStateParam* pParam = ( CShadeModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_ShadeMode );
    if ( NULL == pParam )
    {
        pParam = new CShadeModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetValue( (CShadeModeRenderStateParam::EShadeModeType)shadeModel );
}
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
GLenum CRenderState::GetShadeModel()
{
    GLint retEnum = 0;
    CShadeModeRenderStateParam* pParam = ( CShadeModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_ShadeMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_SHADE_MODEL, &retEnum );
        pParam = new CShadeModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        pParam->SetValue( (CShadeModeRenderStateParam::EShadeModeType)retEnum );
    }
    else
    {
        retEnum = pParam->GetValue();
    }
    return retEnum;
}
#endif
void CRenderState::Restore()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->UseProgram(m_uCurrShaderProgram);
    for ( auto iter : m_pRenderStateParams )
    {
        if ( iter )
        {
            iter->Apply();
            BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        }
    }
}

CRenderStateParamBase* CRenderState::GetRenderStateParamBasePtr( ERenderState state ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_pRenderStateParams.begin();
    for ( ; iter != m_pRenderStateParams.end(); ++iter )
    {
        if ( (*iter)->GetRenderStateType() == state )
        {
            pParam = *iter;
            break;
        }
    }
    return pParam;
}

CFloatRenderStateParam* CRenderState::GetFloatRenderStateParamPtr( CFloatRenderStateParam::EFloatStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_floatRenderStateVector.begin();
    for ( ; iter != m_floatRenderStateVector.end(); ++iter )
    {
        CFloatRenderStateParam* param = (CFloatRenderStateParam*)(*iter);
        if ( param->GetFloatParamType() == type )
        {
            pParam = param;
            break;
        }
    }
    return ( CFloatRenderStateParam* )pParam;
}

CFunctionRenderStateParam* CRenderState::GetFuncRenderStateParamPtr( CFunctionRenderStateParam::EFunctionStateParam type ) const
{
     CRenderStateParamBase* pParam = NULL;
     TParamStateVectorIterConst iter = m_funcRenderStateVector.begin();
     for ( ; iter != m_funcRenderStateVector.end(); ++iter )
     {
         CFunctionRenderStateParam* param = (CFunctionRenderStateParam*)(*iter);
         if ( param->GetType() == type )
         {
             pParam = param;
             break;
         }
     }
     return (CFunctionRenderStateParam*)pParam;
}

CIntRenderStateParam* CRenderState::GetIntRenderStateParamPtr( CIntRenderStateParam::EIntStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_intRenderStateVector.begin();
    for ( ; iter != m_intRenderStateVector.end(); ++iter )
    {
        CIntRenderStateParam* param = (CIntRenderStateParam*)(*iter);
        if ( param->GetType() == type )
        {
            pParam = param;
            break;
        }
    }
    return (CIntRenderStateParam*)pParam;
}

CBoolRenderStateParam* CRenderState::GetBoolRenderStateParamPtr( CBoolRenderStateParam::EBoolStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_boolRenderStateVector.begin();
    for ( ; iter != m_boolRenderStateVector.end(); ++iter )
    {
        CBoolRenderStateParam* param = (CBoolRenderStateParam*)(*iter);
        if ( param->GetType() == type )
        {
            pParam = param;
            break;
        }
    }
    return (CBoolRenderStateParam*)pParam;
}

CRenderStateParamBase* CRenderState::GetRectRenderStateParamPtr( CRectRenderStateParam::ERectStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_rectRenderStateVector.begin();
    for ( ; iter != m_rectRenderStateVector.end(); ++iter )
    {
        CRectRenderStateParam* param = (CRectRenderStateParam*)(*iter);
        if ( param->GetType() == type )
        {
            pParam = param;
            break;
        }
    }
    return pParam;
}

void CRenderState::SetPolygonMode( GLenum face, GLenum mode )
{
    CPolygonModeRenderStateParam* pParam = ( CPolygonModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_PolygonMode );
    if ( NULL == pParam )
    {
        pParam = new CPolygonModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetValue( (CPolygonModeRenderStateParam::EPolygonModeStateParam)face,
        (CPolygonModeRenderStateParam::EPolygonModeType)mode );
}
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
void CRenderState::GetPolygonMode( GLenum& face, GLenum& mode )
{
    CPolygonModeRenderStateParam* pParam = ( CPolygonModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_PolygonMode );
    if ( NULL == pParam )
    {
        GLint value[ 2 ];
        CRenderer::GetInstance()->GetIntegerV( GL_POLYGON_MODE, value );
        face = value[ 0 ];
        mode = value[ 1 ];
        pParam = new CPolygonModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        pParam->SetValue( (CPolygonModeRenderStateParam::EPolygonModeStateParam) face,
            (CPolygonModeRenderStateParam::EPolygonModeType)mode );
    }
    else
    {
        CPolygonModeRenderStateParam::EPolygonModeStateParam tempFace;
        CPolygonModeRenderStateParam::EPolygonModeType tempMode;
        pParam->GetValue( tempFace, tempMode );
        face = tempFace;
        mode = tempMode;
    }
}
#endif
void CRenderState::SetColor( CUintRenderStateParam::EUintStateParam type, GLclampf r, GLclampf g, GLclampf b, GLclampf a )
{
    CUintRenderStateParam* pParam = ( CUintRenderStateParam* )GetUnitRenderStateParamPtr( type );
    if ( NULL == pParam )
    {
        pParam = new CUintRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        m_unitRenderStateVector.push_back( pParam );
    }
    CColor value( r, g, b, a );
    pParam->SetValue( value );
}

CUintRenderStateParam* CRenderState::GetUnitRenderStateParamPtr( CUintRenderStateParam::EUintStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_unitRenderStateVector.begin();
    for ( ; iter != m_unitRenderStateVector.end(); ++iter )
    {
        CUintRenderStateParam* param = (CUintRenderStateParam*)(*iter);
        if ( param->GetType() == type )
        {
            pParam = param;
            break;
        }
    }
    return (CUintRenderStateParam*)pParam;
}

void CRenderState::GetColor( CUintRenderStateParam::EUintStateParam type , GLclampf& r, GLclampf& g, GLclampf& b, GLclampf& a )
{
    CUintRenderStateParam* pParam = ( CUintRenderStateParam* )GetUnitRenderStateParamPtr( type );
    if ( NULL == pParam )
    {
        pParam = new CUintRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        m_unitRenderStateVector.push_back( pParam );
        float value[4];
        switch ( type )
        {
        case CUintRenderStateParam::eUSP_ClearColor:
            {
                glGetFloatv( GL_COLOR_CLEAR_VALUE, value );
                pParam->SetType( CUintRenderStateParam::eUSP_ClearColor );
            }
            break;
        case CUintRenderStateParam::eUSP_BlendColor:
            {
                glGetFloatv( GL_BLEND_COLOR, value );
                pParam->SetType( CUintRenderStateParam::eUSP_BlendColor );
            }
            break;
        default:
            {
                BEATS_ASSERT( "Invalid type of GetColor" );
            }
            break;
        }
        r = value[ 0 ];
        g = value[ 1 ];
        b = value[ 2 ];
        a = value[ 3 ];
        pParam->SetValue( CColor( r, g, b, a ) );
    }
    else
    {
        const CColor color = ( pParam->GetValue() );
        r = (GLclampf)color.r;
        g = (GLclampf)color.g;
        b = (GLclampf)color.b;
        a = (GLclampf)color.a;
    }
}

void CRenderState::SetStencilOp( GLenum fail, GLenum zFail, GLenum zPass )
{
    CStencilRenderStateParam* pParam = ( CStencilRenderStateParam* )GetRenderStateParamBasePtr( eRS_StencilMode );
    if ( NULL == pParam )
    {
        pParam = new CStencilRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetValue( ( CStencilRenderStateParam::EStencilType )fail,
        ( CStencilRenderStateParam::EStencilType )zFail,
        ( CStencilRenderStateParam::EStencilType )zPass );
}

void CRenderState::GetStencilOp( GLenum& fail, GLenum& zFail, GLenum& zPass )
{
    CStencilRenderStateParam* pParam = ( CStencilRenderStateParam* )GetRenderStateParamBasePtr( eRS_StencilMode );
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
        m_pRenderStateParams.push_back( pParam );
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

bool CRenderState::operator==( const CRenderState& other ) const
{
    return ( m_bDepthMark == other.m_bDepthMark &&
        m_bEdgeFlag == other.m_bEdgeFlag &&
        ComparePtrVector( m_pRenderStateParams, other.m_pRenderStateParams ) );
}

bool CRenderState::operator!=( const CRenderState& other ) const
{
    return ( m_bDepthMark != other.m_bDepthMark ||
        m_bEdgeFlag != other.m_bEdgeFlag ||
        !ComparePtrVector( m_pRenderStateParams, other.m_pRenderStateParams ) );
}

bool CRenderState::ComparePtrVector( const TParamStateVector & v1, const TParamStateVector & v2 ) const
{
    bool bReturn = true;
    if( v1 != v2 )
    {
        if ( v1.size() == v2.size() )
        {
            size_t size = v1.size();
            for ( size_t i = 0; i < size; ++i )
            {
                if ( v1[ i ] != v2[ i ] )
                {
                    if( *v1[ i ] != *v2[ i ] )
                    {
                        bReturn = false;
                        break;
                    }
                }
            }
        }
        else
        {
            bReturn = false;
        }
    }
    return bReturn;
}

CRenderState* CRenderState::Clone()
{
    CRenderState* renderState = new CRenderState();
    renderState->m_bDepthMark = m_bDepthMark;
    renderState->m_bEdgeFlag = m_bEdgeFlag;
    renderState->m_uCurrShaderProgram = m_uCurrShaderProgram;
    renderState->m_uCurrActiveTexture = m_uCurrActiveTexture;
    
    auto iter = m_pRenderStateParams.begin();
    for ( ; iter != m_pRenderStateParams.end(); ++iter )
    {
        CRenderStateParamBase* pState = (*iter)->Clone();
        renderState->m_pRenderStateParams.push_back( pState );
        switch ( pState->GetRenderStateType() )
        {
        case eRS_BoolMode:
            m_boolRenderStateVector.push_back( pState );
            break;
        case eRS_FuncMode:
            m_funcRenderStateVector.push_back( pState );
            break;
        case eRS_FloatMode:
            m_floatRenderStateVector.push_back( pState );
            break;
        case eRS_IntMode:
            m_intRenderStateVector.push_back( pState );
            break;
        case eRS_UnitMode:
            m_unitRenderStateVector.push_back( pState );
            break;
        case eRS_RectMode:
            m_rectRenderStateVector.push_back( pState );
            break;
        default:
            //don't do anything
            break;
        }
    }

    return renderState;
}
