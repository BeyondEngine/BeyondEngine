#ifndef BEYOND_ENGINE_RENDER_SHADERPROGRAM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SHADERPROGRAM_H__INCLUDE

#include "Utility/BeatsUtility/Serializer.h"

class CShaderProgram
{
public:
    CShaderProgram(GLuint uVertexShader, GLuint uPixelShader);
    ~CShaderProgram();
    
    GLuint ID();

    GLuint GetVertexShader() const;
    GLuint GetPixelShader() const;

private:
    bool Link();

private:
    GLuint m_uProgram;
    GLuint m_uVertexShader;
    GLuint m_uPixelShader;
};

#endif