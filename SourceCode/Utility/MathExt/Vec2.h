#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_VEC2_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_VEC2_H__INCLUDE

struct CVec2 : public kmVec2
{
public:
    explicit CVec2(float x = 0.f, float y = 0.f)
    {
        Fill(x, y);
    }
    CVec2(const kmVec2 &rhs)
    {
        Fill(rhs.x, rhs.y);
    }
    void operator = (const kmVec2 &rhs)
    {
        Fill(rhs.x, rhs.y);
    }
    void Fill(float x, float y)
    {
        kmVec2Fill(this, x, y);
    }
    void Zero()
    {
        Fill(0.f, 0.f);
    }
    float Length() const
    {
        return kmVec2Length(this);
    }
    //Returns the square of the length of the vector
    float LengthSq() const
    {
        return kmVec2LengthSq(this);
    }
    void Normalize()
    {
        kmVec2Normalize(this, this);
    }
    static CVec2 Normalize(const CVec2 &vec2)
    {
        CVec2 vec2Normalized;
        kmVec2Normalize(&vec2Normalized, &vec2);
        return vec2Normalized;
    }
    void Transform(const kmMat3 *pMat)
    {
        kmVec2Transform(this, this, pMat);
    }
    static CVec2 Transform(const CVec2 &vec2, const kmMat3 *pMat)
    {
        CVec2 vec2Transformed;
        kmVec2Transform(&vec2Transformed, &vec2, pMat);
        return vec2Transformed;
    }
    void TransformCoord(const kmMat3 *pMat)
    {
        kmVec2TransformCoord(this, this, pMat);
    }
    static CVec2 TransformCoord(const CVec2 &vec2, const kmMat3 *pMat)
    {
        CVec2 vec2Transformed;
        kmVec2TransformCoord(&vec2Transformed, &vec2, pMat);
        return vec2Transformed;
    }
    float Dot(const CVec2 &rhs)
    {
        return kmVec2Dot(this, &rhs);
    }
    friend CVec2 operator + (const CVec2 &lhs, const CVec2 &rhs)
    {
        return CVec2(lhs.x + rhs.x, lhs.y + rhs.y);
    }
    friend CVec2 operator + (const CVec2 &lhs, float rhs)
    {
        return CVec2(lhs.x + rhs, lhs.y + rhs);
    }
    friend CVec2 operator + (float lhs, const CVec2 &rhs)
    {
        return CVec2(lhs + rhs.x, lhs + rhs.y);
    }
    void operator += (const CVec2 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
    }
    friend CVec2 operator - (const CVec2 &lhs, const CVec2 &rhs)
    {
        return CVec2(lhs.x - rhs.x, lhs.y - rhs.y);
    }
    friend CVec2 operator - (const CVec2 &lhs, float rhs)
    {
        return CVec2(lhs.x - rhs, lhs.y - rhs);
    }
    friend CVec2 operator - (float lhs, const CVec2 &rhs)
    {
        return CVec2(lhs - rhs.x, lhs - rhs.y);
    }
    CVec2 operator - () const
    {
        return CVec2(-x, -y);
    }
    friend CVec2 operator * (const CVec2 &lhs, const CVec2 &rhs)
    {
        return CVec2(lhs.x * rhs.x, lhs.y * rhs.y);
    }
    friend CVec2 operator * (const CVec2 &lhs, float rhs)
    {
        return CVec2(lhs.x * rhs, lhs.y * rhs);
    }
    friend CVec2 operator * (float lhs, const CVec2 &rhs)
    {
        return CVec2(lhs * rhs.x, lhs * rhs.y);
    }
    void operator *= (float rhs)
    {
        x *= rhs;
        y *= rhs;
    }
    void operator *= (const CVec2 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
    }
    friend bool operator == (const CVec2 &lhs, const CVec2 &rhs)
    {
        return kmVec2AreEqual(&lhs, &rhs) == 1;
    }

    friend bool operator != (const CVec2 &lhs, const CVec2 &rhs)
    {
        return kmVec2AreEqual(&lhs, &rhs) != 1;
    }
};

#endif
