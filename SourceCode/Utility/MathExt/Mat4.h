#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_MAT4_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_MAT4_H__INCLUDE

#include "kazmath/plane.h"
#include "kazmath/mat3.h"

struct CMat4 : public kmMat4
{
    void Fill(const float* pMat)
    {
        kmMat4Fill(this, pMat);
    }
    void Identity()
    {
        kmMat4Identity(this);
    }
    void Inverse()
    {
        kmMat4Inverse(this, this);
    }
    static CMat4 Inverse(const CMat4 &mat4)
    {
        CMat4 mat4Inversed;
        kmMat4Inverse(&mat4Inversed, &mat4);
        return mat4Inversed;
    }
    bool IsIdentity() const
    {
        return kmMat4IsIdentity(this) == 1;
    }
    void Transpose()
    {
        kmMat4Transpose(this, this);
    }
    static CMat4 Transpose(const CMat4 &mat4)
    {
        CMat4 mat4Transposed;
        kmMat4Transpose(&mat4Transposed, &mat4);
        return mat4Transposed;
    }
    friend CMat4 operator * (const CMat4 &lhs, const CMat4 &rhs)
    {
        CMat4 mat4Result;
        kmMat4Multiply(&mat4Result, &lhs, &rhs);
        return mat4Result;
    }
    void operator *= (const CMat4 &rhs)
    {
        kmMat4Multiply(this, this, &rhs);
    }
    void operator = (const kmMat4 &rhs)
    {
        kmMat4Assign(this, &rhs);
    }
    friend bool operator == (const CMat4 &lhs, const CMat4 &rhs)
    {
        return kmMat4AreEqual(&lhs, &rhs) == 1;
    }
    void RotationX(float radians)
    {
        kmMat4RotationX(this, radians);
    }
    void RotationY(float radians)
    {
        kmMat4RotationY(this, radians);
    }
    void RotationZ(float radians)
    {
        kmMat4RotationZ(this, radians);
    }
    void RotationPitchYawRoll(float pitch, float yaw, float roll)
    {
        kmMat4RotationPitchYawRoll(this, pitch, yaw, roll);
    }
    void RotationQuaternion(const kmQuaternion* pQ)
    {
        kmMat4RotationQuaternion(this, pQ);
    }
    void RotationTranslation(const kmMat3 &rotation, const kmVec3 &translation)
    {
        kmMat4RotationTranslation(this, &rotation, &translation);
    }
    void Scaling(float x, float y, float z)
    {
        kmMat4Scaling(this, x, y, z);
    }
    void Translation(float x, float y, float z)
    {
        kmMat4Translation(this, x, y, z);
    }
    CVec3 GetUpVec3() const
    {
        CVec3 vec3;
        kmMat4GetUpVec3(&vec3, this);
        return vec3;
    }
    CVec3 GetRightVec3() const
    {
        CVec3 vec3;
        kmMat4GetRightVec3(&vec3, this);
        return vec3;
    }
    CVec3 GetForwardVec3() const
    {
        CVec3 vec3;
        kmMat4GetForwardVec3(&vec3, this);
        return vec3;
    }
    void PerspectiveProjection(float fovY, float aspect, float zNear, float zFar)
    {
        kmMat4PerspectiveProjection(this, fovY, aspect, zNear, zFar);
    }
    void OrthographicProjection(float left, float right, float bottom, float top, float nearVal, float farVal)
    {
        kmMat4OrthographicProjection(this, left, right, bottom, top, nearVal, farVal);
    }
    void LookAt(const kmVec3 &eye, const kmVec3 &center, const kmVec3 &up)
    {
        kmMat4LookAt(this, &eye, &center, &up);
    }
    void RotationAxisAngle(const kmVec3 &axis, float radians)
    {
        kmMat4RotationAxisAngle(this, &axis, radians);
    }
    kmMat3 ExtractRotation()
    {
        kmMat3 rotation;
        kmMat4ExtractRotation(&rotation, this);
        return rotation;
    }
    kmPlane ExtractPlane(kmEnum plane)
    {
        kmPlane ret;
        kmMat4ExtractPlane(&ret, this, plane);
        return ret;
    }
    void RotationToAxisAngle(kmVec3 *pAxis, float *radians) const
    {
        kmMat4RotationToAxisAngle(pAxis, radians, this);
    }
};

#endif