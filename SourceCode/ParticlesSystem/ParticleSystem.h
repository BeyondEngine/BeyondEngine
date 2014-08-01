#ifndef BEYOND_ENGINE_PARTICLESYSTEM_PARTICLESYSYTEM_H_INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_PARTICLESYSYTEM_H_INCLUDE

#include "Scene/Node3D.h"

namespace NBeyondEngine
{
    class CParticleEmitter;

    class CParticleSystem : public CNode3D
    {
        typedef std::vector< CParticleEmitter* > TEmitterVector;
        DECLARE_REFLECT_GUID( CParticleSystem, 0x154AC159, CNode3D )
    public:
        CParticleSystem();

        virtual ~CParticleSystem();

        virtual void ReflectData(CSerializer& serializer) override;

        virtual bool OnDependencyListChange( void* pComponentAddr, enum EDependencyChangeAction action, CComponentBase* pComponent)override;

        virtual void Update( float dtt ) override;

        virtual void Initialize() override;

        virtual void DoRender() override;

        bool AddEmitter( CParticleEmitter* pEmitter );
        void RemoveEmitter( CParticleEmitter* pEmitter );

        void BeginShoot();
    private:
        TEmitterVector m_emitterVector;
        bool m_bLoop;
        float m_fLiveTime;
        float m_fCurrentTime;
        
    };

};

#endif // ParticleSystem_H_INCLUDE
