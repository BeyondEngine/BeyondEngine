#ifndef BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_SCALEPARTICLEANIMATION_H_INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_SCALEPARTICLEANIMATION_H_INCLUDE

#include "ParticleAnimationBase.h"

namespace NBeyondEngine
{
    class CScaleParticleAnimation : public CParticleAnimationBase
    {
        DECLARE_REFLECT_GUID( CScaleParticleAnimation, 0x1544B156, CParticleAnimationBase )
    public:
        CScaleParticleAnimation();
        virtual ~CScaleParticleAnimation();
        virtual void ReflectData(CSerializer& serializer) override;

        virtual const void* GetCurrentAnimationValue( float currentTime, float totalTime );
        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 );

    private:
        CVec2 m_currentScale;
        CVec2 m_beginScale;
        CVec2 m_endScale;
    };
};

#endif // PARTICLE_SCALEPARTICLEANIMATION_H_INCLUDE