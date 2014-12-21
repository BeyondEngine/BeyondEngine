#include "stdafx.h"
#include "Vec4.h"

CVec4::CVec4(float x, float y, float z, float w)
{
    Fill(x, y, z, w);
}

CVec4::CVec4(const CVec2& vec2, float z, float w)
{
    Fill(vec2.X(), vec2.Y(), z, w);
}

CVec4::CVec4(const CVec3& vec3, float w)
{
    Fill(vec3.X(), vec3.Y(), vec3.Z(), w);
}

CVec4& CVec4::operator = (const CVec4& rhs)
{
    m_data = rhs.m_data;
    return *this;
}

CVec4 CVec4::operator * (const CMat4& mat) const
{
    glm::vec4 ret = mat.m_data * m_data;
    return CVec4(ret.x, ret.y, ret.z, ret.w);
}

CVec4& CVec4::operator *= (const CMat4& mat)
{
    m_data = mat.m_data * m_data;
    return (*this);
}

CVec4 CVec4::operator * (float rhs) const
{
    glm::vec4 ret = m_data * rhs;
    return CVec4(ret.x, ret.y, ret.z, ret.w);
}

CVec4& CVec4::operator *= (float rhs)
{
    m_data *= rhs;
    return *this;
}

bool CVec4::operator == (const CVec4& rhs) const
{
    return m_data == rhs.m_data;
}

bool CVec4::operator != (const CVec4& rhs) const
{
    return m_data != rhs.m_data;
}

CVec4 CVec4::operator + (const CVec4& rhs) const
{
    glm::vec4 ret = m_data + rhs.m_data;
    return CVec4(ret.x, ret.y, ret.z, ret.w);
}

CVec4 CVec4::operator - (const CVec4& rhs) const
{
    glm::vec4 ret = m_data - rhs.m_data;
    return CVec4(ret.x, ret.y, ret.z, ret.w);
}

CVec4& CVec4::operator += (const CVec4& rhs)
{
    m_data = m_data + rhs.m_data;
    return *this;
}

CVec4& CVec4::operator -= (const CVec4& rhs)
{
    m_data = m_data - rhs.m_data;
    return *this;
}

void CVec4::Fill(float x, float y, float z, float w)
{
    m_data.x = x;
    m_data.y = y;
    m_data.z = z;
    m_data.w = w;
}

float& CVec4::X()
{
    return m_data.x;
}

const float& CVec4::X() const
{
    return m_data.x;
}

float& CVec4::Y()
{
    return m_data.y;
}

const float& CVec4::Y() const
{
    return m_data.y;
}

float& CVec4::Z()
{
    return m_data.z;
}

const float& CVec4::Z() const
{
    return m_data.z;
}

float& CVec4::W()
{
    return m_data.w;
}

const float& CVec4::W() const
{
    return m_data.w;
}

CVec4 CVec4::Interpolate(const CVec4& rhs, float fRate)
{
    CVec4 ret = rhs - *this;
    ret *= fRate;
    ret += *this;
    return ret;
}