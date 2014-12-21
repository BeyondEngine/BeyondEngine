#ifndef BEYOND_ENGINE_RENDER_SHADER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SHADER_H__INCLUDE

#include "Resource/Resource.h"
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
    virtual bool Unload() override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual bool ShouldClean() const override;
    GLuint ID() const;

private:
    bool Compile(GLuint * shader, const GLchar* pszSource, GLint length);

private:
    GLenum m_shaderType;
    GLuint m_uId;
    static CSerializer m_commonHeader;
};

#endif