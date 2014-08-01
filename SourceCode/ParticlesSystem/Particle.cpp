#include "stdafx.h"
#include "Particle.h"
#include "Emitter.h"

#define PARTICLE_MASS 100

namespace NBeyondEngine
{
    CParticleEntity::CParticleEntity()
        : m_bIsAlive(true)
        , m_bTangent(false)
        , m_fCurrentLiveTime(0)
        , m_fCurrentSpeed(0)
        , m_pBillboard(new CBillboard()) 
    {
        kmVec3Zero(&m_vec3CurrentPosition);
        kmVec3Zero(&m_vec3CurrentDirection);

        Activate();
    }

    CParticleEntity::~CParticleEntity()
    {
        BEATS_SAFE_DELETE( m_pBillboard );
    }

    void CParticleEntity::Update( float dtt )
    {
        m_fCurrentLiveTime += dtt;
        float updateTime = dtt;//calculation the update time
        if ( m_fCurrentLiveTime > m_sParticle.m_fLiveTime )
        {
            updateTime = dtt - ( m_fCurrentLiveTime - m_sParticle.m_fLiveTime );
            m_bIsAlive = false;//the particle is die
        }
        //calculation the run direction and distance
        kmVec3 pMove;
        m_fCurrentSpeed -= m_sParticle.m_fDecay * dtt;
        float distance = m_fCurrentSpeed * dtt;
        //calculation the position
        kmVec3Scale( &pMove, &m_sParticle.m_vec3OriginDirection, distance );

        // s = v0 * t + 0.5 * a * t * t, a = f / m(m is the mass)
        if ( m_sParticle.m_fForcePower != 0 )
        {
            if ( m_bTangent )
            {
                kmVec3 pDirection;
                GetCurrentDeriction( pDirection );
                kmVec3 upAlex;
                kmVec3Fill(&upAlex, 0.0f, 1.0f, 0.0f);
                kmVec3 faceAlex;
                kmVec3Cross( &faceAlex, &pDirection, &upAlex );
                if ( kmVec3Length( &faceAlex ) < 1e-6 )
                {
                    kmVec3Fill(&upAlex, 1.0f, 0.0f, 0.0f);
                    kmVec3Cross( &faceAlex, &pDirection, &upAlex );
                }
                kmVec3Cross( &m_sParticle.m_vec3ForceDirection, &faceAlex, &pDirection );
                kmVec3Normalize( &m_sParticle.m_vec3ForceDirection , &m_sParticle.m_vec3ForceDirection );
            }
            float length = 0.5f * m_sParticle.m_fForcePower / PARTICLE_MASS * m_fCurrentLiveTime * m_fCurrentLiveTime;
            kmVec3 direction;
            kmVec3Scale( &direction, &m_sParticle.m_vec3ForceDirection, length );
            kmVec3Add( &pMove, &pMove, &direction );
        }
        kmVec3 oldPosition;
        kmVec3Fill( &oldPosition, m_vec3CurrentPosition.x, m_vec3CurrentPosition.y, m_vec3CurrentPosition.z);
        kmVec3Add( &m_vec3CurrentPosition, &oldPosition, &pMove );
        kmVec3Subtract( &m_vec3CurrentDirection, &m_vec3CurrentPosition, &oldPosition );
    }

    void CParticleEntity::SetOriginPosition( const kmVec3& birthPosition )
    {
        kmVec3Fill( &m_sParticle.m_vec3OriginPosition, birthPosition.x, birthPosition.y, birthPosition.z );
        kmVec3Fill( &m_vec3CurrentPosition, birthPosition.x, birthPosition.y, birthPosition.z );
    }

    void CParticleEntity::SetOriginDirection( const kmVec3& direction )
    {
        kmVec3Fill( &m_sParticle.m_vec3OriginDirection, direction.x, direction.y, direction.z );
        if ( kmVec3Length( &m_sParticle.m_vec3OriginDirection ) > 0 )
        {
            kmVec3Normalize( &m_sParticle.m_vec3OriginDirection , &m_sParticle.m_vec3OriginDirection );
        }
        kmVec3Fill( &m_vec3CurrentDirection , m_sParticle.m_vec3OriginDirection.x,
            m_sParticle.m_vec3OriginDirection.y, m_sParticle.m_vec3OriginDirection.z);
    }

    void CParticleEntity::SetSpeed( float velocity )
    {
        m_sParticle.m_fSpeed = velocity;
        m_fCurrentSpeed = velocity;
    }

    void CParticleEntity::SetDecay( float velocityDecay )
    {
        m_sParticle.m_fDecay = velocityDecay;
    }

    void CParticleEntity::SetForcePower( float power )
    {
        m_sParticle.m_fForcePower = power;
    }

    void CParticleEntity::SetForceDirection( const kmVec3& direction )
    {
        kmVec3Fill( &m_sParticle.m_vec3ForceDirection, direction.x, direction.y, direction.z );
        kmVec3Normalize( &m_sParticle.m_vec3ForceDirection , &m_sParticle.m_vec3ForceDirection );
    }

    void CParticleEntity::SetLiveTime( float time )
    {
        m_sParticle.m_fLiveTime = time;
    }

    bool CParticleEntity::IsAlive()
    {
        return m_bIsAlive;
    }

    void CParticleEntity::Activate()
    {
        m_bIsAlive = true;
        kmVec3Fill( &m_vec3CurrentPosition, 0.0, 0.0, 0.0 );
        m_pBillboard->Reset();
        m_fCurrentLiveTime = 0;
    }

    void CParticleEntity::GetCurrentPosition( kmVec3& position )
    {
        kmVec3Fill( &position, m_vec3CurrentPosition.x, m_vec3CurrentPosition.y, m_vec3CurrentPosition.z );
    }

    void CParticleEntity::SetTexUV( const CQuadT& quadt )
    {
        m_pBillboard->SetUV( quadt );
    }

    CBillboard* CParticleEntity::GetBillBoard() const
    {
        return m_pBillboard;
    }

    float CParticleEntity::GetCurrentLiveTime() const
    {
        return m_fCurrentLiveTime;
    }

    float CParticleEntity::GetTotalLiveTime() const
    {
        return m_sParticle.m_fLiveTime;
    }

    void CParticleEntity::SetParticleBirthRadin( float radin )
    {
        m_sParticle.m_fBirthRadin = radin;
    }

    float CParticleEntity::GetParticleBirthRadin() const
    {
        return m_sParticle.m_fBirthRadin;
    }

    void CParticleEntity::GetCurrentDeriction( kmVec3& direction ) const
    {
        kmVec3Fill( &direction, m_vec3CurrentDirection.x, m_vec3CurrentDirection.y, m_vec3CurrentDirection.z );
        kmVec3Normalize( &direction, &direction );
    }

    void CParticleEntity::SetForceTangent( bool bTangent )
    {
        m_bTangent = bTangent;
    }
};
