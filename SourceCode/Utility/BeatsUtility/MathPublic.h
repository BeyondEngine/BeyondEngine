#ifndef BEATS_UTILITY_MATH_PUBLIC_H__INCLUDE
#define BEATS_UTILITY_MATH_PUBLIC_H__INCLUDE

#include <cmath>
#include "fwd.hpp"
#include "glm.hpp"
#include "MathExt/Mat3.h"
#include "MathExt/Mat4.h"
#include "MathExt/Vec2.h"
#include "MathExt/Vec3.h"
#include "MathExt/Vec4.h"
#include "MathExt/Quaternion.h"
#include "MathExt/Ray3.h"
#include "MathExt/Plane.h"

static const float MATH_PI = 3.1415926535897932384626433832795F;
static const float MATH_PI_HALF = 1.5707963267948966192313216916398F;
static const float MATH_PI_DOUBLE = 6.2831853071795864769252867665590F;
static const float MATH_PI_DEGREE = 180.0F;

#define RANGR_RANDOM_FLOAT(min, max) \
    (((( max ) - ( min )) * ((float)rand( ) / (float) RAND_MAX )) + ( min ))

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

inline bool IsPowerOf2(uint32_t uNumber)
{
    return (uNumber & (uNumber - 1)) == 0;
}

inline float DegreesToRadians(float degrees)
{
    return glm::radians(degrees);
}

inline float RadiansToDegrees(float radians)
{
    return glm::degrees(radians);
}

// Sometimes fmodf is nan, I don't know why!
// Call this function for safe!
inline float BEATS_FMOD(float fValue, float fMod)
{
    BEATS_ASSERT(!BEATS_FLOAT_EQUAL(fMod, 0));
#ifdef USE_FMOD 
    float fRet = fmodf(fValue, fMod);
    BEATS_ASSERT(!isinf(fRet) && !isnan(fRet));
    return fRet;
#else
    //port from golang, see https://github.com/golang/go/blob/master/src/math/mod.go
    float x = fValue;
    float y = fMod;
    if (y == 0 || std::isinf(x) || std::isnan(x) || std::isnan(y))
    {
        BEATS_ASSERT(false, "BEATS_FMOD encounter NaN!");
        return std::numeric_limits<float>::signaling_NaN();
    }
    if (y < 0)
    {
        y = -y;
    }
    int yexp = 0;
    float yfr = std::frexp(y, &yexp);
    bool sign = false;
    float r = x;
    if (x < 0)
    {
        r = -x;
        sign = true;
    }
    for (; r >= y;)
    {
        int rexp = 0;
        float rfr = std::frexp(r, &rexp);
        if (rfr < yfr)
        {
            rexp = rexp - 1;
        }
        r = r - std::ldexp(y, rexp - yexp);
    }
    if (sign)
    {
        r = -r;
    }
    return r;
#endif
}

inline uint32_t IntersectLineSegmentSphere(const CVec3& startPos, const CVec3& endPos, const CVec3& sphereCenter, float fSphereRadius, CVec3& intersectPos1, CVec3& intersectPos2)
{
    uint32_t uIntersectPointCount = 0;
    //Read from http://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm
    CVec3 d = endPos - startPos;
    CVec3 f = startPos - sphereCenter;

    float a = d.Dot(d);
    float b = 2 * f.Dot(d);
    float c = f.Dot(f) - fSphereRadius * fSphereRadius;
    float discriminant = b*b - 4 * a * c;
    if (discriminant >= 0)
    {
        discriminant = sqrt(discriminant);
        float t1 = (-b - discriminant) / (2 * a);
        float t2 = (-b + discriminant) / (2 * a);
        // 3x HIT cases:
        //          -o->             --|-->  |            |  --|->
        // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit), 

        // 3x MISS cases:
        //       ->  o                     o ->              | -> |
        // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)
        if (t1 >= 0 && t1 <= 1)
        {
            // t1 is the intersection, and it's closer than t2
            // (since t1 uses -b - discriminant)
            intersectPos1 = startPos + d * t1;
            uIntersectPointCount = 1;
        }
        if (t2 >= 0 && t2 <= 1)
        {
            if (uIntersectPointCount == 1)
            {
                intersectPos2 = startPos + d * t2;
                uIntersectPointCount = 2;
            }
            else
            {
                BEATS_ASSERT(uIntersectPointCount == 0);
                // here t1 didn't intersect so we are either started
                // inside the sphere or completely past it
                // ExitWound
                intersectPos1 = startPos + d * t2;
                uIntersectPointCount = 1;
            }
        }
        // no intersect: FallShort, Past, CompletelyInside
    }
    return uIntersectPointCount;
}

inline bool IntersectPointRectangle(const CVec3& pos, const CVec3& leftUpPos, const CVec3& leftDownPos, const CVec3& rightDownPos)
{
    bool bRet = false;
    CVec3 leftEdge = leftDownPos - leftUpPos;
    CVec3 leftTmp = pos - leftUpPos;
    float a = leftEdge.Dot(leftEdge);
    float b = leftEdge.Dot(leftTmp);
    if (a >= b && b >= 0)
    {
        CVec3 downEdge = rightDownPos - leftDownPos;
        CVec3 downTmp = pos - leftDownPos;
        a = downEdge.Dot(downEdge);
        b = downEdge.Dot(downTmp);
        bRet = a >= b && b >= 0;
    }
    return bRet;
}

inline bool IntersectRectSphere(const CVec3& leftUpPos, const CVec3& leftDownPos, const CVec3& rightDownPos, const CVec3& rightUpPos, const CVec3& sphereCenter, float fSphereRadius)
{
    bool bRet = true;
    CVec3 centerPosOfRect = (rightDownPos - leftUpPos) * 0.5f + leftUpPos;
    float fDistanceSq = (centerPosOfRect - sphereCenter).LengthSq();
    if (BEATS_FLOAT_GREATER(fDistanceSq, fSphereRadius * fSphereRadius))//if the rect center is in the sphere
    {
        if (!IntersectPointRectangle(sphereCenter, leftUpPos, leftDownPos, rightDownPos)) // if the sphere center is in the rectangle
        {
            CVec3 intersectionPosition1;
            CVec3 intersectionNormal1;
            bRet = IntersectLineSegmentSphere(leftUpPos, leftDownPos, sphereCenter, fSphereRadius, intersectionPosition1, intersectionNormal1) > 0 ||
                IntersectLineSegmentSphere(leftDownPos, rightDownPos, sphereCenter, fSphereRadius, intersectionPosition1, intersectionNormal1) > 0 ||
                IntersectLineSegmentSphere(rightDownPos, rightUpPos, sphereCenter, fSphereRadius, intersectionPosition1, intersectionNormal1) > 0 ||
                IntersectLineSegmentSphere(rightUpPos, leftUpPos, sphereCenter, fSphereRadius, intersectionPosition1, intersectionNormal1) > 0;
        }
    }
    return bRet;
}

#endif