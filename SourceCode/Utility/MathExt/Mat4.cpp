#include "stdafx.h"
#include "Mat4.h"
#include "gtc/type_ptr.hpp"
#include "gtx/euler_angles.hpp"
#include "gtx/matrix_interpolation.hpp"
#include "gtx/matrix_decompose.hpp"
#include "Location.h"

CMat4::CMat4()
{

}

CMat4::CMat4(float* pData)
{
    memcpy(Mat4ValuePtr(), pData, sizeof(float) * 16);
}

CMat4::CMat4(const CLocation& location)
{
    Build(location.m_pos, location.m_rotation, location.m_scale);
}

float* CMat4::Mat4ValuePtr() const
{
    return (float*)glm::value_ptr(m_data);
}

bool CMat4::IsIdentity() const
{
    bool bRet = BEATS_FLOAT_EQUAL_EPSILON(m_data[0].x, 1.0f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[0].y, 0.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[0].z, 0.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[0].w, 0.f, 0.0001f) &&

        BEATS_FLOAT_EQUAL_EPSILON(m_data[1].x, 0.0f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[1].y, 1.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[1].z, 0.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[1].w, 0.f, 0.0001f) &&

        BEATS_FLOAT_EQUAL_EPSILON(m_data[2].x, 0.0f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[2].y, 0.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[2].z, 1.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[2].w, 0.f, 0.0001f) &&

        BEATS_FLOAT_EQUAL_EPSILON(m_data[3].x, 0.0f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[3].y, 0.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[3].z, 0.f, 0.0001f) &&
        BEATS_FLOAT_EQUAL_EPSILON(m_data[3].w, 1.f, 0.0001f);
    return bRet;
}

void CMat4::Identity()
{
    m_data = glm::mat4();
}

void CMat4::Inverse()
{
    m_data = glm::inverse(m_data);
}

void CMat4::Transpose()
{
    m_data = glm::transpose(m_data);
}

bool CMat4::IsOrthogonal() const
{
    CMat4 transposeMat = *this;
    transposeMat.Transpose();
    transposeMat = *this * transposeMat;
    return transposeMat.IsIdentity();
}

float& CMat4::operator[](uint32_t uIndex) const
{
    return Mat4ValuePtr()[uIndex];
}

CMat4 CMat4::operator * (const CMat4& rhs) const
{
    glm::mat4 ret = m_data * rhs.m_data;
    return CMat4(glm::value_ptr(ret));
}

CVec2 CMat4::operator * (const CVec2& rhs) const
{
    return rhs * (*this);
}

CVec3 CMat4::operator * (const CVec3& rhs) const
{
    return rhs * (*this);
}

CVec4 CMat4::operator * (const CVec4& rhs) const
{
    glm::vec4 temp = m_data * rhs.m_data;
    return CVec4(temp.x, temp.y, temp.z, temp.w);
}

bool CMat4::operator == (const CMat4& rhs) const
{
    return m_data == rhs.m_data;
}

bool CMat4::operator != (const CMat4& rhs) const
{
    return m_data != rhs.m_data;
}

void CMat4::FromPitchYawRoll(float pitch, float yaw, float roll)
{
    m_data = glm::yawPitchRoll(yaw, pitch, roll);
}

void CMat4::FromQuaternion(const CQuaternion* pQ)
{
    m_data = glm::mat4_cast(pQ->m_data);
}

void CMat4::SetTranslate(const CVec3& translate)
{
    (*this)[12] = translate.X();
    (*this)[13] = translate.Y();
    (*this)[14] = translate.Z();
}

CVec3 CMat4::GetUpVec3() const
{
    CVec3 vec3;
    vec3.X() = (*this)[4];
    vec3.Y() = (*this)[5];
    vec3.Z() = (*this)[6];
    vec3.Normalize();
    return vec3;
}

CVec3 CMat4::GetRightVec3() const
{
    CVec3 vec3;
    vec3.X() = (*this)[0];
    vec3.Y() = (*this)[1];
    vec3.Z() = (*this)[2];
    vec3.Normalize();
    return vec3;
}

CVec3 CMat4::GetForwardVec3() const
{
    CVec3 vec3;
    vec3.X() = (*this)[8];
    vec3.Y() = (*this)[9];
    vec3.Z() = (*this)[10];
    vec3.Normalize();
    return vec3;
}

CVec3 CMat4::GetTranslate() const
{
    return CVec3((*this)[12], (*this)[13], (*this)[14]);
}

CVec3 CMat4::GetScale() const
{
    CVec3 ret;
    ret.X() = glm::length(m_data[0]);
    ret.Y() = glm::length(m_data[1]);
    ret.Z() = glm::length(m_data[2]);
    return ret;
}

void CMat4::PerspectiveProjection(float fovDegree, float aspect, float zNear, float zFar)
{
    m_data = glm::perspective(DegreesToRadians(fovDegree), aspect, zNear, zFar);
}

void CMat4::OrthographicProjection(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    m_data = glm::ortho(left, right, bottom, top, nearVal, farVal);
}

void CMat4::LookAt(const CVec3& eye, const CVec3& center, const CVec3& up)
{
    const glm::vec3 eyeRet(eye.X(), eye.Y(), eye.Z());
    const glm::vec3 centerRet(center.X(), center.Y(), center.Z());
    const glm::vec3 upRet(up.X(), up.Y(), up.Z());
    m_data = glm::lookAt(eyeRet, centerRet, upRet);
}

void CMat4::FromAxisAngle(const CVec3& axis, float radians)
{
    CVec3 nAxis = axis;
    nAxis.Normalize();
    m_data = glm::axisAngleMatrix(nAxis.m_data, radians);
}

void CMat4::ToAxisAngle(CVec3 *pAxis, float *radians) const
{
    CMat4 tmp = *this;
    tmp.RemoveScale();
    tmp.RemoveTranslate();
    glm::axisAngle(tmp.m_data, pAxis->m_data, *radians);
}

CQuaternion CMat4::ToQuaternion() const
{
    CQuaternion quat;
    quat.FromMatrix(*this);
    return quat;
}

bool CMat4::Decompose(CVec3& translate, CQuaternion& rotate, CVec3& scale) const
{
    glm::vec3 skew;
    glm::vec4 perspective;
    return glm::decompose(m_data, scale.m_data, rotate.m_data, translate.m_data, skew, perspective);
}

void CMat4::Build(const CVec3& translate, const CQuaternion& rotate, const CVec3& scale)
{
    m_data = glm::mat4_cast(rotate.m_data);
    m_data[0] *= scale.X();
    m_data[1] *= scale.Y();
    m_data[2] *= scale.Z();
    m_data[3] = glm::vec4(translate.m_data, 1.0);
}

void CMat4::Build(const CVec3& translate, const CVec3& rotate, const CVec3& scale)
{
    CQuaternion rotation;
    rotation.FromPitchYawRoll(rotate.X(), rotate.Y(), rotate.Z());
    Build(translate, rotation, scale);
}

void CMat4::RemoveScale()
{
    if (!BEATS_FLOAT_EQUAL(m_data[0].x, 0) ||
        !BEATS_FLOAT_EQUAL(m_data[0].y, 0) ||
        !BEATS_FLOAT_EQUAL(m_data[0].z, 0))
    {
        m_data[0] = glm::normalize(m_data[0]);
    }
    if (!BEATS_FLOAT_EQUAL(m_data[1].x, 0) ||
        !BEATS_FLOAT_EQUAL(m_data[1].y, 0) ||
        !BEATS_FLOAT_EQUAL(m_data[1].z, 0))
    {
        m_data[1] = glm::normalize(m_data[1]);
    }
    if (!BEATS_FLOAT_EQUAL(m_data[2].x, 0) ||
        !BEATS_FLOAT_EQUAL(m_data[2].y, 0) ||
        !BEATS_FLOAT_EQUAL(m_data[2].z, 0))
    {
        m_data[2] = glm::normalize(m_data[2]);
    }
}

void CMat4::RemoveTranslate()
{
    (*this)[12] = 0;
    (*this)[13] = 0;
    (*this)[14] = 0;
}

void CMat4::RemoveRotate()
{
    CVec3 scale = GetScale();
    (*this)[0] = scale.X();
    (*this)[1] = 0;
    (*this)[2] = 0;
    (*this)[3] = 0;

    (*this)[4] = 0;
    (*this)[5] = scale.Y();
    (*this)[6] = 0;
    (*this)[7] = 0;

    (*this)[8] = 0;
    (*this)[9] = 0;
    (*this)[10] = scale.Z();
    (*this)[11] = 0;
}

const void CMat4::PrintValue() const
{
    BEATS_PRINT("Print CMat4 Value Start : ");
    BEATS_PRINT("%f, %f, %f, %f ", (*this)[0], (*this)[1], (*this)[2], (*this)[3]);
    BEATS_PRINT("%f, %f, %f, %f ", (*this)[4], (*this)[5], (*this)[6], (*this)[7]);
    BEATS_PRINT("%f, %f, %f, %f ", (*this)[8], (*this)[9], (*this)[10], (*this)[11]);
    BEATS_PRINT("%f, %f, %f, %f \n", (*this)[12], (*this)[13], (*this)[14], (*this)[15]);
}