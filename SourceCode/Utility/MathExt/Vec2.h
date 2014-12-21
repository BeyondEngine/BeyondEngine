#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_VEC2_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_VEC2_H__INCLUDE

class CMat4;
class CVec2
{
public:
    CVec2();
    CVec2(float x, float y);
    CVec2& operator = (const CVec2& rhs);

    void Fill(float x, float y);
    float Length() const;
    void Normalize();
    float Dot(const CVec2& rhs);
    CVec2 operator + (const CVec2& rhs) const;
    CVec2 operator - (const CVec2& rhs) const;
    CVec2 operator - () const;
    CVec2 operator * (const CVec2& rhs) const;
    CVec2 operator * (float rhs) const;
    CVec2& operator *= (const CVec2& rhs);
    CVec2& operator *= (float rhs);
    CVec2 operator * (const CMat4& mat) const;
    CVec2& operator *= (const CMat4& mat);
    CVec2& operator += (const CVec2& rhs);
    bool operator == (const CVec2& rhs) const;
    bool operator != (const CVec2& rhs) const;
    float& X();
    const float& X() const;
    float& Y();
    const float& Y() const;
private:
    glm::vec2 m_data;
};

#endif
