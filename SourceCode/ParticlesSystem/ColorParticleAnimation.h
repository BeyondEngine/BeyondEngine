#ifndef BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_COLORPARTICLEANIMATION_H__INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_COLORPARTICLEANIMATION_H__INCLUDE

#include "ParticleAnimationBase.h"

namespace NBeyondEngine
{
    struct SColorSpline
    {
        CColor m_pColor;
        float time;
    };
    class CColorParticleAnimation : public CParticleAnimationBase
    {
        DECLARE_REFLECT_GUID( CColorParticleAnimation, 0x1544B158, CParticleAnimationBase )
    public:
        CColorParticleAnimation();
        virtual ~CColorParticleAnimation();

        virtual void ReflectData(CSerializer& serializer) override;

        virtual const void* GetCurrentAnimationValue( float currentTime, float totalTime );

        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 );

    private:
        float CalcCurrentValue( float begin, float end, float delta );
    private:
         float fBeginTime;
         float fEndTime;
         CColor m_currentColor;
         CColorSpline m_colorSpline;
    };
};

#endif // PARTICLE_COLORPARTICLEANIMATION_H_INCLUDE
