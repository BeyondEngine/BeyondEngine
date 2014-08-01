#include "stdafx.h"
#include "renderer.h"
#include "RenderManager.h"
#include "RenderState.h"
#include "Camera.h"

CRenderer* CRenderer::m_pInstance = NULL;

CRenderer::CRenderer()
    : m_pCurrState(new CRenderState)
{
}

CRenderer::~CRenderer()
{
    BEATS_SAFE_DELETE(m_pCurrState);
}

void CRenderer::GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
    glGetBufferParameteriv(target, pname, params);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::UseProgram(GLuint uProgram)
{
    if(m_pCurrState->GetShaderProgram() != uProgram)
    {
        m_pCurrState->SetShaderProgram(uProgram);
        glUseProgram(uProgram);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    }
}

GLuint CRenderer::CreateShader(GLenum type)
{
    GLuint uRet = glCreateShader(type);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    return uRet;
}

void CRenderer::DeleteShader(GLuint uProgram)
{
    glDeleteShader(uProgram);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::ShaderSource(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths)
{
    glShaderSource(shader, count, strings, lengths);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::CompileShader(GLuint shader)
{
    glCompileShader(shader);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetProgramiv(GLuint program, GLenum pname, GLint* param)
{
    glGetProgramiv(program, pname, param);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetShaderiv(GLuint shader, GLenum pname, GLint* param)
{
    glGetShaderiv(shader, pname, param);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
    glGetShaderInfoLog(shader, bufSize, length, infoLog);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::LinkProgram(GLuint program)
{
    glLinkProgram(program);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::AttachShader(GLuint program, GLuint shader)
{
    glAttachShader(program, shader);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DetachShader(GLuint program, GLuint shader)
{
    glDetachShader(program, shader);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

GLuint CRenderer::CreateProgram()
{
    GLuint uProgram = glCreateProgram();
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    return uProgram;
}

void CRenderer::DeleteProgram(GLuint uProgram)
{
    glDeleteProgram(uProgram);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindAttribLocation(GLuint uProgram, ECommonAttribIndex attribIndex)
{
    glBindAttribLocation(uProgram, attribIndex, COMMON_ATTIB_NAMES[attribIndex]);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

GLint CRenderer::GetUniformLocation(GLuint uProgram, const char *name ) const
{
    BEATS_ASSERT(name != nullptr, _T("Invalid uniform name") );
    BEATS_ASSERT(uProgram != 0, _T("Invalid operation. Cannot get uniform location when program is not initialized"));
    GLint nRet = glGetUniformLocation(uProgram, name);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    return nRet;
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
    glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}
#endif

void CRenderer::SetUniform1i(GLint location, GLint i1)
{
    glUniform1i(location, i1);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform2i(GLint location, GLint i1, GLint i2)
{
    glUniform2i(location, i1, i2);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();

}

void CRenderer::SetUniform3i(GLint location, GLint i1, GLint i2, GLint i3)
{
    glUniform3i(location, i1, i2, i3);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();

}

void CRenderer::SetUniform4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4)
{
    glUniform4i(location, i1, i2, i3, i4);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform1f(GLint location, GLfloat f1)
{
    glUniform1f(location, f1);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform2f(GLint location, GLfloat f1, GLfloat f2)
{
    glUniform2f(location, f1, f2);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3)
{
    glUniform3f(location, f1, f2, f3);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
    glUniform4f(location, f1, f2, f3, f4);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform1fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats)
{
    glUniform1fv(location, numberOfFloats, floats);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniformMatrix4fv(GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices)
{
    glUniformMatrix4fv(location, numberOfMatrices, GL_FALSE, matrixArray);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}
#endif

void CRenderer::LineWidth(GLfloat fLineWidth)
{
    BEATS_ASSERT(fLineWidth > 0);
    if(m_pCurrState->GetLineWidth() != fLineWidth)
    {
        glLineWidth(fLineWidth);
        m_pCurrState->SetLineWidth(fLineWidth);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    }
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::PointSize(GLfloat fPointSize)
{
    BEATS_ASSERT(fPointSize > 0);
    if(m_pCurrState->GetPointSize() != fPointSize)
    {
        glPointSize(fPointSize);
        m_pCurrState->SetPointSize(fPointSize);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    }
}
#endif

void CRenderer::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
#ifdef _DEBUG
    CEngineCenter::GetInstance()->IncreaseDrawCall();
#endif
}

void CRenderer::DrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid* pIndices )
{
    BEATS_ASSERT(type == GL_UNSIGNED_BYTE || type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT,
        _T("Invalid type parameter of glDrawElement!\ntype: %d"), type);
    glDrawElements(mode, count, type, pIndices);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
#ifdef _DEBUG
    CEngineCenter::GetInstance()->IncreaseDrawCall();
#endif
}

void CRenderer::GenVertexArrays( GLsizei n, GLuint* arrays )
{
    glGenVertexArrays(n, arrays);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindVertexArray( GLuint array )
{
    glBindVertexArray(array);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DeleteVertexArrays(GLsizei n, GLuint *arrays)
{
    glDeleteVertexArrays(n, arrays);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GenBuffers( GLsizei n, GLuint* buffers )
{
    glGenBuffers(n, buffers);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindBuffer(GLenum target, GLuint buffer )
{
    glBindBuffer(target, buffer);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::BindBufferRange( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )
{
    glBindBufferRange(target, index, buffer, offset, size);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}
#endif

void CRenderer::DeleteBuffers(GLsizei n, GLuint *buffers)
{
    glDeleteBuffers(n, buffers);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::EnableVertexAttribArray( GLuint attribute )
{
    glEnableVertexAttribArray(attribute);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DisableVertexAttribArray(GLuint attribute)
{
    glDisableVertexAttribArray(attribute);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
{
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
#ifndef GL_ES_VERSION_2_0
    BEATS_ASSERT( type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_INT || type == GL_UNSIGNED_INT );
    glVertexAttribIPointer(index, size, type, stride, pointer);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
#endif
}


void CRenderer::BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::ActiveTexture(GLenum texture)
{
    if (m_pCurrState->GetActiveTexture() != texture)
    {
        glActiveTexture(texture);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetActiveTexture(texture);
    }
}

void CRenderer::GenTextures(GLsizei n, GLuint *textures)
{
    glGenTextures(n, textures);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindTexture(GLenum target, GLuint texture)
{
    glBindTexture( target, texture);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::TextureImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::TextureSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::TexParameteri(GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DeleteTexture(GLsizei n, const GLuint* textures)
{
    glDeleteTextures(n, textures);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GenFramebuffers(GLsizei n, GLuint* framebuffers)
{
    glGenFramebuffers(n, framebuffers);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindFramebuffer(GLenum target, GLuint framebuffer)
{
    glBindFramebuffer(target, framebuffer);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DeleteFramebuffer(GLsizei n, const GLuint * framebuffers)
{
    glDeleteFramebuffers(n, framebuffers);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
    glGenRenderbuffers(n, renderbuffers);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindRenderbuffer(GLenum target, GLuint renderbuffer)
{
    glBindRenderbuffer(target, renderbuffer);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    glRenderbufferStorage(target, internalformat, width, height);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DeleteRenderbuffer(GLsizei n, const GLuint* renderbuffers)
{
    glDeleteRenderbuffers(n, renderbuffers);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetIntegerV( GLenum pname, GLint * params )
{
    glGetIntegerv(pname, params);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::PixelStorei (GLenum pname, GLint param)
{
    glPixelStorei(pname, param);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetScissorRect(GLint x, GLint y, GLsizei width, GLsizei height)
{
    kmVec4 rect;
    m_pCurrState->GetScissorRect(rect);
    if( rect.x != x || rect.y != y ||
        rect.z != width || rect.w != height )
    {
        glScissor(x, y, width, height);
        m_pCurrState->SetScissorRect((kmScalar)x, (kmScalar)y, (kmScalar)width, (kmScalar)height);
    }
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::EnableGL(CBoolRenderStateParam::EBoolStateParam cap)
{
    if (!m_pCurrState->GetBoolState(cap))
    {
        glEnable(cap);
        m_pCurrState->SetBoolState(cap, true);
    }
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DisableGL(CBoolRenderStateParam::EBoolStateParam cap)
{
    if (m_pCurrState->GetBoolState(cap))
    {
        glDisable(cap);
        m_pCurrState->SetBoolState(cap, false);
    }
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

bool CRenderer::IsEnable(CBoolRenderStateParam::EBoolStateParam cap)
{
    bool bRet = glIsEnabled(cap) == GL_TRUE;
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    return bRet;
}

void CRenderer::BlendFunc(GLenum sfactor, GLenum dfactor)
{
#ifndef GL_ES_VERSION_2_0
    bool bChanged = false;
    if (m_pCurrState->GetBlendSrcFactor() != sfactor)
    {
        bChanged = true;
        m_pCurrState->SetBlendFuncSrcFactor(sfactor);
    }
    if (m_pCurrState->GetBlendTargetFactor() != dfactor)
    {
        bChanged = true;
        m_pCurrState->SetBlendFuncTargetFactor(dfactor);
    }
    if (bChanged)
#endif
    {
        glBlendFunc(sfactor, dfactor);
    }
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BlendEquation(GLenum func)
{
    if (m_pCurrState->GetBlendEquation() != func)
    {
        m_pCurrState->SetBlendEquation(func);
        glBlendEquation(func);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    }
}

void CRenderer::BlendColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
    GLclampf rtemp, gtemp, btemp, atemp;
    m_pCurrState->GetColor( CUintRenderStateParam::EUintStateParam::eUSP_BlendColor, rtemp, gtemp, btemp, atemp );
    if ( rtemp != r || gtemp != g || btemp != b || atemp != a )
    {
        glBlendColor(r, g, b, a);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetColor( CUintRenderStateParam::EUintStateParam::eUSP_BlendColor, r, g, b, a );
    }
}

void CRenderer::PolygonMode (GLenum face, GLenum mode)
{
#ifndef GL_ES_VERSION_2_0
    GLenum tempFace, tempMode;
    m_pCurrState->GetPolygonMode( tempFace, tempMode );
    if ( tempFace != face || tempMode != mode )
    {
        glPolygonMode(face, mode);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetPolygonMode(face, mode);
    }
#endif
}


void CRenderer::ClearBuffer(GLbitfield bit )
{
    glClear(bit);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLclampf rtemp, gtemp, btemp, atemp;
    m_pCurrState->GetColor( CUintRenderStateParam::EUintStateParam::eUSP_ClearColor, rtemp, gtemp, btemp, atemp );
    if ( rtemp != red || gtemp != green || btemp != blue || atemp != alpha )
    {
        glClearColor(red, green, blue, alpha);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    }
}

void CRenderer::ClearDepth(GLclampf depth)
{
    BEATS_ASSERT(depth >= 0 && depth <= 1);
    glClearDepth(depth);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(target, offset, size, data);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

GLvoid* CRenderer::MapBuffer(GLenum target, GLenum access)
{
#ifndef GL_ES_VERSION_2_0
    GLvoid* pData = glMapBuffer(target, access);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    return pData;
#else
    return NULL;
#endif
}

void CRenderer::UnmapBuffer(GLenum target)
{
    glUnmapBuffer(target);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DepthMask(bool bWriteable)
{
    if (m_pCurrState->GetDepthMask() != bWriteable)
    {
        glDepthMask(bWriteable);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetDepthMask(bWriteable);
    }
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::EdgeFlag(bool bEdgeFlag)
{
    if (m_pCurrState->GetEdgetFlag() != bEdgeFlag)
    {
        glEdgeFlag(bEdgeFlag);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetEdgeFlag(bEdgeFlag);
    }
}
#endif
void CRenderer::FrontFace(GLenum frontFace)
{
    if (m_pCurrState->GetFrontFace() != frontFace)
    {
        glFrontFace(frontFace);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetFrontFace(frontFace);
    }
}

void CRenderer::CullFace(GLenum cullFace)
{
    if (m_pCurrState->GetCullFace() != cullFace)
    {
        glCullFace(cullFace);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetCullFace(cullFace);
    }
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::DepthRange(float fNear, float fFar)
{
    bool bChanged = false;
    if (!BEATS_FLOAT_EQUAL(m_pCurrState->GetDepthFar(), fFar))
    {
        m_pCurrState->SetDepthFar(fFar);
        bChanged = true;
    }
    if (!BEATS_FLOAT_EQUAL(m_pCurrState->GetDepthNear(), fNear))
    {
        m_pCurrState->SetDepthNear(fNear);
        bChanged = true;
    }
    if (bChanged)
    {
        glDepthRange(fNear, fFar);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    }
}
#endif
void CRenderer::DepthFunc(GLenum depthFunc)
{
    if(m_pCurrState->GetDepthFunc() != depthFunc)
    {
        glDepthFunc(depthFunc);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetDepthFunc(depthFunc);
    }
}

void CRenderer::StencilFunc(GLenum stencilFunc)
{
    if(m_pCurrState->GetStencilFunc() != stencilFunc)
    {
        glStencilFunc(stencilFunc, m_pCurrState->GetStencilReference(), m_pCurrState->GetStencilValueMask());
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetStencilFunc(stencilFunc);
    }
}

void CRenderer::StencilReference(GLint nRef)
{
    if (m_pCurrState->GetStencilReference() != nRef)
    {
        glStencilFunc( m_pCurrState->GetStencilFunc(), nRef, m_pCurrState->GetStencilValueMask());
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetStencilReference(nRef);
    }
}

void CRenderer::StencilValueMask(GLint nValueMask)
{
    if (m_pCurrState->GetStencilValueMask() != nValueMask)
    {
        glStencilFunc( m_pCurrState->GetStencilFunc(), m_pCurrState->GetStencilReference(), nValueMask );
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetStencilValueMask(nValueMask);
    }
}

void CRenderer::ClearStencil(GLint nClearValue)
{
    if (m_pCurrState->GetClearStencil() != nClearValue)
    {
        glClearStencil(nClearValue);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetClearStencil(nClearValue);
    }
    
}

void CRenderer::StencilOp(GLenum fail, GLenum zFail, GLenum zPass)
{
    GLenum failTemp, zFailTemp, zPassTemp;
    m_pCurrState->GetStencilOp( failTemp, zFailTemp, zPassTemp );
    if ( failTemp != fail || zFailTemp != zFail || zPassTemp != zPass )
    {
        glStencilOp(fail, zFail, zPass);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetStencilOp( fail, zFail, zPass );
    }
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::ShadeModel(GLenum shadeModel)
{
    if (m_pCurrState->GetShadeModel() != shadeModel)
    {
        glShadeModel(shadeModel);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetShadeModel(shadeModel);
    }
}
#endif
CRenderState* CRenderer::GetCurrentState() const
{
    return m_pCurrState;
}

void CRenderer::GetFloatV( GLenum pname, GLfloat * params )
{
    glGetFloatv( pname, params );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform2fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats )
{
    glUniform2fv( location, numberOfFloats, floats );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform3fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats )
{
    glUniform3fv( location, numberOfFloats, floats );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform4fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats )
{
    glUniform4fv( location, numberOfFloats, floats );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform1iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform1iv( location, numberOfInt, ints );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform2iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform2iv( location, numberOfInt, ints );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform3iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform3iv( location, numberOfInt, ints );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform4iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform3iv( location, numberOfInt, ints );
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniformMatrix2fv( GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices )
{
    glUniformMatrix2fv(location, numberOfMatrices, GL_FALSE, matrixArray);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniformMatrix3fv( GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices )
{
    glUniformMatrix3fv(location, numberOfMatrices, GL_FALSE, matrixArray);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}
#ifndef GL_ES_VERSION_2_0
void CRenderer::AlphaFunc( GLenum func, GLclampf ref )
{
    GLenum tempFunc = m_pCurrState->GetAlphaFunc();
    GLclampf tempRef = m_pCurrState->GetAlphaRef();

    if ( func != tempFunc || ref != tempRef )
    {
        m_pCurrState->SetAlphaFunc( func );
        m_pCurrState->SetAlphaRef( ref );
        glAlphaFunc( func, ref );
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    }
}
#endif