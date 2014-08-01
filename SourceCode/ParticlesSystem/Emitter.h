#ifndef BEYOND_ENGINE_PARTICLESYSTEM_EMITTER_H_INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_EMITTER_H_INCLUDE

#include "Particle.h"
#include "Render/ShaderProgram.h"
#include "Render/Material.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "BillBoardSet.h"
#include "Render/ReflectTextureInfo.h"

namespace NBeyondEngine
{
    enum EEmitterType
    {
        eET_NULL = 0,
        eET_HEMISPHERE,
        eET_BOX,
        eET_SPHERE,
        eET_CONE
    };

    enum EParticleType
    {
        ePT_BILLBOARD_FACECAMERA_TOTALLY = 0,//aways face to camera
        ePT_BILLBOARD_FACECAMERA,
        ePT_BILLBOARD_FACEZ,//away face to one axis of the coordinate
        ePT_BILLBOARD_FACEY,
        ePT_BILLBOARD_FACEX
    };
    class CParticleSystem;
    class CParticleEmitter : public CComponentInstance
    {
        typedef std::vector< CParticleEntity* > TParticleList;
        DECLARE_REFLECT_GUID( CParticleEmitter, 0x1544A159, CComponentInstance )
    public:
        CParticleEmitter();
        CParticleEmitter( CMaterial* pMaterial );
        virtual ~CParticleEmitter();

        virtual void ReflectData(CSerializer& serializer) override;
        virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
        virtual bool OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent) override;
        virtual void Initialize() override;

        void BeginShoot( );

        // Update the emitter before render
        void Update( float dtt );
        void SetTotalCount( unsigned int count );
        unsigned int GetTotleCount( );
        void SetShootDeriction( const kmVec3& deriction );
        const kmVec3& GetShootDeriction( ) const;
        void SetShootOrigin( const kmVec3& origin );
        const kmVec3& GetShootOrigin( ) const;
        void SetShootVelocity( unsigned int velocity );
        unsigned int GetShootVelocity( );
        void SetMinParticleVelocity( float velocity );
        float GetMinParticleVelocity( );
        void SetMaxParticleVelocity( float velocity );
        float GetMaxParticleVelocity( );
        void SetMinParticleVelocityDecay( float decay );
        float GetMinParticleVelocityDecay( );
        void SetMaxParticleVelocityDecay( float decay );
        float GetMaxParticleVelocityDecay();
        void SetForcePower( float power );
        float GetForcePower( );
        void SetFroceDeriction( const kmVec3& deriction );
        const kmVec3& GetFroceDeriction( ) const;
        void SetMinLiveTime( float time );
        float GetMinLiveTime( );
        void SetMaxLiveTime( float time );
        float GetMaxLiveTime( );
        void SetDelayTime( float time );
        float GetDelayTime( );
        void SetEmitterLiveTime( float time );
        float GetEmitterLiveTime( );
        void SetParticleConstraint( bool bLocalCoordinate );
        bool GetParticleConstraint( );
        void SetPlayLoop( bool bLoop );
        bool GetPlayLoop( );

        void SetParticleWidth( float fWidth );
        float GetParticleWidth() const;
        void SetParticleHeight( float fHeight );
        float GetParticleHeight() const;

        void SetEmitterType( EEmitterType type );
        EEmitterType GetEmittertype() const;

        void Visit( const kmMat4* transform );

        bool IsDied() const;

        void Stop();

    private:

        void ShootParicle( unsigned int count );
        void SetParticleInitProperty( CParticleEntity** pParticleEntity );
        CParticleEntity* GetParticleFromRecycle( ); 
        void UpdateActiveParticles( float dtt );
        void DeleteParticles();
        float RotateParticle( const kmVec3& upVector );
        void GetBirthInformation( EEmitterType type, bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction );
        void GetBirthInformationHemisphere( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction );
        void GetBirthInformationSphere( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction );
        void GetBirthInformationBox( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction );
        void GetBirthInformationCone( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction );

        void CreateDefautMaterial();
    private:

        bool m_bLocalCoord;
        bool m_bIsLoop;
        bool m_bForceTangent;
        bool m_bRandomDeriction;
        bool m_bShootAtShell;
        bool m_bParticleRandomDirection;
        bool m_bSortParticle;
        bool m_bIsDied;
        bool m_bStop;
        bool m_bDirectionRestrain;
        unsigned int m_uTotalCount;
        unsigned int m_uRatePerSecond;
        unsigned int m_uCurrentCount;
        float m_fMinParticleSpeed;
        float m_fMaxParticleSpeed;
        float m_fMinSpeedDecay;
        float m_fMaxSpeedDecay;
        float m_fForcePower;
        float m_fMinLiveTime;
        float m_fMaxLiveTime;
        float m_fDelayTime;
        float m_fEmitterLiveTime;
        float m_fCurrentTime;
        float m_fBeginTime;
        float m_fMinPreShootTime;
        float m_fDeltaTime;
        float m_fHemisphereRadius;
        float m_fConeRadius;
        float m_fConeDegree;
        float m_fSphereRadius;
        float m_fWidth;
        float m_fHeight;
        EEmitterType m_eEmitterType;
        EParticleType m_particalType;
        CMaterial* m_pMaterial;
        CColorParticleAnimation* m_pColorAnimation;
        CRotateParticleAnimation* m_pRotateAnimation;
        CScaleParticleAnimation* m_pScaleAnimation;
        
        CVec3 m_vec3Origin;
        CVec3 m_vec3Direction;
        CVec3 m_vec3ForceDirection;

        TParticleList m_aliveParticleList;
        TParticleList m_particleRecycle;

        SharePtr<CMaterial> m_pMaterialDefaut;

        //textures
        std::vector< SReflectTextureInfo > m_vecTextures;

        //billboard set
        CBillboardSet m_billboardSet;
        CVec3 m_boxSize;
    };
};

#endif // Emitter_H_INCLUDE
