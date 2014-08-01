#ifndef BEYOND_ENGINE_RENDER_MATERIAL_H__INCLUDE
#define BEYOND_ENGINE_RENDER_MATERIAL_H__INCLUDE

#include "resource/Resource.h"
#include "Render/Texture.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "ShaderUniform.h"
#include "Render/RenderStateParam/BlendRenderStateParam.h"
#include "Render/RenderStateParam/BlendEquationRenderStateParam.h"
#include "Render/RenderStateParam/ClockWiseRenderStateParam.h"
#include "Render/RenderStateParam/CullModeRenderStateParam.h"
#include "Render/RenderStateParam/FunctionRenderStateParam.h"

class CRenderState;

class CMaterial : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CMaterial, 0xD507AB1C, CComponentInstance)
    DECLARE_RESOURCE_TYPE(eRT_Material)
public:
    CMaterial( );
    virtual ~CMaterial();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual void Initialize() override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;

    void Use();

    CRenderState* GetRenderState( ) const;

    void SetTexture( size_t stage, SharePtr<CTexture> texture);
    void SetAmbientColor( const CColor& color );
    void SetDiffuseColor( const CColor& color );
    void SetSpecularColor( const CColor& color );
    void SetShininess( float shininess );

    bool operator==( const CMaterial& other ) const;
    bool operator!=( const CMaterial& other ) const;

    void AddUniform( CShaderUniform* uniform );
    CShaderUniform* GetUniform(const TString& name);
    void ClearUniform();

    void SetSharders( const TString& strVsName, const TString& strPsName );

    //bool state
    void SetBlendEnable( bool bEnable );
    void SetDepthTest( bool bDepthTest );
    void SetCullFaceEnable( bool bCullFace );
    void SetScissorTest( bool bScissor );

    void SetBlendColor( const CColor& color );
    void SetBlendEquation( GLenum type );
    void SetBlendSource( GLenum source );
    void SetBlendDest( GLenum dest );

    void SetFrontFace( GLenum type );
    void SetCullFace( GLenum type );

    void SetScssorRect( int x, int y, int w, int h );

    //didn't clone the texture and uniform
    SharePtr<CMaterial> Clone();

private:
    void SendUniform();

    void SendLightInfo( CRenderState* pState );

    bool CompareUniform( const std::map< TString, CShaderUniform*> & m1, const std::map< TString, CShaderUniform*> & m2 ) const;
    bool CompareVector( const std::vector<SharePtr<CTexture>> & v1, const std::vector<SharePtr<CTexture>> & v2 ) const;

private:
    
    std::vector<SharePtr<CTexture>> m_textures;
    std::map< TString, CShaderUniform*> m_uniformMap;

    TString m_strVSSharderName;
    TString m_strPSSharderName;
    bool m_bSetVsShader;
    bool m_bSetPsShader;
    bool m_bBlendEnable;
    bool m_bCullFaceEnable;
    bool m_bDepthTest;
    bool m_bScissorTest;
    float m_fShininess;

    CBlendEquationRenderStateParam::EBlendEquationType m_eBlendType;
    CBlendRenderStateParam::EBlendParamType m_eBlendSource;
    CBlendRenderStateParam::EBlendParamType m_eBlendDest;
    CClockWiseRenderStateParam::EClockWiseType m_eClockType;
    CCullModeRenderStateParam::ECullModeType m_eCullType;
    CFunctionRenderStateParam::EFunctionType m_eDepthFunc;

    CRenderState* m_pRenderState;

    CColor m_ambientColor;
    CColor m_diffuseColor;
    CColor m_specularColor;
    CColor m_colorBlend;
};

#endif