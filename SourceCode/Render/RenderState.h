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
#include "RenderStateParam/RectRenderStateParam.h"
#include "RenderStateParam/UintRenderStateParam.h"

class CRenderState
{
    typedef std::vector< CRenderStateParamBase* > TParamStateVector;
    typedef TParamStateVector::iterator TParamStateVectorIter;
    typedef TParamStateVector::const_iterator TParamStateVectorIterConst;

public:
    CRenderState();
    virtual ~CRenderState();

    void SetBoolState(CBoolRenderStateParam::EBoolStateParam state, bool bEnable);
    bool GetBoolState(CBoolRenderStateParam::EBoolStateParam state);

    void SetDepthMask(bool bWriteable);
    bool GetDepthMask() const;

    void SetEdgeFlag(bool bEdgeFlag);
    bool GetEdgetFlag() const;

    void SetActiveTexture(GLenum activeTexture);
    GLenum GetActiveTexture() const;

    void SetBlendFuncSrcFactor(GLenum src);
    void SetBlendFuncTargetFactor(GLenum target);
    GLenum GetBlendSrcFactor();
    GLenum GetBlendTargetFactor();
    void SetBlendEquation(GLenum func);
    GLenum GetBlendEquation();
    void SetAlphaFunc(GLenum func);
    GLenum GetAlphaFunc();
    void SetAlphaRef(float fRef);
    float GetAlphaRef();

    void SetShaderProgram(GLuint program);
    GLuint GetShaderProgram()const;

    void SetFrontFace(GLenum frontFace);
    GLenum GetFrontFace();

    void SetCullFace(GLenum cullFace);
    GLenum GetCullFace();

    void SetLineWidth(float fLineWidth);
    float GetLineWidth();
    void SetPointSize(float fPointSize);
    float GetPointSize();

    void SetDepthNear(float fDepthNear);
    void SetDepthFar(float fDepthFar);
    float GetDepthNear();
    float GetDepthFar();
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
    
    void SetScissorRect(kmScalar x, kmScalar y, kmScalar width, kmScalar height);
    void GetScissorRect(kmVec4 &rect) const;

    void SetShadeModel(GLenum shadeModel);
    GLenum GetShadeModel();

    void SetPolygonMode( GLenum face, GLenum mode );
    void GetPolygonMode( GLenum& face, GLenum& mode );

    void SetColor( CUintRenderStateParam::EUintStateParam type, GLclampf r, GLclampf g, GLclampf b, GLclampf a );
    void GetColor( CUintRenderStateParam::EUintStateParam type , GLclampf& r, GLclampf& g, GLclampf& b, GLclampf& a );


    void Restore();

    bool operator==( const CRenderState& other ) const;

    bool operator!=( const CRenderState& other ) const;

    CRenderState* Clone();


private:

    CRenderStateParamBase* GetRenderStateParamBasePtr( ERenderState state ) const;

    CFloatRenderStateParam* GetFloatRenderStateParamPtr( CFloatRenderStateParam::EFloatStateParam type ) const;

    CFunctionRenderStateParam* GetFuncRenderStateParamPtr( CFunctionRenderStateParam::EFunctionStateParam type ) const;

    CIntRenderStateParam* GetIntRenderStateParamPtr( CIntRenderStateParam::EIntStateParam type ) const;

    CBoolRenderStateParam* GetBoolRenderStateParamPtr( CBoolRenderStateParam::EBoolStateParam type ) const;

    CUintRenderStateParam* GetUnitRenderStateParamPtr( CUintRenderStateParam::EUintStateParam type ) const;

    CRenderStateParamBase* GetRectRenderStateParamPtr( CRectRenderStateParam::ERectStateParam type) const;

    bool ComparePtrVector( const TParamStateVector & v1, const TParamStateVector & v2 ) const;

private:
    bool m_bDepthMark;
    bool m_bEdgeFlag;
    size_t m_uCurrShaderProgram;
    size_t m_uCurrActiveTexture;
    TParamStateVector m_pRenderStateParams;
    TParamStateVector m_boolRenderStateVector;
    TParamStateVector m_funcRenderStateVector;
    TParamStateVector m_floatRenderStateVector;
    TParamStateVector m_intRenderStateVector;
    TParamStateVector m_unitRenderStateVector;
    TParamStateVector m_rectRenderStateVector;
};

#endif