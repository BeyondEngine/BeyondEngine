#ifndef BEYOND_ENGINE_RENDER_SHADERUNIFORM_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SHADERUNIFORM_H__INCLUDE

#include "Component/Component/ComponentInstance.h"

enum EShaderUniformType
{
    eSUT_1i,
    eSUT_2i,
    eSUT_3i,
    eSUT_4i,

    eSUT_1f,
    eSUT_2f,
    eSUT_3f,
    eSUT_4f,

    eSUT_Matrix2f,
    eSUT_Matrix3f,
    eSUT_Matrix4f,

    eSUT_Count,
    eSUT_Force32Bit = 0xFFFFFF
};

class CShaderUniform
{
public:
    CShaderUniform();
    CShaderUniform(const TString& strName, EShaderUniformType type);
    CShaderUniform( const CShaderUniform& other );
    CShaderUniform& operator=( const CShaderUniform& other );
    virtual ~CShaderUniform();

    void SetData(const std::vector<float>& data);
    std::vector<float>& GetData();
    const TString& GetName() const;
    void SetName(const TString& strName);
    EShaderUniformType GetType() const;
    void SetType(EShaderUniformType type);

    void SendUniform() const;
    bool operator==( const CShaderUniform& other ) const;
    bool operator!=( const CShaderUniform& other ) const;
    void ResetData();

    CShaderUniform* Clone( );

private:
    bool CheckType() const;
private:
    EShaderUniformType m_type;
    //TODO: Don't use float to simulate other type of data.
    std::vector<float> m_data;
    TString m_strName;
};
#endif