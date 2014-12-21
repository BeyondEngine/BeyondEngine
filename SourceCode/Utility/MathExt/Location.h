#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_LOCATION_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_LOCATION_H__INCLUDE
#include "Vec3.h"
#include "Quaternion.h"
class CLocation
{
public:
    CLocation();
    ~CLocation();
    CLocation& operator = (const CLocation& rhs);
    CLocation Lerp(const CLocation& rhs, float fFactor) const;

public:
    CVec3 m_pos;
    CVec3 m_scale;
    CQuaternion m_rotation;
};
#endif