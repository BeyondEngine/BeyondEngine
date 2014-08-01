#ifndef BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_ROTATEPARTICLEANIMATION_H_INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_ROTATEPARTICLEANIMATION_H_INCLUDE

#include "ParticleAnimationBase.h"

namespace NBeyondEngine
{
    class CRotateParticleAnimation : public CParticleAnimationBase
    {
        DECLARE_REFLECT_GUID( CRotateParticleAnimation, 0x1544B157, CParticleAnimationBase )
    public:
        CRotateParticleAnimation();
        virtual ~CRotateParticleAnimation();

        virtual void ReflectData(CSerializer& serializer) override;

        virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;

        virtual const void* GetCurrentAnimationValue( float currentTime, float totalTime );
        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 );
        CRotateParticleAnimation& operator=( const CRotateParticleAnimation& other );
    private:
        float m_fCurrentRotate;
        float m_fBaseRotate;
    };
};

#endif // PARTICLE_ROTATEPARTICLEANIMATION_H_INCLUDE
