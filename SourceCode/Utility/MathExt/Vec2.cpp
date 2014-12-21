#include "stdafx.h"
#include "Vec2.h"

CVec2::CVec2()
{

}

CVec2::CVec2(float x, float y)
{
    Fill(x, y);
}

CVec2& CVec2::operator = (const CVec2& rhs)
{
    m_data = rhs.m_data;
    return *this;
}

void CVec2::Fill(float x, float y)
{
    m_data.x = x;
    m_data.y = y;
}

float CVec2::Length() const
{
    return glm::length(m_data);
}

void CVec2::Normalize()
{
    m_data = glm::normalize(m_data);
}

float CVec2::Dot(const CVec2& rhs)
{
    return glm::dot(m_data, rhs.m_data);
}

CVec2 CVec2::operator + (const CVec2& rhs) const
{
    glm::vec2 ret = m_data + rhs.m_data;
    return CVec2(ret.x, ret.y);
}

CVec2 CVec2::operator - (const CVec2& rhs) const
{
    glm::vec2 ret = m_data - rhs.m_data;
    return CVec2(ret.x, ret.y);
}

CVec2 CVec2::operator - () const
{
    glm::vec2 ret = - m_data;
    return CVec2(ret.x, ret.y);
}

CVec2 CVec2::operator * (const CVec2& rhs) const
{
    glm::vec2 ret = m_data * rhs.m_data;
    return CVec2(ret.x, ret.y);
}

CVec2 CVec2::operator * (float rhs) const
{
    glm::vec2 ret = m_data * rhs;
    return CVec2(ret.x, ret.y);
}

CVec2& CVec2::operator *= (const CVec2& rhs)
{
    m_data = m_data * rhs.m_data;
    return *this;
}

CVec2& CVec2::operator *= (float rhs)
{
    m_data = m_data * rhs;
    return *this;
}

CVec2 CVec2::operator * (const CMat4& mat) const
{
    CVec2 ret = *this;
    ret *= mat;
    return ret;
}

CVec2& CVec2::operator *= (const CMat4& mat)
{
    glm::vec4 vec4Data(m_data, 0, 1.0f);
    vec4Data = mat.m_data * vec4Data;
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(vec4Data.w, 1.0f));
    m_data.x = vec4Data.x;
    m_data.y = vec4Data.y;
    return *this;
}

CVec2& CVec2::operator += (const CVec2& rhs)
{
	m_data = m_data + rhs.m_data;
	return *this;
}

bool CVec2::operator == (const CVec2& rhs) const
{
    return BEATS_FLOAT_EQUAL(m_data.x, rhs.m_data.x) && BEATS_FLOAT_EQUAL(m_data.y, rhs.m_data.y);
}

bool CVec2::operator != (const CVec2& rhs) const
{
    return !(*this == rhs);
}

float& CVec2::X()
{
    return m_data.x;
}

const float& CVec2::X() const
{
    return m_data.x;
}

float& CVec2::Y()
{
    return m_data.y;
}

const float& CVec2::Y() const
{
    return m_data.y;
}
