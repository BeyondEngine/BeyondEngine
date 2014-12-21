#include "stdafx.h"
#include "Vec3.h"

CVec3::CVec3(float x, float y, float z)
{
    Fill(x, y, z);
    BEATS_ASSERT(!IsNAN());
}

CVec3::CVec3(const CVec2& vec2, float z)
{
    Fill(vec2.X(), vec2.Y(), z);
    BEATS_ASSERT(!IsNAN());
}

CVec3& CVec3::operator = (const CVec3& rhs)
{
    m_data = rhs.m_data;
    BEATS_ASSERT(!IsNAN());
    return *this;
}

void CVec3::Fill(float x, float y, float z)
{
    m_data.x = x;
    m_data.y = y;
    m_data.z = z;
    BEATS_ASSERT(!IsNAN());
}

void CVec3::Zero()
{
    Fill(0.f, 0.f, 0.f);
}

float CVec3::Length() const
{
    return glm::length(m_data);
}

float CVec3::LengthSq() const
{
    return glm::dot(m_data, m_data);
}

void CVec3::Normalize()
{
    BEATS_ASSERT(LengthSq() > 0, "Can't normalize a zero vec3");
    m_data = glm::normalize(m_data);
    BEATS_ASSERT(!IsNAN());
}

bool CVec3::IsNormalized() const
{
    float fLength = Length();
    return BEATS_FLOAT_EQUAL_EPSILON(fLength, 1.0f, 0.001f);
}

bool CVec3::IsNAN() const
{
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    return _isnan(m_data.x) || _isnan(m_data.y) || _isnan(m_data.z);
#else
    return false;
#endif
}

CVec3 CVec3::Cross(const CVec3& rhs) const
{
    glm::vec3 ret = glm::cross(m_data, rhs.m_data);
    return CVec3(ret.x, ret.y, ret.z);
}

float CVec3::Dot(const CVec3& rhs) const
{
    return glm::dot(m_data, rhs.m_data);
}

CVec3 CVec3::Lerp(const CVec3& rhs, float fScalar) const
{
    CVec3 ret = rhs - *this;
    ret *= fScalar;
    ret += *this;
    BEATS_ASSERT(!IsNAN());
    return ret;
}

CVec3 CVec3::operator + (const CVec3& rhs) const
{
    glm::vec3 ret = m_data + rhs.m_data;
    BEATS_ASSERT(!IsNAN());
    return CVec3(ret.x, ret.y, ret.z);
}

CVec3 CVec3::operator - (const CVec3& rhs) const
{
    glm::vec3 ret = m_data - rhs.m_data;
    BEATS_ASSERT(!IsNAN());
    return CVec3(ret.x, ret.y, ret.z);
}

CVec3& CVec3::operator += (const CVec3& rhs)
{
    m_data = m_data + rhs.m_data;
    BEATS_ASSERT(!IsNAN());
    return *this;
}

CVec3& CVec3::operator -= (const CVec3& rhs)
{
    m_data = m_data - rhs.m_data;
    BEATS_ASSERT(!IsNAN());
    return *this;
}

CVec3 CVec3::operator * (float rhs) const
{
    glm::vec3 ret = m_data * rhs;
    BEATS_ASSERT(!IsNAN());
    return CVec3(ret.x, ret.y, ret.z);
}

CVec3& CVec3::operator *= (float rhs)
{
    m_data *= rhs;
    BEATS_ASSERT(!IsNAN());
    return *this;
}

CVec3 CVec3::operator * (const CVec3& rhs) const
{
    CVec3 ret = *this;
    ret.X() *= rhs.X();
    ret.Y() *= rhs.Y();
    ret.Z() *= rhs.Z();
    BEATS_ASSERT(!ret.IsNAN());
    return ret;
}

CVec3& CVec3::operator *= (const CVec3& rhs)
{
    *this = *this * rhs;
    BEATS_ASSERT(!IsNAN());
    return *this;
}

CVec3 CVec3::operator * (const CQuaternion& quat) const
{
    // Do not change the order.
    glm::vec3 tmp = quat.m_data * m_data;
    CVec3 ret(tmp.x, tmp.y, tmp.z);
    BEATS_ASSERT(!ret.IsNAN());
    return ret;
}

CVec3& CVec3::operator *= (const CQuaternion& quat)
{
    m_data = quat.m_data * m_data;
    BEATS_ASSERT(!IsNAN());
    return *this;
}

CVec3 CVec3::operator * (const CMat4& mat) const
{
    CVec3 ret = *this;
    ret *= mat;
    BEATS_ASSERT(!ret.IsNAN());
    return ret;
}

CVec3& CVec3::operator *= (const CMat4& mat)
{
    glm::vec4 vec4Data(m_data, 1.0f);
    vec4Data = mat.m_data * vec4Data;
    BEATS_ASSERT(!BEATS_FLOAT_EQUAL(vec4Data.w, 0));
    m_data.x = vec4Data.x / vec4Data.w;
    m_data.y = vec4Data.y / vec4Data.w;
    m_data.z = vec4Data.z / vec4Data.w;
    BEATS_ASSERT(!IsNAN());
    return *this;
}

bool CVec3::operator == (const CVec3& rhs) const
{
    return BEATS_FLOAT_EQUAL_EPSILON(m_data.x, rhs.m_data.x, 0.0001f) && BEATS_FLOAT_EQUAL_EPSILON(m_data.y, rhs.m_data.y, 0.0001f) && BEATS_FLOAT_EQUAL_EPSILON(m_data.z, rhs.m_data.z, 0.0001f);
}

bool CVec3::operator != (const CVec3& rhs) const
{
    return !(*this == rhs);
}

bool CVec3::operator < (const CVec3& rhs) const
{
    return BEATS_FLOAT_LESS_EPSILON(m_data.x, rhs.m_data.x, 0.0001f) && BEATS_FLOAT_LESS_EPSILON(m_data.y, rhs.m_data.y, 0.0001f) && BEATS_FLOAT_LESS_EPSILON(m_data.z, rhs.m_data.z, 0.0001f);
}

bool CVec3::operator > (const CVec3& rhs) const
{
    return BEATS_FLOAT_GREATER_EPSILON(m_data.x, rhs.m_data.x, 0.0001f) && BEATS_FLOAT_GREATER_EPSILON(m_data.y, rhs.m_data.y, 0.0001f) && BEATS_FLOAT_GREATER_EPSILON(m_data.z, rhs.m_data.z, 0.0001f);
}

float& CVec3::X()
{
    return m_data.x;
}

const float& CVec3::X() const
{
    return m_data.x;
}

float& CVec3::Y()
{
    return m_data.y;
}

const float& CVec3::Y() const
{
    return m_data.y;
}

float& CVec3::Z()
{
    return m_data.z;
}

const float& CVec3::Z() const
{
    return m_data.z;
}