#include "stdafx.h"
#include "ScaleParticleAnimation.h"

namespace NBeyondEngine
{
    CScaleParticleAnimation::CScaleParticleAnimation()
    {
        kmVec2Fill( &m_beginScale, 1.0f, 1.0f);
        kmVec2Fill( &m_endScale, 1.0f, 1.0f);
        kmVec2Fill( &m_currentScale, 1.0f, 1.0f);
        m_animationType = eAT_SCALE;
    }

    CScaleParticleAnimation::~CScaleParticleAnimation()
    {

    }

    void CScaleParticleAnimation::ReflectData(CSerializer& serializer)
    {
        super::ReflectData(serializer);
        DECLARE_PROPERTY( serializer, m_beginScale, true, 0xFFFFFFFF, _T("开始放缩倍数"), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_endScale, true, 0xFFFFFFFF, _T("结束放缩倍数"), NULL, NULL, NULL );
    }

    const void* CScaleParticleAnimation::GetCurrentAnimationValue( float currentTime, float totalTime )
    {
        if ( totalTime > 0 )
        {
            float cursor = currentTime / totalTime;
            float x = ( m_endScale.x - m_beginScale.x ) * cursor + m_beginScale.x;
            float y = ( m_endScale.y - m_beginScale.y ) * cursor + m_beginScale.y;
            kmVec2Fill( &m_currentScale, x, y );
        }
        return &m_currentScale;
    }

    void CScaleParticleAnimation::SetInitAnimationValue( const void* pValue, unsigned int count )
    {
        BEATS_ASSERT( count == 2, _T( "pValue must not be two float type" ) );
        kmVec2* pRelValue = (kmVec2*)pValue;
        kmVec2Fill( &m_beginScale, pRelValue[ 0 ].x, pRelValue[ 0 ].y);
        kmVec2Fill( &m_endScale, pRelValue[ 1 ].x, pRelValue[ 1 ].y);
    }
};