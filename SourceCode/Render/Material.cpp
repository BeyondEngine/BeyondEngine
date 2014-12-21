#include "stdafx.h"
#include "Material.h"
#include "RenderManager.h"
#include "RenderState.h"
#include "Renderer.h"
#include "Resource/ResourceManager.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShaderUniform.h"
#include "Render/Texture.h"

#ifdef DEVELOP_VERSION
uint32_t CMaterial::m_uMaterialCount = 0;
uint32_t CMaterial::m_uUseCountPerFrame = 0;
#endif

CMaterial::CMaterial()
    : m_bSetVsShader(false)
    , m_bSetPsShader(false)
    , m_bBlendEnable(true)
    , m_bCullFaceEnable(true)
    , m_bDepthTest(true)
    , m_eBlendType(CBlendEquationRenderStateParam::eBET_ADD)
    , m_eBlendSource(CBlendRenderStateParam::eBPT_SRC_ALPHA)
    , m_eBlendDest(CBlendRenderStateParam::eBPT_ONE_MINUS_SRC_ALPHA)
    , m_eClockType(CClockWiseRenderStateParam::eCWT_CCW)
    , m_eCullType(CCullModeRenderStateParam::eCMT_BACK)
    , m_eDepthFunc(CFunctionRenderStateParam::eFT_LESS_EQUAL)
    , m_pRenderState(nullptr)
{
     m_pRenderState = new CRenderState();
#ifndef GL_ES_VERSION_2_0
    //there is a win32 use
    m_pRenderState->SetPolygonMode(GL_FILL, GL_FILL);
#endif
#ifdef DEVELOP_VERSION
    ++m_uMaterialCount;
#endif
}

CMaterial::~CMaterial()
{
    BEATS_SAFE_DELETE( m_pRenderState );
    ClearUniform();
    if (IsInitialized())
    {
        Uninitialize();
    }
#ifdef DEVELOP_VERSION
    --m_uMaterialCount;
#endif
}

void CMaterial::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strVSSharderName, true, 0xFFFFFFFF, _T("顶点程序"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strPSSharderName, true, 0xFFFFFFFF, _T("片段程序"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_eClockType, true, 0xFFFFFFFF, _T("多边形正面"), NULL, NULL, NULL );

    DECLARE_PROPERTY(serializer, m_bDepthTest, true, 0xFFFFFFFF, _T("深度测试"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_bCullFaceEnable, true, 0xFFFFFFFF,_T("剔除开启"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_eCullType, true, 0xFFFFFFFF, _T("模式"), _T("剔除模式"), NULL, _T("VisibleWhen:m_bCullFaceEnable == true") );

    DECLARE_PROPERTY(serializer, m_bBlendEnable, true, 0xFFFFFFFF, _T("开启混合"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_eBlendType, true, 0xFFFFFFFF, _T("混合种类"), _T("混合选项"), NULL, _T("VisibleWhen:m_bBlendEnable == true") );
    DECLARE_PROPERTY(serializer, m_eBlendSource, true, 0xFFFFFFFF, _T("混合源"), _T("混合选项"), NULL, _T("VisibleWhen:m_bBlendEnable == true") );
    DECLARE_PROPERTY(serializer, m_eBlendDest, true, 0xFFFFFFFF, _T("混合目标"), _T("混合选项"), NULL, _T("VisibleWhen:m_bBlendEnable == true") );
}

void CMaterial::Use()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_MaterialUse);
    BEYONDENGINE_PERFORMDETECT_START(ePNT_MaterialUse_SetRenderState);
        if (m_bSetPsShader && m_bSetVsShader)
        {
            SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(m_strVSSharderName);
            SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(m_strPSSharderName);
            CShaderProgram* program = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
            if (program->ID() != m_pRenderState->GetShaderProgram())
            {
                m_pRenderState->SetShaderProgram(program->ID());
            }
            m_bSetPsShader = m_bSetVsShader = false;
        }
        m_pRenderState->Restore();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_MaterialUse_SetRenderState);
    BEYONDENGINE_PERFORMDETECT_START(ePNT_MaterialUse_ApplyGPU);
        if ( m_pRenderState->GetShaderProgram() != 0 )
        {
            for (auto iter : m_uniformMap)
            {
                iter.second->SendUniform();
            }
        }
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_MaterialUse_ApplyGPU);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_MaterialUse);
#ifdef DEVELOP_VERSION
    ++m_uUseCountPerFrame;
#endif
}

void CMaterial::AddUniform( CShaderUniform* uniform )
{
    BEATS_ASSERT(!uniform->GetName().empty() && m_uniformMap.find(uniform->GetName()) == m_uniformMap.end());
    m_uniformMap[ uniform->GetName() ] = uniform;
}

CShaderUniform* CMaterial::GetUniform(const TString& name)
{
    CShaderUniform* pRet = NULL;
    auto iter = m_uniformMap.find(name);
    if (iter != m_uniformMap.end())
    {
        pRet = iter->second;
    }
    return pRet;
}

void CMaterial::ClearUniform()
{
    for( auto iter : m_uniformMap )
    {
        CShaderUniform* pUniform = iter.second;
        BEATS_SAFE_DELETE(pUniform);
    }
    m_uniformMap.clear();
}

const std::map< TString, CShaderUniform*>& CMaterial::GetUniformMap() const
{
    return m_uniformMap;
}

void CMaterial::Initialize()
{
    super::Initialize();
    if ( !m_strVSSharderName.empty() && !m_strPSSharderName.empty() )
    {
        m_bSetPsShader = m_bSetVsShader = true;
    }
    SetDepthTestEnable( m_bDepthTest );
    SetCullFaceEnable( m_bCullFaceEnable );
    SetCullFace( m_eCullType );
    SetFrontFace( m_eClockType );
    SetBlendEnable( m_bBlendEnable );
    SetBlendEquation( m_eBlendType );
    SetBlendSource(m_eBlendSource);
    SetBlendDest(m_eBlendDest);
    SetDepthFunc(m_eDepthFunc);
    SetStencilEnable(false);
}

void CMaterial::SetSharders( const TString& strVsName, const TString& strPsName )
{
    if (m_strVSSharderName != strVsName || m_strPSSharderName != strPsName)
    {
        m_strVSSharderName = strVsName;
        m_strPSSharderName = strPsName;
        if (!m_strVSSharderName.empty() && !m_strPSSharderName.empty())
        {
            m_bSetPsShader = m_bSetVsShader = true;
        }
    }
}

uint32_t CMaterial::GetShaderProgram() const
{
    return m_pRenderState->GetShaderProgram();
}

void CMaterial::SetShaderProgram(GLuint uProgram)
{
    m_pRenderState->SetShaderProgram(uProgram);
}
#ifdef EDITOR_MODE
bool CMaterial::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_strVSSharderName == pVariableAddr )
        {
            TString tempStr = m_strVSSharderName;
            DeserializeVariable(m_strVSSharderName, pSerializer, this);
            if ( tempStr != m_strVSSharderName && !m_strVSSharderName.empty() )
            {
                m_bSetVsShader = true;
            }
            bRet = true;
        }
        else if ( &m_strPSSharderName == pVariableAddr )
        {
            TString tempStr = m_strPSSharderName;
            DeserializeVariable(m_strPSSharderName, pSerializer, this);
            if ( tempStr != m_strPSSharderName && !m_strPSSharderName.empty() )
            {
                m_bSetPsShader = true;
            }
            bRet = true;
        }
        else if( &m_bDepthTest == pVariableAddr )
        {
            DeserializeVariable(m_bDepthTest, pSerializer, this);
            SetDepthTestEnable( m_bDepthTest );
            bRet = true;
        }
        else if( &m_bCullFaceEnable == pVariableAddr )
        {
            DeserializeVariable(m_bCullFaceEnable, pSerializer, this);
            SetCullFaceEnable( m_bCullFaceEnable );
            bRet = true;
        }
        else if( &m_eCullType == pVariableAddr )
        {
            DeserializeVariable(m_eCullType, pSerializer, this);
            SetCullFace( m_eCullType );
            bRet = true;
        }
        else if( &m_eClockType == pVariableAddr )
        {
            DeserializeVariable(m_eClockType, pSerializer, this);
            SetFrontFace( m_eClockType );
            bRet = true;
        }
        else if( &m_bBlendEnable == pVariableAddr )
        {
            DeserializeVariable(m_bBlendEnable, pSerializer, this);
            SetBlendEnable( m_bBlendEnable );
            bRet = true;
        }
        else if( &m_eBlendType == pVariableAddr )
        {
            DeserializeVariable(m_eBlendType, pSerializer, this);
            SetBlendEquation( m_eBlendType );
            bRet = true;
        }
        else if( &m_eBlendSource == pVariableAddr )
        {
            DeserializeVariable(m_eBlendSource, pSerializer, this);
            SetBlendSource(m_eBlendSource);
            bRet = true;
        }
        else if( &m_eBlendDest == pVariableAddr )
        {
            DeserializeVariable(m_eBlendDest, pSerializer, this);
            SetBlendDest(m_eBlendDest);
            bRet = true;
        }
    }
    return bRet;
}
#endif
void CMaterial::SetBlendEnable( bool bEnable )
{
    m_bBlendEnable = bEnable;
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_Blend, bEnable );
}

bool CMaterial::GetBlendEnable() const
{
    return m_bBlendEnable;
}

void CMaterial::SetDepthTestEnable( bool bDepthTest )
{
    m_bDepthTest = bDepthTest;
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_DepthTest, bDepthTest );
}

void CMaterial::SetDepthFunc(CFunctionRenderStateParam::EFunctionType depthFunc)
{
    m_eDepthFunc = depthFunc;
    m_pRenderState->SetDepthFunc(depthFunc);
}

void CMaterial::SetCullFaceEnable( bool bCullFace )
{
    m_bCullFaceEnable = bCullFace;
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_CullFace, bCullFace );
}

void CMaterial::SetBlendColor( const CColor& /*color*/ )
{
}

void CMaterial::SetBlendEquation( GLenum type )
{
    m_eBlendType = (CBlendEquationRenderStateParam::EBlendEquationType)type;
    m_pRenderState->SetBlendEquation( type );
}

void CMaterial::SetBlendSource( GLenum source )
{
    m_eBlendSource = (CBlendRenderStateParam::EBlendParamType)source;
    m_pRenderState->SetBlendSrcFactor( source );
}

void CMaterial::SetBlendDest( GLenum dest )
{
    m_eBlendDest = (CBlendRenderStateParam::EBlendParamType)dest;
    m_pRenderState->SetBlendTargetFactor( dest );
}

void CMaterial::SetFrontFace( GLenum type )
{
    m_pRenderState->SetFrontFace( type );
}

void CMaterial::SetCullFace( GLenum type )
{
    m_eCullType = (CCullModeRenderStateParam::ECullModeType)type;
    m_pRenderState->SetCullFace( type );
}

SharePtr<CMaterial> CMaterial::Clone()
{
    SharePtr<CMaterial> material = new CMaterial();
    material->m_bBlendEnable = m_bBlendEnable;
    material->m_bCullFaceEnable = m_bCullFaceEnable;
    material->m_bDepthTest = m_bDepthTest;

    material->m_bSetVsShader = m_bSetVsShader;
    material->m_bSetPsShader = m_bSetPsShader;
    material->m_strPSSharderName = m_strPSSharderName;
    material->m_strVSSharderName = m_strVSSharderName;

    material->m_eBlendType = m_eBlendType;
    material->m_eBlendSource = m_eBlendSource;
    material->m_eBlendDest = m_eBlendDest;
    material->m_eClockType = m_eClockType;
    material->m_eCullType = m_eCullType;
    for ( auto uniform : m_uniformMap )
    {
        CShaderUniform* u = uniform.second->Clone();
        material->m_uniformMap.insert( std::make_pair( uniform.first, u ));
    }
    BEATS_SAFE_DELETE(material->m_pRenderState);
    material->m_pRenderState = m_pRenderState->Clone();
    return material;
}

void CMaterial::SetLineWidth(float fLineWidth)
{
    m_pRenderState->SetLineWidth(fLineWidth);
}

float CMaterial::GetLineWidth() const
{
    return m_pRenderState->GetLineWidth();
}

void CMaterial::SetPointSize(float fPointSize)
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    m_pRenderState->SetPointSize(fPointSize);
#endif
}

void CMaterial::SetStencilEnable(bool bEnable)
{
    m_pRenderState->SetBoolState(CBoolRenderStateParam::eBSP_StencilTest, bEnable);
}

void CMaterial::SetStencilFunc(CFunctionRenderStateParam::EFunctionType stencilFunc, int32_t nRefValue, int32_t nMask)
{
    m_pRenderState->SetStencilReference(nRefValue);
    m_pRenderState->SetStencilValueMask(nMask);
    m_pRenderState->SetStencilFunc(stencilFunc);
}

void CMaterial::SetStencilOp(CStencilRenderStateParam::EStencilType sfail, CStencilRenderStateParam::EStencilType zfail, CStencilRenderStateParam::EStencilType zpass)
{
    m_pRenderState->SetStencilOp(sfail, zfail, zpass);
}

void CMaterial::SetAlphaTest(bool bAlphaTest)
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    m_pRenderState->SetBoolState((CBoolRenderStateParam::EBoolStateParam)GL_ALPHA_TEST, bAlphaTest);
#endif
}

void CMaterial::SetAlphaFunc(GLenum func)
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    m_pRenderState->SetAlphaFunc(func);
#endif
}

void CMaterial::SetAlphaRef(float fRef)
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    m_pRenderState->SetAlphaRef(fRef);
#endif
}

void CMaterial::SetScissorRect( int x, int y, int w, int h )
{
    m_pRenderState->SetScissorRect( (float)x, (float)y, (float)w, (float)h );
}

void CMaterial::SetDepthMask(bool bMask)
{
    m_pRenderState->SetDepthMask(bMask);
}

bool CMaterial::IsDepTest() const
{
    return m_bDepthTest;
}
