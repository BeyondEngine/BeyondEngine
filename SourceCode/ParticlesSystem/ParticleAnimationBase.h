#ifndef BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_PARTICLEANIMATIONBASE_H_INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_PARTICLEANIMATION_PARTICLEANIMATIONBASE_H_INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

namespace NBeyondEngine
{
    enum EAnimationType
    {
        eAT_COLOR = 0,
        eAT_SCALE,
        eAT_ROTATE
    };

    class CParticleAnimationBase : public CComponentInstance
    {
        DECLARE_REFLECT_GUID_ABSTRACT( CParticleAnimationBase, 0x1544B159, CComponentInstance )
    public:
        virtual ~CParticleAnimationBase();

        CParticleAnimationBase();

        // this method return void* ,the type decision by the ParticleAnimation,
        // when use this method must be care the type
        virtual const void* GetCurrentAnimationValue( float currentTime, float totalTime ) = 0;
        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 ) = 0;

        const EAnimationType& GetAnimationType( );

    protected:
        EAnimationType m_animationType;
    };
};

#endif // ParticleAnimationBase_H_INCLUDE