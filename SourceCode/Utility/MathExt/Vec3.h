#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_VEC3_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_VEC3_H__INCLUDE
#include "gtc/matrix_transform.hpp"
class CVec2;
class CMat4;

class CVec3
{
public:
    explicit CVec3(float x = 0.f, float y = 0.f, float z = 0.f);
    explicit CVec3(const CVec2& vec2, float z = 0.f);
    CVec3& operator = (const CVec3& rhs);
    void Fill(float x, float y, float z);
    void Zero(); 
    float Length() const;
    float LengthSq() const;
    void Normalize();
    bool IsNormalized() const;
    bool IsNAN() const;
    CVec3 Cross(const CVec3& rhs) const;
    float Dot(const CVec3& rhs) const;
    CVec3 Lerp(const CVec3& rhs, float fScalar) const;
    CVec3 operator + (const CVec3& rhs) const;
    CVec3 operator - (const CVec3& rhs) const;
    CVec3& operator += (const CVec3& rhs);
    CVec3& operator -= (const CVec3& rhs);

    CVec3 operator * (float rhs) const;
    CVec3& operator *= (float rhs);
    CVec3 operator * (const CVec3& quat) const;
    CVec3& operator *= (const CVec3& quat);
    CVec3 operator * (const CQuaternion& quat) const;
    CVec3& operator *= (const CQuaternion& quat);
    CVec3 operator * (const CMat4& mat) const;
    CVec3& operator *= (const CMat4& mat);
    bool operator == (const CVec3& rhs) const;
    bool operator != (const CVec3& rhs) const;
    bool operator < (const CVec3& rhs) const;
    bool operator > (const CVec3& rhs) const;
    float& X();
    const float& X() const;
    float& Y();
    const float& Y() const;
    float& Z();
    const float& Z() const;

public:
    glm::vec3 m_data;
};

#endif