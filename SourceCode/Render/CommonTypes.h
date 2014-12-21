#ifndef BEYOND_ENGINE_RENDER_COMMONTYPES_H__INCLUDE
#define BEYOND_ENGINE_RENDER_COMMONTYPES_H__INCLUDE

#include "Utility/MathExt/Vec2.h"
#include "Utility/MathExt/Vec3.h"
#include "Utility/MathExt/Vec4.h"
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

    CColor(uint32_t colorUint)
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
    CColor& operator=( const CColor& other )
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
    CColor operator * (float fFactor) const
    {
        CColor ret;
        ret.r = (unsigned char)(r * fFactor);
        ret.g = (unsigned char)(g * fFactor);
        ret.b = (unsigned char)(b * fFactor);
        ret.a = a;
        return ret;
    }

    const CColor& operator + ( const CColor& other )
    {
        r = MIN(uint32_t(other.r) + r, 0xFF);
        g = MIN(uint32_t(other.g) + g, 0xFF);
        b = MIN(uint32_t(other.b) + b, 0xFF);
        a = (unsigned char)((float)other.a / 0xFF * a);

        return *this;
    }

    operator uint32_t () const
    {
        uint32_t uRet = (r << 24) + (g << 16) + (b << 8) + a;
        return uRet;
    }

    CColor Interpolate(const CColor& rhs, float fRate)
    {
        BEATS_CLIP_VALUE(fRate, 0, 1);
        CVec4 startColor(r, g, b, a);
        CVec4 endColor(rhs.r, rhs.g, rhs.b, rhs.a);
        CVec4 ret = startColor.Interpolate(endColor, fRate);
        return CColor((unsigned char)ret.X(), (unsigned char)ret.Y(), (unsigned char)ret.Z(), (unsigned char)ret.W());
    }
};

class CColorSpline
{
public:
    std::map< float , CColor> m_mapColors;
    CColorSpline()
    {
    }
    CColorSpline( const std::map< float , CColor>& colorMap )
        :m_mapColors(colorMap)
    {
    }
    CColorSpline(const std::map<float, CColor>& colorMapRef, const std::map<float, uint8_t>& alphaMapRef)
    {
        std::map<float, CColor> colorMap = colorMapRef;
        std::map<float, uint8_t> alphaMap = alphaMapRef;
        BEATS_ASSERT(colorMap.size() >= 1);
        BEATS_ASSERT(alphaMap.size() >= 1);
        if (colorMap.size() == 1)
        {
            CColor color = colorMap.begin()->second;
            colorMap.clear();
            colorMap[0] = color;
            colorMap[1] = color;
        }
        if (alphaMap.size() == 1)
        {
            uint8_t uAlpha = alphaMap.begin()->second;
            alphaMap.clear();
            alphaMap[0] = uAlpha;
            alphaMap[1] = uAlpha;
        }

        auto iterColor = colorMap.begin();
        auto iterAlpha = alphaMap.begin();
        auto lastIterColor = colorMap.end();
        auto lastIterAlpha = alphaMap.end();
        m_mapColors.clear();
        while (iterColor != colorMap.end() || iterAlpha != alphaMap.end())
        {
            if (iterColor == colorMap.end())
            {
                CColor& color = m_mapColors[iterAlpha->first];
                color = lastIterColor->second;
                color.a = iterAlpha->second;
                lastIterAlpha = iterAlpha;
                ++iterAlpha;
            }
            else if (iterAlpha == alphaMap.end())
            {
                CColor& color = m_mapColors[iterColor->first];
                color = iterColor->second;
                color.a = lastIterAlpha->second;
                lastIterColor = iterColor;
                ++iterColor;
            }
            else
            {
                if (BEATS_FLOAT_EQUAL(iterAlpha->first, iterColor->first))
                {
                    CColor& color = m_mapColors[iterAlpha->first];
                    color = iterColor->second;
                    color.a = iterAlpha->second;
                    lastIterAlpha = iterAlpha;
                    lastIterColor = iterColor;
                    ++iterAlpha;
                    ++iterColor;
                }
                else if (BEATS_FLOAT_LESS(iterAlpha->first, iterColor->first))
                {
                    CColor& color = m_mapColors[iterAlpha->first];
                    if (lastIterColor == colorMap.end())
                    {
                        color = iterColor->second;
                    }
                    else
                    {
                        float fRate = (iterAlpha->first - lastIterColor->first) / (iterColor->first - lastIterColor->first);
                        BEATS_ASSERT(BEATS_FLOAT_GREATER(fRate, 0) && BEATS_FLOAT_LESS(fRate, 1));
                        color = lastIterColor->second.Interpolate(iterColor->second, fRate);
                    }
                    color.a = iterAlpha->second;
                    lastIterAlpha = iterAlpha;
                    ++iterAlpha;
                }
                else
                {
                    BEATS_ASSERT(BEATS_FLOAT_LESS(iterColor->first, iterAlpha->first));
                    CColor& color = m_mapColors[iterColor->first];
                    color = iterColor->second;
                    if (lastIterAlpha == alphaMap.end())
                    {
                        color.a = iterAlpha->second;
                    }
                    else
                    {
                        float fRate = (iterColor->first - lastIterAlpha->first) / (iterAlpha->first - lastIterAlpha->first);
                        BEATS_ASSERT(BEATS_FLOAT_GREATER(fRate, 0) && BEATS_FLOAT_LESS(fRate, 1));
                        int startAlpha = lastIterAlpha->second;
                        int endAlpha = iterAlpha->second;
                        color.a = (uint8_t)((endAlpha - startAlpha) * fRate + startAlpha);
                    }
                    lastIterColor = iterColor;
                    ++iterColor;
                }
            }
        }
    }
    CColor GetValue(float fProgress) const
    {
        CColor ret = 0xFFFFFFFF;
        if (m_mapColors.size() > 0)
        {
            BEATS_CLIP_VALUE(fProgress, 0, 1);
            if (fProgress <= m_mapColors.begin()->first)
            {
                ret = m_mapColors.begin()->second;
            }
            else if (fProgress >= m_mapColors.rbegin()->first)
            {
                ret = m_mapColors.rbegin()->second;
            }
            else
            {
                auto iter = m_mapColors.begin();
                auto preIter = iter;
                ++iter;
                for (; iter != m_mapColors.end(); ++iter)
                {
                    if (fProgress < iter->first)
                    {
                        CColor startColor = preIter->second;
                        CColor endColor = iter->second;
                        float fInterplotion = ((fProgress - preIter->first) / (iter->first - preIter->first));
                        ret = startColor.Interpolate(endColor, fInterplotion);
                        break;
                    }
                    else
                    {
                        preIter = iter;
                    }
                }
            }
        }
        return ret;
    }
};

class CRect
{
public:
    CVec2 position;
    CVec2 size;

    CRect()
    {
    }

    CRect(const CVec2 &position, const CVec2 &size)
        : position(position)
        , size(size)
    {}

    bool operator==( const CRect& other ) const
    {
        return position.X() == other.position.X() && position.Y() == other.position.Y() &&
            size.X() == other.size.X() && size.Y() == other.size.Y();
    }

    CRect& operator=(const CRect& other)
    {
        position = other.position;
        size = other.size;
        return *this;
    }

    bool isZero()
    {
        CRect c;
        return (*this == c);
    }

    friend CRect Intersects(const CRect &a, const CRect &b)
    {
        CVec2 bottomRightA = a.position + a.size;
        CVec2 bottomRightB = b.position + b.size;
        return CRect(
            CVec2(MAX(a.position.X(), b.position.X()), MAX(a.position.Y(), b.position.Y())),
            CVec2(MIN(bottomRightA.X(), bottomRightB.X()), MIN(bottomRightA.Y(), bottomRightB.Y())) );
    }

    friend CRect MerageRect(const CRect &a, const CRect &b)
    {
        CVec2 beginPoint(MIN(a.position.X(), b.position.X()), MIN(a.position.Y(), b.position.Y()));
        CVec2 aEndPoint = a.position + a.size;
        CVec2 bEndPoint = b.position + b.size;
        CVec2 endPoint(MAX(aEndPoint.X(), bEndPoint.X()), MAX(aEndPoint.Y(), bEndPoint.Y()));
        return CRect(beginPoint, endPoint - beginPoint);
    }

    bool containsPoint(const CVec2& point) const
    {
        bool bRet = false;
        CVec2 maxPoint = position + size;
        if (point.X() >= position.X() && point.X() <= maxPoint.X()
            && point.Y() >= position.Y() && point.Y() <= maxPoint.Y())
        {
            bRet = true;
        }

        return bRet;
    }


    bool intersectsRect(const CRect& a, const CRect& b) const
    {
        float aMaXX = a.position.X() + a.size.X();
        float aMaXY = a.position.Y() + a.size.Y();
        float aMinX = a.position.X();
        float aMinY = a.position.Y();

        float bMaXX = b.position.X() + b.size.X();
        float bMaXY = b.position.Y() + b.size.Y();
        float bMinX = b.position.X();
        float bMinY = b.position.Y();

        return !(aMaXX < bMinX || bMaXX < aMinX || aMaXY < bMinY || bMaXY < aMinY);
    }
};


class CVertexPT
{
public:
    CVec3 position;
    CTex tex;
};

class CVertexPTT
{
public:
    CVec3 position;
    CTex tex;
    CTex lightmapTex;
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
    CVertexPTB()
    {
        bones.Fill(-1.0f, -1.0f, -1.0f, -1.0f);
    }
    CVec3 position;
    CTex tex;
    CVec4 bones; // use CVec4 because it can effect 4 bones at most.
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