#ifndef BEYOND_ENGINE_RENDER_MATERIAL_H__INCLUDE
#define BEYOND_ENGINE_RENDER_MATERIAL_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "Render/RenderStateParam/BlendRenderStateParam.h"
#include "Render/RenderStateParam/BlendEquationRenderStateParam.h"
#include "Render/RenderStateParam/ClockWiseRenderStateParam.h"
#include "Render/RenderStateParam/CullModeRenderStateParam.h"
#include "Render/RenderStateParam/FunctionRenderStateParam.h"
#include "Render/RenderStateParam/StencilRenderStateParam.h"

class CRenderState;
class CShaderUniform;
class CTexture;
class CMaterial : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CMaterial, 0xD507AB1C, CComponentInstance)
public:
    CMaterial( );
    virtual ~CMaterial();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual void Initialize() override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    void Use();

    void AddUniform( CShaderUniform* uniform );
    CShaderUniform* GetUniform(const TString& name);
    void ClearUniform();
    const std::map< TString, CShaderUniform*>& GetUniformMap() const;

    void SetSharders( const TString& strVsName, const TString& strPsName );
    uint32_t GetShaderProgram() const;
    void SetShaderProgram(GLuint uProgram);

    //bool state
    void SetBlendEnable( bool bEnable );
    bool GetBlendEnable() const;
    void SetDepthTestEnable( bool bDepthTest );
    void SetDepthMask(bool bMask);
    void SetDepthFunc(CFunctionRenderStateParam::EFunctionType depthFunc);
    void SetCullFaceEnable( bool bCullFace );
    void SetStencilEnable(bool bEnable);
    void SetStencilFunc(CFunctionRenderStateParam::EFunctionType stencilFunc, int32_t nRefValue, int32_t nMask);
    void SetStencilOp(CStencilRenderStateParam::EStencilType sfail, CStencilRenderStateParam::EStencilType zfail, CStencilRenderStateParam::EStencilType zpass);

    // Some platform doesn't support alpha test, so you'd better not use it.
    void SetAlphaTest(bool bAlphaTest);
    void SetAlphaFunc(GLenum func);
    void SetAlphaRef(float fRef);

    void SetBlendColor( const CColor& color );
    void SetBlendEquation( GLenum type );
    void SetBlendSource( GLenum source );
    void SetBlendDest( GLenum dest );

    void SetFrontFace( GLenum type );
    void SetCullFace( GLenum type );

    void SetScissorRect( int x, int y, int w, int h );

    //didn't clone the texture and uniform
    SharePtr<CMaterial> Clone();

    void SetLineWidth(float fLineWidth);
    float GetLineWidth() const;
    void SetPointSize(float fPointSize);
    bool IsDepTest() const;

private:
    std::map< TString, CShaderUniform*> m_uniformMap;
    TString m_strVSSharderName;
    TString m_strPSSharderName;
    bool m_bSetVsShader;
    bool m_bSetPsShader;
    bool m_bBlendEnable;
    bool m_bCullFaceEnable;
    bool m_bDepthTest;

    CBlendEquationRenderStateParam::EBlendEquationType m_eBlendType;
    CBlendRenderStateParam::EBlendParamType m_eBlendSource;
    CBlendRenderStateParam::EBlendParamType m_eBlendDest;
    CClockWiseRenderStateParam::EClockWiseType m_eClockType;
    CCullModeRenderStateParam::ECullModeType m_eCullType;
    CFunctionRenderStateParam::EFunctionType m_eDepthFunc;

    CRenderState* m_pRenderState;
#ifdef DEVELOP_VERSION
public:
    static uint32_t m_uMaterialCount;
    static uint32_t m_uUseCountPerFrame;
#endif
};

#endif