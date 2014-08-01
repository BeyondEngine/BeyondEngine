#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_VEC3_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_VEC3_H__INCLUDE

struct CVec3 : public kmVec3
{
public:
    explicit CVec3(float x = 0.f, float y = 0.f, float z = 0.f)
    {
        kmVec3Fill(this, x, y, z);
    }
    explicit CVec3(const kmVec2 &vec2, float z = 0.f)
    {
        kmVec3Fill(this, vec2.x, vec2.y, z);
    }
    CVec3(const kmVec3 &rhs)
    {
        kmVec3Fill(this, rhs.x, rhs.y, rhs.z);
    }
    void operator = (const kmVec3 &rhs)
    {
        kmVec3Fill(this, rhs.x, rhs.y, rhs.z);
    }
    CVec2 XY() const
    {
        return CVec2(x, y);
    }
    void Fill(float x, float y, float z)
    {
        kmVec3Fill(this, x, y, z);
    }
    void Zero()
    {
        Fill(0.f, 0.f, 0.f);
    }
    float Length() const
    {
        return kmVec3Length(this);
    }
    //Returns the square of the length of the vector
    float LengthSq() const
    {
        return kmVec3LengthSq(this);
    }
    void Normalize()
    {
        kmVec3Normalize(this, this);
    }
    static CVec3 Normalize(const CVec3 &vec3)
    {
        CVec3 vec3Normalized;
        kmVec3Normalize(&vec3Normalized, &vec3);
        return vec3Normalized;
    }
    void Transform(const kmMat4 &mat)
    {
        kmVec3Transform(this, this, &mat);
    }
    static CVec3 Transform(const CVec3 &vec3, const kmMat4 &mat)
    {
        CVec3 vec3Transformed;
        kmVec3Transform(&vec3Transformed, &vec3, &mat);
        return vec3Transformed;
    }
    void TransformCoord(const kmMat4 &mat)
    {
        kmVec3TransformCoord(this, this, &mat);
    }
    static CVec3 TransformCoord(const CVec3 &vec3, const kmMat4 &mat)
    {
        CVec3 vec3Transformed;
        kmVec3TransformCoord(&vec3Transformed, &vec3, &mat);
        return vec3Transformed;
    }
    void TransformNormal(const kmMat4 &mat)
    {
        kmVec3TransformNormal(this, this, &mat);
    }
    static CVec3 TransformNormal(const CVec3 &vec3, const kmMat4 &mat)
    {
        CVec3 vec3Transformed;
        kmVec3TransformNormal(&vec3Transformed, &vec3, &mat);
        return vec3Transformed;
    }
    CVec3 Cross(const CVec3 &rhs) const
    {
        CVec3 vec3Crossed;
        kmVec3Cross(&vec3Crossed, this, &rhs);
        return vec3Crossed;
    }
    float Dot(const CVec3 &rhs) const
    {
        return kmVec3Dot(this, &rhs);
    }
    friend CVec3 operator + (const CVec3 &lhs, const CVec3 &rhs)
    {
        CVec3 vec3Added;
        kmVec3Add(&vec3Added, &lhs, &rhs);
        return vec3Added;
    }
    friend CVec3 operator - (const CVec3 &lhs, const CVec3 &rhs)
    {
        CVec3 vec3Subtracted;
        kmVec3Subtract(&vec3Subtracted, &lhs, &rhs);
        return vec3Subtracted;
    }
    friend CVec3 operator * (const CVec3 &lhs, const CVec3 &rhs)
    {
        return CVec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }
    friend CVec3 operator * (const CVec3 &lhs, float rhs)
    {
        CVec3 vec3Scaled;
        kmVec3Scale(&vec3Scaled, &lhs, rhs);
        return vec3Scaled;
    }
    friend bool operator == (const CVec3 &lhs, const CVec3 &rhs)
    {
        return kmVec3AreEqual(&lhs, &rhs) == 1;
    }
};

#endif