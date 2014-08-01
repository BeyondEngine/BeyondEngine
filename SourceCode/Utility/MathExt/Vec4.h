#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_VEC4_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_VEC4_H__INCLUDE

struct CVec4 : public kmVec4
{
public:
    explicit CVec4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f)
    {
        kmVec4Fill(this, x, y, z, w);
    }
    explicit CVec4(const kmVec2 &vec2, float z = 0.f, float w = 0.f)
    {
        kmVec4Fill(this, vec2.x, vec2.y, z, w);
    }
    explicit CVec4(const kmVec3 &vec3, float w = 0.f)
    {
        kmVec4Fill(this, vec3.x, vec3.y, vec3.z, w);
    }
    void operator = (const kmVec4 &rhs)
    {
        kmVec4Fill(this, rhs.x, rhs.y, rhs.z, rhs.w);
    }
    CVec2 XY() const
    {
        return CVec2(x, y);
    }
    CVec3 XYZ() const
    {
        return CVec3(x, y, z);
    }
};

#endif