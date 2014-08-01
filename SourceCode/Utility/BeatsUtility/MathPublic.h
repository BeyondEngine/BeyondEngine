#ifndef BEATS_UTILITY_MATH_PUBLIC_H__INCLUDE
#define BEATS_UTILITY_MATH_PUBLIC_H__INCLUDE

enum EVectorPos
{
    eVP_X,
    eVP_Y,
    eVP_Z,
    eVP_W,

    eVP_Count,
    eVP_ForceTo32Bit = 0xffffffff
};

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

#endif