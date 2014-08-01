#ifndef BEYOND_ENGINE_RENDER_SHADER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SHADER_H__INCLUDE

#include "resource/Resource.h"
#include "Utility/BeatsUtility/Serializer.h"

class CShader : public CResource
{
    DECLARE_REFLECT_GUID(CShader, 0x447D02CD, CResource)
    DECLARE_RESOURCE_TYPE(eRT_Shader)

public:
    CShader();
    virtual ~CShader();

    GLenum GetShaderType()const;
    void SetShaderType(GLenum shaderType);

    virtual bool Load() override;
    virtual void Initialize() override;

    GLuint ID() const;

private:
    bool Compile(GLuint * shader, const GLchar* pszSource, GLint length);

private:
    GLenum m_shaderType;
    GLuint m_uId;
    static CSerializer m_commonHeader;
    static CSerializer m_vsHeader;
    static CSerializer m_psHeader;
};

#endif