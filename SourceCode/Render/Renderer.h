#ifndef BEYOND_ENGINE_RENDER_RENDERER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERER_H__INCLUDE

#include "RenderState.h"

class CRenderer
{
    BEATS_DECLARE_SINGLETON(CRenderer);
public:

    //Shader
    void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params);
    void UseProgram(GLuint uProgram);
    GLuint CreateShader(GLenum type);
    void DeleteShader(GLuint uProgram);
    void ShaderSource(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
    void CompileShader(GLuint shader);
    void GetProgramiv(GLuint program, GLenum pname, GLint* param);
    void GetShaderiv(GLuint shader, GLenum pname, GLint* param);
    void GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
    void LinkProgram(GLuint program);
    void AttachShader(GLuint program, GLuint shader);
    void DetachShader(GLuint program, GLuint shader);
    GLuint CreateProgram();
    void DeleteProgram(GLuint uProgram);
    void BindAttribLocation(GLuint uProgram, ECommonAttribIndex attribIndex);
    GLint GetUniformLocation(GLuint uProgram, const char *name) const;
    void GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
    void SetUniform1i(GLint location, GLint i1);
    void SetUniform2i(GLint location, GLint i1, GLint i2);
    void SetUniform3i(GLint location, GLint i1, GLint i2, GLint i3);
    void SetUniform4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4);

    void SetUniform1f(GLint location, GLfloat f1);
    void SetUniform2f(GLint location, GLfloat f1, GLfloat f2);
    void SetUniform3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3);
    void SetUniform4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);
    void SetUniform1fv(GLint location, const GLfloat *floats, GLsizei numberOfFloats);
    void SetUniform2fv(GLint location, const GLfloat *floats, GLsizei numberOfFloats);
    void SetUniform3fv(GLint location, const GLfloat *floats, GLsizei numberOfFloats);
    void SetUniform4fv(GLint location, const GLfloat *floats, GLsizei numberOfFloats);
    void SetUniform1iv(GLint location, const GLint *ints, GLsizei numberOfInt);
    void SetUniform2iv(GLint location, const GLint *ints, GLsizei numberOfInt);
    void SetUniform3iv(GLint location, const GLint *ints, GLsizei numberOfInt);
    void SetUniform4iv(GLint location, const GLint *ints, GLsizei numberOfInt);
    void SetUniformMatrix2fv(GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices);
    void SetUniformMatrix3fv(GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices);
    void SetUniformMatrix4fv(GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices);
    void UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

    void DrawArrays(GLenum mode, GLint first, GLsizei count);
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* pIndices );

    //buffer
    void GenVertexArrays(GLsizei n, GLuint* arrays);
    void BindVertexArray(GLuint array);
    void DeleteVertexArrays(GLsizei n, GLuint *arrays);
    void GenBuffers(GLsizei n, GLuint* buffers);
    void BindBuffer(GLenum target, GLuint buffer);
    void BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
    void DeleteBuffers(GLsizei n, GLuint *buffers);
    void EnableVertexAttribArray(GLuint attribute);
    void DisableVertexAttribArray(GLuint attribute);
    void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
    void VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

    //Texture
    void ActiveTexture(GLenum texture);
    void GenTextures(GLsizei n, GLuint *textures);
    void BindTexture(GLenum target, GLuint texture);
    void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
    void TextureImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    void TextureSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    void TexParameteri(GLenum target, GLenum pname, GLint param);
    void DeleteTexture(GLsizei n, const GLuint* textures);

    //FBO
    void GenFramebuffers(GLsizei n, GLuint* framebuffers);
    void BindFramebuffer(GLenum target, GLuint framebuffer);
    void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    void DeleteFramebuffer(GLsizei n, const GLuint * framebuffers);

    //RBO
    void GenRenderbuffers(GLsizei n, GLuint *renderbuffers);
    void BindRenderbuffer(GLenum target,  GLuint renderbuffer);
    void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    void DeleteRenderbuffer(GLsizei n, const GLuint* renderbuffers);

    void GetIntegerV(GLenum pname, GLint * params );
    void GetFloatV(GLenum pname, GLfloat * params );
    void PixelStorei (GLenum pname, GLint param);

    void Viewport(GLint x, GLint y, GLsizei width, GLsizei height);
    void SetScissorRect(GLint x, GLint y, GLsizei width, GLsizei height);

    void EnableGL(CBoolRenderStateParam::EBoolStateParam cap);
    void DisableGL(CBoolRenderStateParam::EBoolStateParam cap);
    bool IsEnable(CBoolRenderStateParam::EBoolStateParam cap);
    void BlendFunc(GLenum sfactor, GLenum dfactor);
    void BlendEquation(GLenum func);
    void BlendColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
    void PolygonMode (GLenum face, GLenum mode);
    void ClearBuffer(GLbitfield bit);
    void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void ClearDepth(GLclampf depth);
    void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
    GLvoid* MapBuffer(GLenum target, GLenum access);
    void UnmapBuffer(GLenum target);
    void DepthMask(bool bWriteable);
    void EdgeFlag(bool bEdgeFlag);
    void FrontFace(GLenum frontFace);
    void CullFace(GLenum cullFace);

    void LineWidth(GLfloat fLineWidth);
    void PointSize(GLfloat fPointSize);
    void DepthRange(float fNear, float fFar);
    void DepthFunc(GLenum depthFunc);

    void StencilFunc(GLenum stencilFunc);
    void StencilReference(GLint nRef);
    void StencilValueMask(GLint nValueMask);
    void ClearStencil(GLint nClearValue);
    void StencilOp(GLenum fail, GLenum zFail, GLenum zPass);

    void ShadeModel(GLenum shadeModel);

    void AlphaFunc(GLenum func, GLclampf ref );

    CRenderState* GetCurrentState() const;

private:
    CRenderState* m_pCurrState;
};

#endif