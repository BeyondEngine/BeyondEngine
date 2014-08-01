#ifndef BEYOND_ENGINE_PARTICLESYSTEM_PARTICLE_H_INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_PARTICLE_H_INCLUDE

#include "ColorParticleAnimation.h"
#include "RotateParticleAnimation.h"
#include "ScaleParticleAnimation.h"
#include "Billboard.h"

namespace NBeyondEngine
{
    struct SParticle
    {
        float m_fSpeed;
        float m_fDecay;
        float m_fForcePower;
        float m_fLiveTime;
        float m_fBirthRadin;
        kmVec3 m_vec3ForceDirection;
        kmVec3 m_vec3OriginPosition;
        kmVec3 m_vec3OriginDirection;

        SParticle( )
        {
            kmVec3Fill( &m_vec3OriginPosition, 0.0, 0.0, 0.0 );
            kmVec3Fill( &m_vec3OriginDirection, 0.0, 1.0, 0.0 );
            m_fSpeed = 1;
            m_fDecay = 0;
            m_fForcePower = 0;
            kmVec3Fill( &m_vec3ForceDirection, 0.0, -1.0, 0.0 );
            m_fLiveTime = 1000;
            m_fBirthRadin = 0.0f;
        }
        ~SParticle( )
        {

        }
    };
    class CParticleEntity
    {
    public:
        CParticleEntity( );
        ~CParticleEntity( );

        void Update( float dtt );
        void SetOriginPosition( const kmVec3& birthPosition );
        void SetOriginDirection( const kmVec3& direction );
        void SetSpeed( float velocity );
        void SetDecay( float velocityDecay );
        void SetForcePower( float power );
        void SetForceTangent( bool bTangent );
        void SetForceDirection( const kmVec3& direction );
        void SetLiveTime( float time );
        void SetTexUV( const CQuadT& quadt );

        void SetParticleBirthRadin( float radin );
        float GetParticleBirthRadin() const;

        bool IsAlive( );
        void Activate( );

        void GetCurrentPosition( kmVec3& position );

        float GetCurrentLiveTime() const;
        float GetTotalLiveTime() const;

        void GetCurrentDeriction( kmVec3& direction ) const;

        CBillboard* GetBillBoard() const;

    private:
        bool m_bIsAlive;
        bool m_bTangent;
        kmVec3 m_vec3CurrentPosition;
        kmVec3 m_vec3CurrentDirection;
        float m_fCurrentLiveTime;
        SParticle m_sParticle;
        float m_fCurrentSpeed;

        CBillboard* m_pBillboard;
        
    };
};

#endif // Particle_H_INCLUDE
