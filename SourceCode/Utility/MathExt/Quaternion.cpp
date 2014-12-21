#include "stdafx.h"
#include "Quaternion.h"

void CQuaternion::FromPitchYawRoll(float fPitch, float fYaw, float fRoll)
{
    m_data = glm::quat(glm::vec3(fPitch, fYaw, fRoll));
}

void CQuaternion::ToPitchYawRoll(float& fPitch, float& fYaw, float& fRoll) const
{
    glm::vec3 ret = glm::eulerAngles(m_data);
    fPitch = ret.x;
    fYaw = ret.y;
    fRoll = ret.z;
}

void CQuaternion::FromAxisAngle(const CVec3& axis, float angle)
{
    CVec3 normalizeAxis = axis;
    normalizeAxis.Normalize();
    m_data = glm::angleAxis(angle, normalizeAxis.m_data);
}

void CQuaternion::ToAxisAngle(CVec3& axis, float& angle) const
{
    angle = glm::angle(m_data);
    axis.m_data = glm::axis(m_data);
}

void CQuaternion::FromMatrix(const CMat4& mat)
{
    CMat4 matTmp = mat;
    matTmp.RemoveScale();
    m_data = glm::quat_cast(matTmp.m_data);
}

CQuaternion CQuaternion::Inverse() const
{
    CQuaternion ret;
    ret.m_data.x = -1.0f * m_data.x;
    ret.m_data.y = -1.0f * m_data.y;
    ret.m_data.z = -1.0f * m_data.z;
    ret.m_data.w = m_data.w;
    return ret;
}

float& CQuaternion::X()
{
    return m_data.x;
}

const float& CQuaternion::X() const
{
    return m_data.x;
}

float& CQuaternion::Y()
{
    return m_data.y;
}

const float& CQuaternion::Y() const
{
    return m_data.y;
}

float& CQuaternion::Z()
{
    return m_data.z;
}

const float& CQuaternion::Z() const
{
    return m_data.z;
}

float& CQuaternion::W()
{
    return m_data.w;
}

const float& CQuaternion::W() const
{
    return m_data.w;
}

bool CQuaternion::operator == (const CQuaternion& rhs) const
{
    // if it is the same, the checkQuart will be (0,0,0,1)
    CQuaternion checkQuart = Inverse() * rhs;
    return BEATS_FLOAT_EQUAL_EPSILON(checkQuart.m_data.x, 0, 0.0001f) && BEATS_FLOAT_EQUAL_EPSILON(checkQuart.m_data.y, 0, 0.0001f) && BEATS_FLOAT_EQUAL_EPSILON(checkQuart.m_data.z, 0, 0.0001f) && BEATS_FLOAT_EQUAL_EPSILON(fabs(checkQuart.m_data.w), 1.0f, 0.0001f);
}

bool CQuaternion::operator != (const CQuaternion& rhs) const
{
    return !(*this == rhs);
}

CQuaternion CQuaternion::operator * (const CQuaternion& rhs) const
{
    CQuaternion ret;
    ret.m_data = m_data * rhs.m_data;
    return ret;
}

CQuaternion CQuaternion::Slerp(const CQuaternion& dest, float fFactor) const
{
    CQuaternion ret;
    BEATS_CLIP_VALUE(fFactor, 0, 1.0f);
    ret.m_data = glm::slerp(m_data, dest.m_data, fFactor);
    return ret;
}
