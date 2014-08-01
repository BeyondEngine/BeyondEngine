#include "stdafx.h"
#include "Material.h"
#include "RenderManager.h"
#include "RenderState.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Renderer.h"
#include "Resource/ResourceManager.h"
#include "Shader.h"
#include "ShaderProgram.h"


CMaterial::CMaterial()
    : m_bSetVsShader(false)
    , m_bSetPsShader(false)
    , m_bBlendEnable(true)
    , m_bCullFaceEnable(true)
    , m_bDepthTest(true)
    , m_bScissorTest(false)
    , m_fShininess(0.0f)
    , m_eBlendType(CBlendEquationRenderStateParam::eBET_ADD)
    , m_eBlendSource(CBlendRenderStateParam::eBPT_SRC_ALPHA)
    , m_eBlendDest(CBlendRenderStateParam::eBPT_INV_SRC_ALPHA)
    , m_eClockType(CClockWiseRenderStateParam::eCWT_CCW)
    , m_eCullType(CCullModeRenderStateParam::eCMT_BACK)
    , m_eDepthFunc(CFunctionRenderStateParam::eFT_ALWAYS)
    , m_pRenderState(nullptr)
    , m_ambientColor(CColor( 0.2f, 0.2f, 0.2f, 1.0f ))
    , m_diffuseColor(CColor( 0.8f, 0.8f, 0.8f, 1.0f ))
    , m_specularColor(CColor( 0.0f, 0.0f, 0.0f, 1.0f ))
    , m_colorBlend(0x00000000)
{
     m_pRenderState = new CRenderState();
#ifndef GL_ES_VERSION_2_0
    //there is a win32 use
    m_pRenderState->SetPolygonMode( GL_FRONT, GL_FILL );
#endif
    //default set
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_ScissorTest, false );
}

CMaterial::~CMaterial()
{
    BEATS_SAFE_DELETE( m_pRenderState );
    ClearUniform();
}

void CMaterial::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_ambientColor, true, 0xFFFFFFFF, _T("环境反射"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_diffuseColor, true, 0xFFFFFFFF, _T("漫反射"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_specularColor, true, 0xFFFFFFFF, _T("镜面反射"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fShininess, true, 0xFFFFFFFF, _T("耀光度"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strVSSharderName, true, 0xFFFFFFFF, _T("顶点程序"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strPSSharderName, true, 0xFFFFFFFF, _T("片段程序"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uniformMap, true, 0xFFFFFFFF, _T("shader自定义参数"), NULL, NULL, NULL);

    DECLARE_PROPERTY(serializer, m_eClockType, true, 0xFFFFFFFF, _T("多边形正面"), NULL, NULL, NULL );

    DECLARE_PROPERTY(serializer, m_bDepthTest, true, 0xFFFFFFFF, _T("深度测试"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_bScissorTest, true, 0xFFFFFFFF, _T("裁剪测试"), NULL, NULL, NULL );

    DECLARE_PROPERTY(serializer, m_bCullFaceEnable, true, 0xFFFFFFFF,_T("剔除开启"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_eCullType, true, 0xFFFFFFFF, _T("模式"), _T("剔除模式"), NULL, _T("VisibleWhen:m_bCullFaceEnable == true") );

    DECLARE_PROPERTY(serializer, m_bBlendEnable, true, 0xFFFFFFFF, _T("开启混合"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_colorBlend, true, 0xFFFFFFFF, _T("混合颜色"), _T("混合选项"), NULL, _T("VisibleWhen:m_bBlendEnable == true") );
    DECLARE_PROPERTY(serializer, m_eBlendType, true, 0xFFFFFFFF, _T("混合种类"), _T("混合选项"), NULL, _T("VisibleWhen:m_bBlendEnable == true") );
    DECLARE_PROPERTY(serializer, m_eBlendSource, true, 0xFFFFFFFF, _T("混合源"), _T("混合选项"), NULL, _T("VisibleWhen:m_bBlendEnable == true") );
    DECLARE_PROPERTY(serializer, m_eBlendDest, true, 0xFFFFFFFF, _T("混合目标"), _T("混合选项"), NULL, _T("VisibleWhen:m_bBlendEnable == true") );
}


void CMaterial::Use()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_MaterialUse);
    CRenderer* pRenderer = CRenderer::GetInstance();
    if ( m_pRenderState )
    {
        BEYONDENGINE_PERFORMDETECT_START(ePNT_MaterialUse_SetRenderState);
        m_pRenderState->Restore();
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_MaterialUse_SetRenderState);
        BEYONDENGINE_PERFORMDETECT_START(ePNT_MaterialUse_ApplyGPU);
        if ( m_bSetPsShader && m_bSetVsShader )
        {
            SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(m_strVSSharderName);
            SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(m_strPSSharderName);
            CShaderProgram* program = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
            if ( program->ID() != m_pRenderState->GetShaderProgram() )
            {
                m_pRenderState->SetShaderProgram( program->ID() );
            }
            m_bSetPsShader = m_bSetVsShader = false;
        }
        if ( m_pRenderState->GetShaderProgram() != 0 )
        {
            pRenderer->BindTexture( GL_TEXTURE_2D, 0 );
            pRenderer->UseProgram( m_pRenderState->GetShaderProgram() );
            for ( size_t i = 0; i < m_textures.size(); ++i )
            {
                pRenderer->ActiveTexture( GL_TEXTURE0 + i );
                pRenderer->BindTexture( GL_TEXTURE_2D, m_textures[i] ? m_textures[ i ]->ID() : 0 );
                pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                GLuint currProgram = pRenderer->GetCurrentState()->GetShaderProgram();
                if ( 0 != currProgram )
                {
                    GLint textureLocation = pRenderer->GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_TEX0 + i]);
                    pRenderer->SetUniform1i( textureLocation, i);
                }
            }
            SendLightInfo( m_pRenderState );
            SendUniform();
        }
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_MaterialUse_ApplyGPU);
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_MaterialUse);
}

void CMaterial::SendUniform()
{
    for ( auto iter : m_uniformMap )
    {
        iter.second->SendUniform( );
    }
}

void CMaterial::SendLightInfo(CRenderState* pState)
{
    float ambientColor[4] = {(float)m_ambientColor.r / 0xFF, (float)m_ambientColor.g / 0xFF, (float)m_ambientColor.b / 0xFF, (float)m_ambientColor.a / 0xFF};
    float diffuseColor[4] = {(float)m_diffuseColor.r / 0xFF, (float)m_diffuseColor.g / 0xFF, (float)m_diffuseColor.b / 0xFF, (float)m_diffuseColor.a / 0xFF};
    float specularColor[4] = {(float)m_specularColor.r / 0xFF, (float)m_specularColor.g / 0xFF, (float)m_specularColor.b / 0xFF, (float)m_specularColor.a / 0xFF};

    GLuint currProgram = pState->GetShaderProgram();
    if ( currProgram != 0 )
    {
        GLint ambientLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_AMBIENT_COLOR]);
        if ( -1 != ambientLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( ambientLocation, ambientColor, 4 );
        }
        GLint diffuseLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_DIFFUSE_COLOR]);
        if ( -1 != diffuseLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( diffuseLocation, diffuseColor, 4 );
        }
        GLint specularLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_SPECULAR_COLOR]);
        if ( -1 != specularLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( specularLocation, specularColor, 4 );
        }
        GLint shininessLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_SHININESS]);
        if ( -1 != shininessLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( shininessLocation, &m_fShininess, 1 );
        }
    }
}

CRenderState* CMaterial::GetRenderState() const
{
    return m_pRenderState;
}

void CMaterial::SetTexture(size_t stage, SharePtr<CTexture> texture)
{
    if ( stage >= m_textures.size() )
    {
        m_textures.resize(stage + 1);
    }
    m_textures[stage] = texture;
}


void CMaterial::SetAmbientColor( const CColor& color )
{
    m_ambientColor = color;
}

void CMaterial::SetDiffuseColor( const CColor& color )
{
    m_diffuseColor = color;
}

void CMaterial::SetSpecularColor( const CColor& color )
{
    m_specularColor = color;
}

void CMaterial::SetShininess( float shininess )
{
    m_fShininess = shininess;
}

bool CMaterial::operator==( const CMaterial& other ) const
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_MaterialCompare);
    bool bEqual = false;
    if ( this == &other )
    {
        bEqual = true;
    }
    else
    {
        if ( m_strPSSharderName == other.m_strPSSharderName 
            && m_strVSSharderName == other.m_strVSSharderName )
        {
            if ( CompareVector( m_textures, other.m_textures ) &&
                CompareUniform( m_uniformMap, other.m_uniformMap ) )
            {
                bEqual = *m_pRenderState == *other.m_pRenderState;
            }
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_MaterialCompare);
    return bEqual;
}

bool CMaterial::operator!=( const CMaterial& other) const
{
    return !((*this) == other);
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
    m_uniformMap.clear();
}

void CMaterial::Initialize()
{
    super::Initialize();
    if ( !m_strVSSharderName.empty() && !m_strPSSharderName.empty() )
    {
        m_bSetPsShader = m_bSetVsShader = true;
    }

    SetDepthTest( m_bDepthTest );
    SetCullFaceEnable( m_bCullFaceEnable );
    SetCullFace( m_eCullType );
    SetFrontFace( m_eClockType );
    SetBlendEnable( m_bBlendEnable );
    SetBlendColor( m_colorBlend );
    SetBlendEquation( m_eBlendType );
    SetBlendSource(m_eBlendSource);
    SetBlendDest(m_eBlendDest);
}

void CMaterial::SetSharders( const TString& strVsName, const TString& strPsName )
{
    m_strVSSharderName = strVsName;
    m_strPSSharderName = strPsName;
    if ( !m_strVSSharderName.empty() && !m_strPSSharderName.empty() )
    {
        m_bSetPsShader = m_bSetVsShader = true;
    }
}

bool CMaterial::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_strVSSharderName == pVariableAddr )
        {
            TString tempStr = m_strVSSharderName;
            DeserializeVariable( m_strVSSharderName, pSerializer );
            if ( tempStr != m_strVSSharderName && !m_strVSSharderName.empty() )
            {
                m_bSetVsShader = true;
            }
            bRet = true;
        }
        else if ( &m_strPSSharderName == pVariableAddr )
        {
            TString tempStr = m_strPSSharderName;
            DeserializeVariable( m_strPSSharderName, pSerializer );
            if ( tempStr != m_strPSSharderName && !m_strPSSharderName.empty() )
            {
                m_bSetPsShader = true;
            }
            bRet = true;
        }
        else if( &m_bDepthTest == pVariableAddr )
        {
            DeserializeVariable( m_bDepthTest, pSerializer );
            SetDepthTest( m_bDepthTest );
            bRet = true;
        }
        else if( &m_bCullFaceEnable == pVariableAddr )
        {
            DeserializeVariable( m_bCullFaceEnable, pSerializer );
            SetCullFaceEnable( m_bCullFaceEnable );
            bRet = true;
        }
        else if( &m_eCullType == pVariableAddr )
        {
            DeserializeVariable( m_eCullType, pSerializer );
            SetCullFace( m_eCullType );
            bRet = true;
        }
        else if( &m_eClockType == pVariableAddr )
        {
            DeserializeVariable( m_eClockType, pSerializer );
            SetFrontFace( m_eClockType );
            bRet = true;
        }
        else if( &m_bBlendEnable == pVariableAddr )
        {
            DeserializeVariable( m_bBlendEnable, pSerializer );
            SetBlendEnable( m_bBlendEnable );
            bRet = true;
        }
        else if( &m_colorBlend == pVariableAddr )
        {
            DeserializeVariable( m_colorBlend, pSerializer );
            SetBlendColor( m_colorBlend );
            bRet = true;
        }
        else if( &m_eBlendType == pVariableAddr )
        {
            DeserializeVariable( m_eBlendType, pSerializer );
            SetBlendEquation( m_eBlendType );
            bRet = true;
        }
        else if( &m_eBlendSource == pVariableAddr )
        {
            DeserializeVariable( m_eBlendSource, pSerializer );
            SetBlendSource(m_eBlendSource);
            bRet = true;
        }
        else if( &m_eBlendDest == pVariableAddr )
        {
            DeserializeVariable( m_eBlendDest, pSerializer );
            SetBlendDest(m_eBlendDest);
            bRet = true;
        }
    }
    return bRet;
}

void CMaterial::SetBlendEnable( bool bEnable )
{
    m_bBlendEnable = bEnable;
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_Blend, bEnable );
}

void CMaterial::SetDepthTest( bool bDepthTest )
{
    m_bDepthTest = bDepthTest;
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_DepthTest, bDepthTest );
}

void CMaterial::SetCullFaceEnable( bool bCullFace )
{
    m_bCullFaceEnable = bCullFace;
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_CullFace, bCullFace );
}

void CMaterial::SetBlendColor( const CColor& color )
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
    m_pRenderState->SetBlendFuncSrcFactor( source );
}

void CMaterial::SetBlendDest( GLenum dest )
{
    m_eBlendDest = (CBlendRenderStateParam::EBlendParamType)dest;
    m_pRenderState->SetBlendFuncTargetFactor( dest );
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

bool CMaterial::CompareVector( const std::vector<SharePtr<CTexture>> & v1, const std::vector<SharePtr<CTexture>> & v2 ) const
{
    bool bReturn = true;
    if ( v1.size() == v2.size() )
    {
        for ( size_t i = 0; i < v1.size(); ++i )
        {
            if ( v1[ i ] != v2[ i ] )
            {
                if( !(v1[i] && v2[i])|| *v1[ i ].Get() != *v2[ i ].Get() )
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
    return bReturn;
}

bool CMaterial::CompareUniform( const std::map< TString, CShaderUniform*> & m1, const std::map< TString, CShaderUniform*> & m2 ) const
{
    bool bReturn = true;

    if ( m1.size() == m2.size() )
    {
        auto m1iter = m1.begin();
        for ( ; m1iter != m1.end(); ++m1iter )
        {
            auto m2iter = m2.find( m1iter->first );
            if ( m2iter == m2.end() )
            {
                bReturn = false;
                break;
            }
            else
            {
                bReturn = *m1iter->second == *m2iter->second;
                if ( !bReturn )
                {
                    break;
                }
            }
        }
    }
    else
    {
        bReturn = false;
    }

    return bReturn;
}

SharePtr<CMaterial> CMaterial::Clone()
{
    SharePtr<CMaterial> material = new CMaterial();
    material->m_bBlendEnable = m_bBlendEnable;
    material->m_bCullFaceEnable = m_bCullFaceEnable;
    material->m_bDepthTest = m_bDepthTest;
    material->m_fShininess = m_fShininess;
    material->m_bScissorTest = m_bScissorTest;

    material->m_bSetVsShader = m_bSetVsShader;
    material->m_bSetPsShader = m_bSetPsShader;
    material->m_strPSSharderName = m_strPSSharderName;
    material->m_strVSSharderName = m_strVSSharderName;

    material->m_eBlendType = m_eBlendType;
    material->m_eBlendSource = m_eBlendSource;
    material->m_eBlendDest = m_eBlendDest;
    material->m_eClockType = m_eClockType;
    material->m_eCullType = m_eCullType;
    material->m_eDepthFunc = m_eDepthFunc;

    material->m_ambientColor = m_ambientColor;
    material->m_diffuseColor = m_diffuseColor;
    material->m_specularColor = m_specularColor;

    material->m_colorBlend = m_colorBlend;

    for ( auto uniform : m_uniformMap )
    {
        CShaderUniform* u = uniform.second->Clone();
        material->m_uniformMap.insert( std::make_pair( uniform.first, u ));
    }
    material->m_pRenderState = m_pRenderState->Clone();
    return material;
}

void CMaterial::SetScissorTest( bool bScissor )
{
    m_bScissorTest = bScissor;
    m_pRenderState->SetBoolState( CBoolRenderStateParam::eBSP_ScissorTest, bScissor );
}

void CMaterial::SetScssorRect( int x, int y, int w, int h )
{
    m_pRenderState->SetScissorRect( (kmScalar)x, (kmScalar)y, (kmScalar)w, (kmScalar)h );
}
