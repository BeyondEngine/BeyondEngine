#ifndef BEYOND_ENGINE_RENDER_RENDERSTATE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERSTATE_H__INCLUDE

#include "RenderStateParam/RenderStateParamBase.h"
#include "RenderStateParam/BlendEquationRenderStateParam.h"
#include "RenderStateParam/BlendRenderStateParam.h"
#include "RenderStateParam/BoolRenderStateParam.h"
#include "RenderStateParam/ClockWiseRenderStateParam.h"
#include "RenderStateParam/CullModeRenderStateParam.h"
#include "RenderStateParam/FloatRenderStateParam.h"
#include "RenderStateParam/FunctionRenderStateParam.h"
#include "RenderStateParam/IntRenderStateParam.h"
#include "RenderStateParam/PolygonModeRenderStateParam.h"
#include "RenderStateParam/UintRenderStateParam.h"

class CRenderState
{
public:
    CRenderState();
    ~CRenderState();

    void SetBoolState(CBoolRenderStateParam::EBoolStateParam state, bool bEnable);
    bool GetBoolState(CBoolRenderStateParam::EBoolStateParam state);

    void SetDepthMask(bool bWriteable);
    bool GetDepthMask() const;
    void SetScissorTest(bool bEnable);
    bool GetScissorTest() const;

    void SetActiveTexture(uint32_t uChannel);
    uint32_t GetActiveTexture() const;
    uint32_t GetCurrBindingTexture() const;
    void SetCurrBindingTexture(uint32_t uTextureId);
    std::map<unsigned char, uint32_t>& GetBindingTextureMap();

    void SetBlendSrcFactor(GLenum src);
    void SetBlendTargetFactor(GLenum target);

#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    GLenum GetBlendSrcFactor();
    GLenum GetBlendTargetFactor();
    void SetAlphaFunc(GLenum func);
    GLenum GetAlphaFunc();
    void SetAlphaRef(float fRef);
    float GetAlphaRef();
    void SetPointSize(float fPointSize);
    float GetPointSize();
    void SetShadeModel(GLenum shadeModel);
    GLenum GetShadeModel();
    void SetPolygonMode(GLenum frontMode, GLenum backMode);
    void GetPolygonMode(CPolygonModeRenderStateParam::EPolygonModeType& frontType, CPolygonModeRenderStateParam::EPolygonModeType& backType);
#endif
    void SetBlendEquation(GLenum func);
    GLenum GetBlendEquation();

    void SetShaderProgram(GLuint program);
    GLuint GetShaderProgram()const;

    void SetFrontFace(GLenum frontFace);
    GLenum GetFrontFace();

    void SetCullFace(GLenum cullFace);
    GLenum GetCullFace();

    void SetLineWidth(float fLineWidth);
    float GetLineWidth();

    void SetDepthRange(float fDepthNear, float fDepthFar);
    void GetDepthRange(float& fDepthNear, float& fDepthFar);
    void SetDepthFunc(GLenum func);
    GLenum GetDepthFunc();

    void SetStencilFunc(GLenum func);
    GLenum GetStencilFunc();
    void SetStencilReference(GLint nRef);
    GLint GetStencilReference();
    void SetStencilValueMask(GLint nValueMask);
    GLint GetStencilValueMask();
    void SetClearStencil(GLint nClearValue);
    GLint GetClearStencil();
    void SetStencilOp( GLenum fail, GLenum zFail, GLenum zPass );
    void GetStencilOp( GLenum& fail, GLenum& zFail, GLenum& zPass );
    
    void SetScissorRect(float x, float y, float width, float height);
    const CVec4& GetScissorRect() const;
    void SetViewport(float x, float y, float width, float height);
    const CVec4& GetViewport() const;

    void SetColor( CUintRenderStateParam::EUintStateParam type, GLclampf r, GLclampf g, GLclampf b, GLclampf a );
    void GetColor( CUintRenderStateParam::EUintStateParam type , GLclampf& r, GLclampf& g, GLclampf& b, GLclampf& a );

    void Restore();

    CRenderState* Clone();
    uint32_t GetBindingVAO() const;
    uint32_t GetBindingVBO() const;
    uint32_t GetBindingEBO() const;
    void SetBindingVAO(uint32_t uVAO);
    void SetBindingVBO(uint32_t uVBO);
    void SetBindingEBO(uint32_t uEBO);

private:
    CRenderStateParamBase* GetRenderStateParamBasePtr(ERenderStateParamType state, int nSubType) const;

private:
    bool m_bDepthMark;
    bool m_bScissorTest = false;
    CVec4 m_scissorRect;
    CVec4 m_viewport;
    uint32_t m_uCurrShaderProgram;
    uint32_t m_uCurrActiveTexture;
    uint32_t m_uVAO;
    uint32_t m_uVBO;
    uint32_t m_uEBO;
    std::map<ERenderStateParamType, std::map<int, CRenderStateParamBase*> > m_renderStateParamMap;
    std::map<unsigned char, uint32_t> m_bindingTexture;
};

#endif