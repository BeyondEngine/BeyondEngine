#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_MAT4_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_MAT4_H__INCLUDE
class CQuaternion;
class CVec3;
class CVec4;
class CVec2;
class CLocation;
class CMat4
{
public:
    CMat4();
    CMat4(float* pData);
    CMat4(const CLocation& location);
    float* Mat4ValuePtr() const;
    bool IsIdentity() const;
    void Identity();
    void Inverse();
    void Transpose();
    bool IsOrthogonal() const;
    float& operator[](uint32_t uIndex) const;
    CMat4 operator * (const CMat4& rhs) const;
    CVec2 operator * (const CVec2& rhs) const;
    CVec3 operator * (const CVec3& rhs) const;
    CVec4 operator * (const CVec4& rhs) const;
    bool operator == (const CMat4& rhs) const;
    bool operator != (const CMat4& rhs) const;
    void FromPitchYawRoll(float pitch, float yaw, float roll);
    void FromQuaternion(const CQuaternion* pQ);
    void SetTranslate(const CVec3& translate);
    CVec3 GetUpVec3() const;
    CVec3 GetRightVec3() const;
    CVec3 GetForwardVec3() const;
    CVec3 GetTranslate() const;
    CVec3 GetScale() const;
    void PerspectiveProjection(float fovDegree, float aspect, float zNear, float zFar);
    void OrthographicProjection(float left, float right, float bottom, float top, float nearVal, float farVal);
    void LookAt(const CVec3& eye, const CVec3& center, const CVec3& up);
    void FromAxisAngle(const CVec3& axis, float radians);
    void ToAxisAngle(CVec3 *pAxis, float *radians) const;
    CQuaternion ToQuaternion() const;
    bool Decompose(CVec3& translate, CQuaternion& rotate, CVec3& scale) const;
    void Build(const CVec3& translate, const CQuaternion& rotate, const CVec3& scale);
    void Build(const CVec3& translate, const CVec3& rotate, const CVec3& scale);
    void RemoveScale();
    void RemoveTranslate();
    void RemoveRotate();
    const void PrintValue() const;
public:
    glm::mat4 m_data;
};

#endif