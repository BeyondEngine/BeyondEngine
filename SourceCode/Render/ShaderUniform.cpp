﻿
#include "stdafx.h"
#include "ShaderUniform.h"
#include "Renderer.h"

CShaderUniform::CShaderUniform()
    : m_type(eSUT_Count)
{

}

CShaderUniform::CShaderUniform(const TString& strName, EShaderUniformType type)
    : m_strName(strName)
{
    SetType(type);
}

CShaderUniform::CShaderUniform( const CShaderUniform& other )
{
    m_type = other.m_type;
    m_strName = other.m_strName;
    m_data = other.m_data;
}

CShaderUniform::~CShaderUniform()
{
}

void CShaderUniform::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strName, true, 0xFFFFFFFF, _T("变量名"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_type, true, 0xFFFFFFFF, _T("变量类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_data, true, 0xFFFFFFFF, _T("变量数据"), NULL, NULL, NULL);
}

void CShaderUniform::SetData(const std::vector<float>& data )
{
    m_data = data;
}

std::vector<float>& CShaderUniform::GetData()
{
    return m_data;
}

const TString& CShaderUniform::GetName() const
{
    return m_strName;
}

void CShaderUniform::SetName(const TString& strName)
{
    m_strName = strName;
}

EShaderUniformType CShaderUniform::GetType() const
{
    return m_type;
}

void CShaderUniform::SetType(EShaderUniformType type)
{
    m_type = type;
    int nDataSize = 0;
    switch ( m_type )
    {
    case eSUT_1i:
    case eSUT_1f:
        nDataSize = 1;
        break;
    case eSUT_2i:
    case eSUT_2f:
        nDataSize = 2;
        break;
    case eSUT_3i:
    case eSUT_3f:
        nDataSize = 3;
        break;
    case eSUT_4i:
    case eSUT_4f:
    case eSUT_Matrix2f:
        nDataSize = 4;
        break;
    case eSUT_Matrix3f:
        nDataSize = 9;
        break;
    case eSUT_Matrix4f:
        nDataSize = 16;
        break;
    default:
        BEATS_ASSERT(false, _T("Unknown type of shader uniform!"));
        break;
    }
    m_data.resize(nDataSize);
}

void CShaderUniform::SendUniform()
{
    BEATS_ASSERT( CheckType(), _T("Shader uniform's type %d doesn't match data size %d"), m_type, m_data.size() );
    BEATS_ASSERT(!m_strName.empty(), _T("Shader uniform's name should not be empty!"));
    GLuint currProgram = CRenderer::GetInstance()->GetCurrentState()->GetShaderProgram();
    char name[ MAX_PATH ];
    CStringHelper::GetInstance()->ConvertToCHAR( m_strName.c_str(), name, MAX_PATH );
    GLint nameLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, name );
    BEATS_ASSERT(nameLocation != -1, _T("Get uniform location failed! %s"), m_strName.c_str());
    if ( -1 != nameLocation )
    {
        switch ( m_type)
        {
        case eSUT_1i:
            CRenderer::GetInstance()->SetUniform1i(nameLocation, (GLint)m_data[0]);
            break;
        case eSUT_2i:
            CRenderer::GetInstance()->SetUniform2i(nameLocation, (GLint)m_data[0], (GLint)m_data[1]);
            break;
        case eSUT_3i:
            CRenderer::GetInstance()->SetUniform3i(nameLocation, (GLint)m_data[0], (GLint)m_data[1], (GLint)m_data[2]);
            break;
        case eSUT_4i:
            CRenderer::GetInstance()->SetUniform4i(nameLocation, (GLint)m_data[0], (GLint)m_data[1], (GLint)m_data[2], (GLint)m_data[3]);
            break;
        case eSUT_1f:
            CRenderer::GetInstance()->SetUniform1f(nameLocation, m_data[0]);
            break;
        case eSUT_2f:
            CRenderer::GetInstance()->SetUniform2f(nameLocation, m_data[0], m_data[1]);
            break;
        case eSUT_3f:
            CRenderer::GetInstance()->SetUniform3f(nameLocation, m_data[0], m_data[1], m_data[2]);
            break;
        case eSUT_4f:
            CRenderer::GetInstance()->SetUniform4f(nameLocation, m_data[0], m_data[1], m_data[2], m_data[3]);
            break;
        case eSUT_Matrix2f:
            CRenderer::GetInstance()->SetUniformMatrix2fv(nameLocation, (GLfloat*)m_data.data(), 1 );
            break;
        case eSUT_Matrix3f:
            CRenderer::GetInstance()->SetUniformMatrix3fv(nameLocation, (GLfloat*)m_data.data(), 1);
            break;
        case eSUT_Matrix4f:
            CRenderer::GetInstance()->SetUniformMatrix4fv(nameLocation, (GLfloat*)m_data.data(), 1);
            break;
        default:
            BEATS_ASSERT(false, _T("Unknown Type of SShaderUniform"));
            break;
        }
    }
}

bool CShaderUniform::CheckType()
{
    size_t size = 0;
    switch ( m_type )
    {
    case eSUT_1i:
    case eSUT_1f:
        size = 1;
        break;
    case eSUT_2i:
    case eSUT_2f:
        size = 2;
        break;
    case eSUT_3i:
    case eSUT_3f:
        size = 3;
        break;
    case eSUT_4i:
    case eSUT_4f:
    case eSUT_Matrix2f:
        size = 4;
        break;
    case eSUT_Matrix3f:
        size = 9;
        break;
    case eSUT_Matrix4f:
        size = 16;
        break;
    default:
        BEATS_ASSERT(false, _T("Unknown type of shader uniform!"));
        break;
    }

    return (size == m_data.size());
}

bool CShaderUniform::operator==( const CShaderUniform& other ) const
{
    return ( m_strName == other.m_strName && m_type == other.m_type && ComparePtrVector( m_data, other.m_data ));
}

bool CShaderUniform::operator!=( const CShaderUniform& other ) const
{
    return !(*this == other);
}

bool CShaderUniform::ComparePtrVector( const std::vector<float> & v1, const std::vector<float> & v2 ) const
{
    bool bReturn = true;
    if ( v1.size() == v2.size() )
    {
        for ( size_t i = 0; i < v1.size(); ++i )
        {
            if ( v1[ i ] != v2[ i ] )
            {
                bReturn = false;
                break;
            }
        }
    }
    else
    {
        bReturn = false;
    }
    return bReturn;
}

const CShaderUniform& CShaderUniform::operator=( const CShaderUniform& other )
{
    if ( this != &other )
    {
        m_type = other.m_type;
        m_strName = other.m_strName;
        m_data = other.m_data;
    }
    return *this;
}

void CShaderUniform::ResetData()
{
    m_data.clear();
}

CShaderUniform* CShaderUniform::Clone( )
{
    CShaderUniform* uniform = new CShaderUniform();
    uniform->m_strName = m_strName;
    uniform->m_type = m_type;
    for ( auto f : m_data )
    {
        uniform->m_data.push_back( f );
    }
    return uniform;
}
