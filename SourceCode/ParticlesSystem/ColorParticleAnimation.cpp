#include "stdafx.h"
#include "ColorParticleAnimation.h"

namespace NBeyondEngine
{
    CColorParticleAnimation::CColorParticleAnimation()
        : fBeginTime(0.0f)
        , fEndTime(0.0f)
        , m_currentColor(CColor( 0.0f, 0.0f, 0.0f, 0.0f ))
    {
        m_animationType = eAT_COLOR;
    }

    CColorParticleAnimation::~CColorParticleAnimation()
    {

    }

    void CColorParticleAnimation::ReflectData(CSerializer& serializer)
    {
        super::ReflectData(serializer);
        DECLARE_PROPERTY( serializer, m_colorSpline, true, 0xFFFFFFFF, _T("颜色样条"), NULL, NULL, NULL );
    }

    const void* CColorParticleAnimation::GetCurrentAnimationValue( float currentTime, float totalTime )
    {
        if ( totalTime > 0 )
        {
            float delta = currentTime / totalTime;
            if ( delta <= 1.0f )
            {
                float fCurrentIndex = 0.0f;
                float nextIndex = 1.0f;
                auto iter = m_colorSpline.m_mapColors.begin();
                for ( ; iter != m_colorSpline.m_mapColors.end(); ++iter )
                {
                    if ( iter->first > delta )
                    {
                        nextIndex = iter->first;
                        fCurrentIndex = (--iter)->first;
                        break;
                    }
                }
                fBeginTime = fCurrentIndex;
                fEndTime = nextIndex;

                float r = CalcCurrentValue( m_colorSpline.m_mapColors[fBeginTime].r, m_colorSpline.m_mapColors[fEndTime].r, delta - fBeginTime / totalTime);
                float g = CalcCurrentValue( m_colorSpline.m_mapColors[fBeginTime].g,  m_colorSpline.m_mapColors[fEndTime].g, delta - fBeginTime / totalTime);
                float b = CalcCurrentValue( m_colorSpline.m_mapColors[fBeginTime].b,  m_colorSpline.m_mapColors[fEndTime].b, delta - fBeginTime / totalTime);
                float a = CalcCurrentValue( m_colorSpline.m_mapColors[fBeginTime].a,  m_colorSpline.m_mapColors[fEndTime].a, delta - fBeginTime / totalTime);
                //Temporarily divided by 255， change it after editor complate
                m_currentColor = CColor( r / 255.0f , g / 255.0f , b / 255.0f , a / 255.0f );
            }
        }
        return &m_currentColor;
    }

    void CColorParticleAnimation::SetInitAnimationValue( const void* pValue, unsigned int count )
    {
        BEATS_ASSERT( count > 0, _T( "pValue must not be lager zero CColor type" ) );
        SColorSpline* colorSpline = (SColorSpline*)pValue;
        for ( unsigned int i = 0; i < count; ++i )
        {
            m_colorSpline.m_mapColors.insert( std::make_pair( colorSpline[i].time,colorSpline[i].m_pColor ));
        }
    }

    float CColorParticleAnimation::CalcCurrentValue( float begin, float end, float delta )
    {
        if ( delta < 0.0 )
        {
            delta = 0.0;
        }
        else if( delta > 1.0 )
        {
            delta = 1.0;
        }
        return ( end - begin ) * delta + begin;
    }
};
