#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_VEC4_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_VEC4_H__INCLUDE
class CVec2;
class CVec3;
class CMat4;

class CVec4
{
public:
    CVec4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f);
    CVec4(const CVec2& vec2, float z = 0.f, float w = 0.f);
    CVec4(const CVec3& vec3, float w = 0.f);
    CVec4& operator = (const CVec4& rhs);
    CVec4 operator * (const CMat4& mat) const;
    CVec4& operator *= (const CMat4& mat);
    CVec4 operator * (float rhs) const;
    CVec4& operator *= (float rhs);
    bool operator == (const CVec4& rhs) const;
    bool operator != (const CVec4& rhs) const;
    CVec4 operator + (const CVec4& rhs) const;
    CVec4 operator - (const CVec4& rhs) const;
    CVec4& operator += (const CVec4& rhs);
    CVec4& operator -= (const CVec4& rhs);

    void Fill(float x, float y, float z, float w);
    float& X();
    const float& X() const;
    float& Y();
    const float& Y() const;
    float& Z();
    const float& Z() const;
    float& W();
    const float& W() const;
    CVec4 Interpolate(const CVec4& rhs, float fRate);
public:
    glm::vec4 m_data;
};

#endif