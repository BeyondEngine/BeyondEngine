#include "stdafx.h"
#include "RotateParticleAnimation.h"

namespace NBeyondEngine
{
    CRotateParticleAnimation::CRotateParticleAnimation()
        : m_fCurrentRotate(0.0f)
        , m_fBaseRotate(0.0f)
    {
        m_animationType = eAT_ROTATE;
    }

    CRotateParticleAnimation::~CRotateParticleAnimation()
    {

    }

    void CRotateParticleAnimation::ReflectData(CSerializer& serializer)
    {
        super::ReflectData(serializer);
        DECLARE_PROPERTY( serializer, m_fBaseRotate, true, 0xFFFFFFFF, _T("旋转速度"), NULL, NULL, NULL );
        m_fBaseRotate = kmDegreesToRadians( m_fBaseRotate );
    }

    const void* CRotateParticleAnimation::GetCurrentAnimationValue( float currentTime, float totalTime )
    {
        if ( totalTime > 0 )
        {
            m_fCurrentRotate = currentTime * m_fBaseRotate;
            if ( m_fCurrentRotate > MATH_PI_DOUBLE )
            {
                m_fCurrentRotate = m_fCurrentRotate - MATH_PI_DOUBLE;
            }
        }
        return &m_fCurrentRotate;
    }

    void CRotateParticleAnimation::SetInitAnimationValue( const void* pValue, unsigned int count )
    {
        BEATS_ASSERT( count == 1, _T( "pValue must not be one float type" ) );
        m_fBaseRotate = *(float*)pValue;
        //convert degree to radian
        m_fBaseRotate = kmDegreesToRadians( m_fBaseRotate );
    }

    bool CRotateParticleAnimation::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
    {
        bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
        if (!bRet)
        {
            if (pVariableAddr == &m_fBaseRotate)
            {
                DeserializeVariable(m_fBaseRotate, pSerializer);
                m_fBaseRotate = kmDegreesToRadians( m_fBaseRotate );
                bRet = true;
            }
        }
        return bRet;
    }
};