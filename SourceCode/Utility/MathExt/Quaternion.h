#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_QUATERNION_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_QUATERNION_H__INCLUDE
#include "gtc/quaternion.hpp"

class CVec3;
class CMat4;
class CQuaternion
{
public:
    void FromPitchYawRoll(float fPitch, float fYaw, float fRoll);
    void ToPitchYawRoll(float& fPitch, float& fYaw, float& fRoll) const;
    void FromAxisAngle(const CVec3& axis, float angle);
    void ToAxisAngle(CVec3& axis, float& angle) const;
    void FromMatrix(const CMat4& mat);
    CQuaternion Inverse() const;
    CQuaternion Slerp(const CQuaternion& dest, float fFactor) const;
    float& X();
    const float& X() const;
    float& Y();
    const float& Y() const;
    float& Z();
    const float& Z() const;
    float& W();
    const float& W() const;
    bool operator == (const CQuaternion& rhs) const;
    bool operator != (const CQuaternion& rhs) const;
    CQuaternion operator * (const CQuaternion& rhs) const;
public:
    glm::quat m_data;
};
#endif