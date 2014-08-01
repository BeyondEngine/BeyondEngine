#ifndef BEYOND_ENGINE_RENDER_COMMONTYPES_H__INCLUDE
#define BEYOND_ENGINE_RENDER_COMMONTYPES_H__INCLUDE

#include "Utility/MathExt/Vec2.h"
#include "Utility/MathExt/Vec3.h"
#include "Utility/MathExt/Vec4.h"
#include "Utility/MathExt/IVec4.h"
#include "Utility/MathExt/Mat4.h"

class CTex
{
public:
    CTex()
    : u(0)
    , v(0)
    {
    }

    CTex(float u, float v)
        : u(u)
        , v(v)
    {
    }

    float u, v;
};

class CColor
{
public:
    unsigned char r, g, b, a;
    CColor()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }
    CColor( const CColor& other )
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }

    CColor(float fr, float fg, float fb, float fa)
    {
        BEATS_ASSERT(fr <= 1.0f && fr >= 0.0f);
        BEATS_ASSERT(fg <= 1.0f && fg >= 0.0f);
        BEATS_ASSERT(fb <= 1.0f && fb >= 0.0f);
        BEATS_ASSERT(fa <= 1.0f && fa >= 0.0f);
        r = (char)(fr * 0xFF);
        g = (char)(fg * 0xFF);
        b = (char)(fb * 0xFF);
        a = (char)(fa * 0xFF);
    }

    CColor(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa)
        : r(rr)
        , g(gg)
        , b(bb)
        , a(aa)
    {
    }

    CColor(size_t colorUint)
    {
        r = colorUint >> 24;
        g = (colorUint >> 16) & 0x000000FF;
        b = (colorUint >> 8) & 0x000000FF;
        a = colorUint & 0x000000FF;
    }

    bool operator==( const CColor& other ) const
    {
        return ( r == other.r && g == other.g && b == other.b && a == other.a );
    }

    bool operator!=( const CColor& other ) const
    {
        return ( r != other.r || g != other.g || b != other.b || a != other.a );
    }
    const CColor& operator=( const CColor& other )
    {
        if ( this != &other )
        {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
        }
        return *this;
    }

    const CColor& operator + ( const CColor& other )
    {
        r = MIN(size_t(other.r) + r, 0xFF);
        g = MIN(size_t(other.g) + g, 0xFF);
        b = MIN(size_t(other.b) + b, 0xFF);
        a = (unsigned char)((float)other.a / 0xFF * a);

        return *this;
    }


    operator size_t () const
    {
        size_t uRet = (r << 24) + (g << 16) + (b << 8) + a;
        return uRet;
    }
};

class CColorSpline
{
public:
    std::map< float , CColor> m_mapColors;
    CColorSpline()
    {
    }
    CColorSpline( std::map< float , CColor>& colorMap )
    {
        std::map< float , CColor>::iterator iter = colorMap.begin();
        for ( ; iter != colorMap.end(); ++iter )
        {
            m_mapColors.insert( std::make_pair( iter->first, iter->second ));
        }
    }
};

class CRect
{
public:
    CVec2 position;
    CVec2 size;

    CRect()
    {}

    CRect(const CVec2 &position, const CVec2 &size)
        : position(position)
        , size(size)
    {}
    bool operator==( const CRect& other ) const
    {
        return position.x == other.position.x && position.y == other.position.y &&
            size.x == other.size.x && size.y == other.size.y;
    }

    friend CRect Intersects(const CRect &a, const CRect &b)
    {
        CVec2 bottomRightA = a.position + a.size;
        CVec2 bottomRightB = b.position + b.size;
        return CRect(
            CVec2(MAX(a.position.x, b.position.x), MAX(a.position.y, b.position.y)),
            CVec2(MIN(bottomRightA.x, bottomRightB.x), MIN(bottomRightA.y, bottomRightB.y)) );
    }
};

class CVertexPT
{
public:
    CVec3 position;
    CTex tex;
};

class CVertexPTC
{
public:
    CVec3 position;
    CColor color;
    CTex tex;
};

class CVertexPTCC
{
public:
    CVec3 position;
    CColor color;
    CColor color2;
    CTex tex;
};

class CVertexPTB
{
public:
    CVec3 position;
    CTex tex;
    CIVec4 bones;
    CVec4 weights;
};

class CVertexPC
{
public:
    CVec3 position;
    CColor color;
};

class CVertex2DPC
{
public:
    CVec2 position;
    CColor color;
};

class CVertexPTN
{
public:
    CVec3 position;
    CVec3 normal;
    CTex tex;
};

class CVertexPTNC
{
public:
    CVec3 position;
    CVec3 normal;
    CTex tex;
    CColor color;
};

class CQuadPT
{
public:
    CVertexPT tl,    //top left
              bl,    //bottom left
              tr,    //top right
              br;    //bottom right
};

class CQuadPC
{
public:
    CVertexPC tl,
              bl,
              tr,
              br;
};

class CQuadPTCC
{
public:
    CVertexPTCC tl,
        bl,
        tr,
        br;
};

class CQuadPTC
{
public:
    CVertexPTC tl,
               bl,
               tr,
               br;
};

class CQuadP
{
public:
    CVec3  tl,    //topleft
           bl,    //bottomleft
           tr,    //topright
           br;    //bottomright
};

class CQuadT
{
public:
    CTex    tl,    //topleft
            bl,    //bottomleft
            tr,    //topright
            br;    //bottomright
};

class CQuadC
{
public:
    CColor  tl,
            bl,
            tr,
            br;
};

#endif