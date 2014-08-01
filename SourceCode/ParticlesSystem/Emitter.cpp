#include "stdafx.h"
#include "Emitter.h"
#include "Render/Renderer.h"
#include "Resource/ResourceManager.h"
#include "Render/Shader.h"
#include "Render/TextureFrag.h"
#include "Render/TextureAtlas.h"
#include "Scene/SceneManager.h"
#include "Render/ReflectTextureInfo.h"

namespace NBeyondEngine
{
    CParticleEmitter::CParticleEmitter()
        : m_bLocalCoord(true)
        , m_bIsLoop(true)
        , m_bForceTangent(false)
        , m_bRandomDeriction(false)
        , m_bShootAtShell(false)
        , m_bParticleRandomDirection(false)
        , m_bSortParticle(false)
        , m_bIsDied(false)
        , m_bStop(false)
        , m_bDirectionRestrain(false)
        , m_uTotalCount(10)
        , m_uRatePerSecond(5)
        , m_uCurrentCount(0)
        , m_fMinParticleSpeed(1.0f)
        , m_fMaxParticleSpeed(3.0f)
        , m_fMinSpeedDecay(0.0f)
        , m_fMaxSpeedDecay(0.0f)
        , m_fForcePower(0.0f)
        , m_fMinLiveTime(3.0f)
        , m_fMaxLiveTime(5.0f)
        , m_fDelayTime(0.0f)
        , m_fEmitterLiveTime(-1.0f)
        , m_fCurrentTime(0.0f)
        , m_fBeginTime(-1.0f)
        , m_fMinPreShootTime(5.0f)
        , m_fDeltaTime(0.0f)
        , m_fHemisphereRadius(1.0f)
        , m_fConeRadius(1.0f)
        , m_fConeDegree(0.0f)
        , m_fSphereRadius(1.0f)
        , m_fWidth(1.0f)
        , m_fHeight(1.0f)
        , m_eEmitterType(eET_BOX)
        , m_particalType(ePT_BILLBOARD_FACECAMERA_TOTALLY)
        , m_pMaterial(nullptr)
        , m_pColorAnimation(nullptr)
        , m_pRotateAnimation(nullptr)
        , m_pScaleAnimation(nullptr)
    {
        kmVec3Fill( &m_vec3Origin, 0.0, 0.0, 0.0 );
        kmVec3Fill( &m_vec3Direction, 0.0, 1.0, 0.0 );
        kmVec3Fill( &m_vec3ForceDirection, 0.0, -1.0, 0.0 );
        kmVec3Fill( &m_boxSize, 1.0f, 1.0f, 1.0f );
        CreateDefautMaterial();
    }

    CParticleEmitter::CParticleEmitter( CMaterial* pMaterial )
        : m_bLocalCoord(true)
        , m_bIsLoop(true)
        , m_bForceTangent(false)
        , m_bRandomDeriction(false)
        , m_bShootAtShell(false)
        , m_bParticleRandomDirection(false)
        , m_bSortParticle(false)
        , m_bIsDied(false)
        , m_bStop(false)
        , m_bDirectionRestrain(false)
        , m_uTotalCount(10)
        , m_uRatePerSecond(5)
        , m_uCurrentCount(0)
        , m_fMinParticleSpeed(1.0f)
        , m_fMaxParticleSpeed(3.0f)
        , m_fMinSpeedDecay(0.0f)
        , m_fMaxSpeedDecay(0.0f)
        , m_fForcePower(0.0f)
        , m_fMinLiveTime(3.0f)
        , m_fMaxLiveTime(5.0f)
        , m_fDelayTime(0.0f)
        , m_fEmitterLiveTime(-1.0f)
        , m_fCurrentTime(0.0f)
        , m_fBeginTime(-1.0f)
        , m_fMinPreShootTime(5.0f)
        , m_fDeltaTime(0.0f)
        , m_fHemisphereRadius(1.0f)
        , m_fConeRadius(1.0f)
        , m_fConeDegree(0.0f)
        , m_fSphereRadius(1.0f)
        , m_fWidth(1.0f)
        , m_fHeight(1.0f)
        , m_eEmitterType(eET_BOX)
        , m_particalType(ePT_BILLBOARD_FACECAMERA_TOTALLY)
        , m_pMaterial(nullptr)
        , m_pColorAnimation(nullptr)
        , m_pRotateAnimation(nullptr)
        , m_pScaleAnimation(nullptr)
    {

        kmVec3Fill( &m_vec3Origin, 0.0, 0.0, 0.0 );
        kmVec3Fill( &m_vec3Direction, 0.0, 1.0, 0.0 );
        kmVec3Fill( &m_vec3ForceDirection, 0.0, -1.0, 0.0 );
        kmVec3Fill( &m_boxSize, 1.0f, 1.0f, 1.0f );
        CreateDefautMaterial();

        m_pMaterial = pMaterial;
        if ( m_pMaterial )
        {
            m_billboardSet.SetMaterial( m_pMaterial );
        }
        else
        {
            m_billboardSet.SetMaterial( m_pMaterialDefaut );
        }
    }

    CParticleEmitter::~CParticleEmitter()
    {
        DeleteParticles();
    }

    void CParticleEmitter::ReflectData(CSerializer& serializer)
    {
        TCHAR buf[MAX_PATH] = {0};
        super::ReflectData(serializer);
        DECLARE_PROPERTY( serializer, m_eEmitterType, true, 0xFFFFFFF, _T("发射器类型"), NULL, NULL, NULL);
        sprintf(buf, _T("VisibleWhen:m_eEmitterType == %d"), eET_HEMISPHERE);
        DECLARE_PROPERTY( serializer, m_fHemisphereRadius, true, 0xFFFFFFF, _T("半球发射器半径"), _T("半球发射器属性"), NULL, buf);
        sprintf(buf, _T("VisibleWhen:m_eEmitterType == %d"), eET_BOX);
        DECLARE_PROPERTY( serializer, m_boxSize, true, 0xFFFFFFF, _T("发射器大小"), _T("盒子发射器属性"), NULL, buf);
        sprintf(buf, _T("VisibleWhen:m_eEmitterType == %d"), eET_SPHERE);
        DECLARE_PROPERTY( serializer, m_fSphereRadius, true, 0xFFFFFFF, _T("发射器半径"), _T("球发射器属性"), NULL, buf);
        sprintf(buf, _T("VisibleWhen:m_eEmitterType == %d"), eET_CONE);
        DECLARE_PROPERTY( serializer, m_fConeRadius, true, 0xFFFFFFF, _T("发射器地面半径"), _T("椎发射器属性"), NULL, buf);
        DECLARE_PROPERTY( serializer, m_fConeDegree, true, 0xFFFFFFF, _T("发射角度"), _T("椎发射器属性"), NULL, buf);
        DECLARE_PROPERTY( serializer, m_uTotalCount, true, 0xFFFFFFFF, _T( "发射粒子总数" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_vec3Origin, true, 0xFFFFFFFF, _T( "发射器原点坐标" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_uRatePerSecond, true, 0xFFFFFFFF, _T( "每秒发射粒子个数" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fMinParticleSpeed, true, 0xFFFFFFFF, _T( "最小运动速度" ), _T( "粒子运动速度" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fMaxParticleSpeed, true, 0xFFFFFFFF, _T( "最大运动速度" ), _T( "粒子运动速度" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fMinSpeedDecay, true, 0xFFFFFFFF, _T( "最小衰减速度" ), _T( "粒子运动速度衰减" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fMaxSpeedDecay, true, 0xFFFFFFFF, _T( "最大衰减速度" ), _T( "粒子运动速度衰减" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fForcePower, true, 0xFFFFFFFF, _T( "外力大小" ), _T( "外力" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bForceTangent, true, 0xFFFFFFF, _T("外力方向与运动方向相切"), _T("外力"), NULL, NULL);
        DECLARE_PROPERTY( serializer, m_vec3ForceDirection, true, 0xFFFFFFFF, _T( "外力方向" ), _T( "外力" ), NULL, _T("VisibleWhen: m_bForceTangent == false") );
        DECLARE_PROPERTY( serializer, m_fMinLiveTime, true, 0xFFFFFFFF, _T( "最小生存时间" ), _T( "粒子运动生存周期" ), _T( "单位(s)" ), NULL );
        DECLARE_PROPERTY( serializer, m_fMaxLiveTime, true, 0xFFFFFFFF, _T( "最大生存时间" ), _T( "粒子运动生存周期" ), _T( "单位(s)" ), NULL );
        DECLARE_PROPERTY( serializer, m_fDelayTime, true, 0xFFFFFFFF, _T( "延迟发射时间" ), NULL, _T( "单位(s)" ), NULL );
        DECLARE_PROPERTY( serializer, m_bLocalCoord, true, 0xFFFFFFFF, _T( "是否约束粒子" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bIsLoop, true, 0xFFFFFFFF, _T( "是否循环播放" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bRandomDeriction, true, 0xFFFFFFFF, _T( "随机运动方向" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bParticleRandomDirection, true, 0xFFFFFFFF, _T( "产生粒子随机方向" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fEmitterLiveTime, true, 0xFFFFFFFF, _T( "发射器生存时间" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bShootAtShell, true, 0xFFFFFFFF, _T( "是否在表面发射" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_vecTextures, true, 0xFFFFFFF, _T("纹理集"),NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fWidth, true, 0xFFFFFFF, _T("粒子宽度"),_T("粒子属性"), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_fHeight, true, 0xFFFFFFF, _T("粒子高度"),_T("粒子属性"), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_particalType, true, 0xFFFFFFF, _T("粒子类型"),_T("粒子属性"), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bDirectionRestrain, true, 0xFFFFFFF, _T("方向与运动方向重合"),_T("粒子属性"), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pRotateAnimation, true, 0xFFFFFFF, _T("旋转动画"),_T("粒子动画"), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pScaleAnimation, true, 0xFFFFFFF, _T("放缩动画"),_T("粒子动画"), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pColorAnimation, true, 0xFFFFFFF, _T("颜色动画"),_T("粒子动画"), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bSortParticle, true, 0xFFFFFFFF, _T("粒子自动排序"), NULL, NULL, NULL );
        DECLARE_DEPENDENCY( serializer, m_pMaterial, _T( "材质" ), eDT_Weak);
    }

    void CParticleEmitter::BeginShoot()
    {
        m_fBeginTime = 0;
        DeleteParticles();
    }

    void CParticleEmitter::Update( float dtt )
    {
        do 
        {
            if ( m_fBeginTime < 0 )
            {
                break;
            }
            if ( m_eEmitterType != eET_NULL )
            {
                UpdateActiveParticles( dtt );

                if ( m_fDelayTime > 0 && m_fBeginTime < m_fDelayTime )//delay emit
                {
                    m_fBeginTime += dtt;
                    if ( m_fBeginTime < m_fDelayTime )
                    {
                        break;
                    }
                }
                if ( -1.0f != m_fEmitterLiveTime )
                {
                    if ( m_fCurrentTime > m_fEmitterLiveTime )
                    {
                        m_bIsDied = true;
                        break;
                    }
                }
                if ( m_bStop )
                {
                    m_bIsDied = true;
                    break;
                }
                m_fCurrentTime += dtt;
                if ( m_uCurrentCount < m_uTotalCount || m_bIsLoop )
                {
                    //To calculation how many particles will be shoot in current frame
                    //check the time is longer than the minimum emit time
                    m_fDeltaTime += dtt;
                    int iCount = (int)( m_fDeltaTime / m_fMinPreShootTime);
                    if ( iCount > 0 )
                    {
                        ShootParicle( iCount );
                        m_fDeltaTime -= iCount * m_fMinPreShootTime;
                    }
                }
            }
        } while ( false );
    }

    void CParticleEmitter::ShootParicle( unsigned int count )
    {
        BEATS_ASSERT( m_eEmitterType != eET_NULL , _T( "The emitter type can't be sure!" ) );
        m_uCurrentCount += count;
        unsigned int pCreateCount = count;
        unsigned int pReUseCount = 0;
        if ( m_uCurrentCount >= m_uTotalCount )//check the particles number is bigger than the total number
        {
            pReUseCount = m_uCurrentCount - m_uTotalCount;
            pCreateCount -= pReUseCount;
            m_uCurrentCount = m_uTotalCount;
            if ( !m_bIsLoop )
            {
                pReUseCount = 0;
            }
            else
            {
                m_fBeginTime = m_fDelayTime;
            }
        }

        //create a new particle
        for ( unsigned int i = 0; i < pCreateCount; ++i )
        {
            CParticleEntity* pParticle = new CParticleEntity( );
            kmVec3 birthPosition, birthDeriction;
            GetBirthInformation ( m_eEmitterType, m_bShootAtShell, m_bRandomDeriction, birthPosition, birthDeriction );
            kmVec3Add( &birthPosition, &m_vec3Origin, &birthPosition );
            pParticle->SetOriginPosition( birthPosition );
            pParticle->SetOriginDirection( birthDeriction );
            SetParticleInitProperty( &pParticle );
            m_aliveParticleList.push_back( pParticle );
        }

        //get the particle from the recycle
        for ( unsigned int i = 0; i < pReUseCount; ++i )
        {
            CParticleEntity* pParticle = GetParticleFromRecycle( );
            if ( pParticle )
            {
                pParticle->Activate();//active the died particle
                kmVec3 birthPosition, birthDeriction;
                GetBirthInformation( m_eEmitterType, m_bShootAtShell, m_bRandomDeriction, birthPosition, birthDeriction );
                kmVec3Add( &birthPosition, &m_vec3Origin, &birthPosition );
                pParticle->SetOriginPosition( birthPosition );
                pParticle->SetOriginDirection( birthDeriction );
                SetParticleInitProperty( &pParticle );
                m_aliveParticleList.push_back( pParticle );
            }
        }
    }


    void CParticleEmitter::SetTotalCount( unsigned int count )
    {
        m_uTotalCount = count;
        DeleteParticles();
    }

    unsigned int CParticleEmitter::GetTotleCount()
    {
        return m_uTotalCount;
    }


    void CParticleEmitter::SetShootDeriction( const kmVec3& deriction )
    {
        kmVec3Fill( &m_vec3Direction, deriction.x, deriction.y, deriction.z );
    }

    const kmVec3& CParticleEmitter::GetShootDeriction() const
    {
        return m_vec3Direction;
    }

    void CParticleEmitter::SetShootOrigin( const kmVec3& origin )
    {
        kmVec3Fill( &m_vec3Origin, origin.x, origin.y, origin.z );
    }

    const kmVec3& CParticleEmitter::GetShootOrigin() const
    {
        return m_vec3Origin;
    }

    void CParticleEmitter::SetShootVelocity( unsigned int velocity )
    {
        m_uRatePerSecond = velocity;
        m_fMinPreShootTime = 1.0f / velocity;
    }

    unsigned int CParticleEmitter::GetShootVelocity()
    {
        return m_uRatePerSecond;
    }

    void CParticleEmitter::SetMinParticleVelocity( float velocity )
    {
        m_fMinParticleSpeed = velocity;
    }

    float CParticleEmitter::GetMinParticleVelocity()
    {
        return m_fMinParticleSpeed;
    }

    void CParticleEmitter::SetMaxParticleVelocity( float velocity )
    {
        m_fMaxParticleSpeed = velocity;
    }

    float CParticleEmitter::GetMaxParticleVelocity()
    {
        return m_fMaxParticleSpeed;
    }

    void CParticleEmitter::SetMinParticleVelocityDecay( float decay )
    {
        m_fMinSpeedDecay = decay;
    }

    float CParticleEmitter::GetMinParticleVelocityDecay()
    {
        return m_fMinSpeedDecay;
    }

    void CParticleEmitter::SetMaxParticleVelocityDecay( float decay )
    {
        m_fMaxSpeedDecay = decay;
    }

    float CParticleEmitter::GetMaxParticleVelocityDecay()
    {
        return m_fMaxSpeedDecay;
    }

    void CParticleEmitter::SetForcePower( float power )
    {
        m_fForcePower = power;
    }

    float CParticleEmitter::GetForcePower()
    {
        return m_fForcePower;
    }

    void CParticleEmitter::SetFroceDeriction( const kmVec3& deriction )
    {
        kmVec3Fill( &m_vec3ForceDirection, deriction.x, deriction.y, deriction.z );
    }

    const kmVec3& CParticleEmitter::GetFroceDeriction() const
    {
        return m_vec3ForceDirection;
    }

    void CParticleEmitter::SetMinLiveTime( float time )
    {
        m_fMinLiveTime = time;
    }

    float CParticleEmitter::GetMinLiveTime()
    {
        return m_fMinLiveTime;
    }

    void CParticleEmitter::SetMaxLiveTime( float time )
    {
        m_fMaxLiveTime = time;
    }

    float CParticleEmitter::GetMaxLiveTime()
    {
        return m_fMaxLiveTime;
    }

    void CParticleEmitter::SetDelayTime( float time )
    {
        m_fDelayTime = time;
    }

    float CParticleEmitter::GetDelayTime()
    {
        return m_fDelayTime;
    }

    void CParticleEmitter::SetEmitterLiveTime(float time)
    {
        m_fEmitterLiveTime = time;
    }

    float CParticleEmitter::GetEmitterLiveTime()
    {
        return m_fEmitterLiveTime;
    }

    void CParticleEmitter::SetParticleConstraint( bool bConstraint )
    {
        m_bLocalCoord = bConstraint;
    }

    bool CParticleEmitter::GetParticleConstraint()
    {
        return m_bLocalCoord;
    }

    void CParticleEmitter::UpdateActiveParticles( float dtt )
    {
        for ( auto i : m_aliveParticleList )
        {
            CParticleEntity* pParticle = i;
            pParticle->Update( dtt );
        }
    }

    void CParticleEmitter::SetParticleInitProperty( CParticleEntity** pParticleEntity )
    {
        CParticleEntity* pParticle = *pParticleEntity;
        pParticle->SetForcePower( m_fForcePower );
        pParticle->SetForceDirection( m_vec3ForceDirection );
        float tempNum = RANGR_RANDOM_FLOAT( m_fMinParticleSpeed, m_fMaxParticleSpeed );
        pParticle->SetSpeed( tempNum );
        tempNum = RANGR_RANDOM_FLOAT( m_fMinSpeedDecay, m_fMaxSpeedDecay );
        pParticle->SetDecay( tempNum );
        tempNum = RANGR_RANDOM_FLOAT( m_fMinLiveTime, m_fMaxLiveTime );
        pParticle->SetLiveTime( tempNum );
        pParticle->SetForceTangent( m_bForceTangent );
        if ( m_bParticleRandomDirection )
        {
            float fRadin = RANGR_RANDOM_FLOAT( 0.0f, MATH_PI_DOUBLE );
            pParticle->SetParticleBirthRadin( fRadin );
        }
        size_t count = m_vecTextures.size();
        if ( count > 0 )
        {
            size_t index = 0;
            if ( count > 1 )
            {
                index = rand() % count;
            }
            const SReflectTextureInfo& texutreInfo = m_vecTextures[ index ];
            CTextureFrag* pFrag = texutreInfo.GetTextureFrag( );
            if ( pFrag )
            {
                pParticle->SetTexUV( pFrag->Quad() );
            }
        }
    }

    CParticleEntity* CParticleEmitter::GetParticleFromRecycle()
    {
        CParticleEntity* pParticle = 0;
        if ( !m_particleRecycle.empty() )
        {
            TParticleList::iterator iter = m_particleRecycle.begin();
            pParticle =  *iter;
            m_particleRecycle.erase( iter );
        }
        return pParticle;
    }

    void CParticleEmitter::SetPlayLoop( bool bLoop )
    {
        m_bIsLoop = bLoop;
    }

    bool CParticleEmitter::GetPlayLoop()
    {
        return m_bIsLoop;
    }

    bool CParticleEmitter::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
    {
        bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
        if (!bRet)
        {
            if ( &m_uTotalCount == pVariableAddr )
            {
                DeserializeVariable(m_uTotalCount, pSerializer);
                SetTotalCount( m_uTotalCount );
                bRet = true;
            }
            else if( &m_uRatePerSecond == pVariableAddr )
            {
                DeserializeVariable(m_uRatePerSecond, pSerializer);
                SetShootVelocity( m_uRatePerSecond );
                bRet = true;
            }
            else if( &m_vecTextures == pVariableAddr )
            {
                DeserializeVariable( m_vecTextures, pSerializer );
                if ( m_pMaterial )
                {
                    //set the default shaders
                    for ( size_t i = 0 ; i < m_vecTextures.size(); ++i )
                    {
                        if ( m_vecTextures[i].GetTextureFrag() != NULL )
                        {
                            SharePtr<CTextureAtlas> pAtals = m_vecTextures[i].GetAtlas();
                            if ( pAtals )
                            {
                                m_pMaterial->SetTexture( i, pAtals->Texture() );
                            }
                        }
                    }
                }
                bRet = true;
            }
        }
        return bRet;
    }


    bool CParticleEmitter::OnDependencyChange( void* pComponentAddr, CComponentBase* pComponent )
    {
        bool bRet = super::OnDependencyChange(pComponentAddr, pComponent);
        if ( &m_pMaterial == pComponentAddr )
        {
            m_pMaterial = (CMaterial*)pComponent;
            if( m_pMaterial )
            {
                m_billboardSet.SetMaterial( m_pMaterial );
            }
            else
            {
                m_billboardSet.SetMaterial( m_pMaterialDefaut );
            }
            bRet = true;
        }
        return bRet;
    }
    void CParticleEmitter::Initialize()
    {
        super::Initialize();
        SetTotalCount( m_uTotalCount );
        SetShootVelocity( m_uRatePerSecond );
        if ( m_pMaterial )
        {
            m_billboardSet.SetMaterial( m_pMaterial );
        }
        else
        {
            m_billboardSet.SetMaterial( m_pMaterialDefaut );
        }
    }


    void CParticleEmitter::DeleteParticles()
    {
        for ( auto iter : m_aliveParticleList )
        {
            BEATS_SAFE_DELETE( iter );
        }

        for ( auto iter : m_particleRecycle )
        {
            BEATS_SAFE_DELETE( iter );
        }
        m_aliveParticleList.clear();
        m_particleRecycle.clear();
        m_uCurrentCount = 0;
        m_fCurrentTime = 0.0f;
        m_fDeltaTime = 0.0f;
        m_bIsDied = false;
        m_bStop = false;
    }

    void CParticleEmitter::Visit( const kmMat4* transform )
    {
        m_billboardSet.Clear();
        if ( !m_aliveParticleList.empty())
        {
            TParticleList::iterator it = m_aliveParticleList.begin();
            for ( ; it != m_aliveParticleList.end(); )
            {
                CParticleEntity* pParticle = *it;
                BEATS_ASSERT( nullptr != pParticle );
                CBillboard* pBillboard = pParticle->GetBillBoard();
                if ( pBillboard )
                {
                    kmVec3 position;
                    pParticle->GetCurrentPosition( position );

                    float fCurrentTime = pParticle->GetCurrentLiveTime();
                    float fTotalTime = pParticle->GetTotalLiveTime();
                    float fRotateRadin = pParticle->GetParticleBirthRadin();
                    float fScaleWidth = m_fWidth, fScaleHeight = m_fHeight;
                    if ( m_pScaleAnimation )
                    {
                        const kmVec2* pScale = (const kmVec2*)m_pScaleAnimation->GetCurrentAnimationValue( fCurrentTime, fTotalTime );
                        BEATS_ASSERT( nullptr != pScale );
                        fScaleWidth *= pScale->x;
                        fScaleHeight *= pScale->y;
                    }
                    if ( m_pRotateAnimation )
                    {
                        const float* rotate = (const float*)m_pRotateAnimation->GetCurrentAnimationValue( fCurrentTime, fTotalTime );
                        BEATS_ASSERT( nullptr != rotate );
                        fRotateRadin += *rotate;
                    }
                    if( m_pColorAnimation )
                    {
                        const CColor* color = (const CColor*)m_pColorAnimation->GetCurrentAnimationValue( fCurrentTime, fTotalTime );
                        BEATS_ASSERT( nullptr != color );
                        pBillboard->SetColor( *color );
                    }
                    if ( m_bDirectionRestrain )
                    {
                        kmVec3 upVector;
                        pParticle->GetCurrentDeriction( upVector );
                        fRotateRadin += RotateParticle( upVector );
                    }
                    pBillboard->SetPosition( position );
                    pBillboard->SetSize( fScaleWidth, fScaleHeight );
                    pBillboard->Roll( fRotateRadin );
                    m_billboardSet.AddBillboard( pBillboard );
                }
                if ( !pParticle->IsAlive() )
                {
                    m_particleRecycle.push_back( *it );
                    it = m_aliveParticleList.erase( it );
                }
                else
                {
                    ++it;
                }
            }

            if ( m_bSortParticle )
            {
                m_billboardSet.SortBillBoards();
            }
        }
        SharePtr<CMaterial> pMaterial = m_billboardSet.GetMaterial();
        if ( pMaterial )
        {
            if ( m_vecTextures.size() > 0 )
            {
                const SReflectTextureInfo& texInfo = m_vecTextures[0];
                if ( texInfo.GetTextureFrag() != NULL )
                {
                    SharePtr<CTextureAtlas> pAtals = texInfo.GetAtlas();
                    if ( pAtals )
                    {
                        pMaterial->SetTexture( 0, pAtals->Texture() );
                    }
                }
                else
                {
                    pMaterial->SetTexture( 0, nullptr );
                }
            }
            else
            {
                pMaterial->SetTexture( 0, nullptr );
            }
        }
        //render
        m_billboardSet.SetType( (EBillboardType)m_particalType );
        //TODO: Try to replace it. I comment here just for compile.
        //m_billboardSet.SetTransform(transform);
        m_billboardSet.Render();
    }

    float CParticleEmitter::RotateParticle( const kmVec3& upVector  )
    {
        kmVec3 yAlex;
        kmVec3Fill( &yAlex, 0.0f, 1.0f, 0.0f );
        float dotValue = kmVec3Dot( &upVector, &yAlex );
        float radin = acos( dotValue );

        kmVec3 yAlexContrast;
        kmVec3Fill( &yAlexContrast, 0.0f, -1.0f, 0.0f );

        kmVec3 crossVector;
        kmVec3Cross( &crossVector, &yAlex, &upVector );
        kmVec3Normalize( &crossVector, &crossVector );
        kmVec3 crossVectorContrast;
        kmVec3Cross( &crossVectorContrast, &yAlexContrast, &upVector );
        kmVec3Normalize( &crossVectorContrast, &crossVectorContrast );
        
        if ( crossVectorContrast.x - crossVector.x < 1e-6 
            && crossVectorContrast.y - crossVector.y < 1e-6 
            && crossVectorContrast.z - crossVector.z < 1e-6)
        {
            radin = MATH_PI_DOUBLE - radin;
        }

        return radin;
    }

    void CParticleEmitter::GetBirthInformation( EEmitterType type, bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction )
    {
        switch ( type )
        {
        case eET_HEMISPHERE:
            GetBirthInformationHemisphere( bAtShell, bRandom, position, deriction );
            break;
        case eET_SPHERE:
            GetBirthInformationSphere( bAtShell, bRandom, position, deriction );
            break;
        case eET_BOX:
            GetBirthInformationBox( bAtShell, bRandom, position, deriction );
            break;
        case eET_CONE:
            GetBirthInformationCone( bAtShell, bRandom, position, deriction );
            break;
        default:
            BEATS_ASSERT( false, _T("type error"));
            break;
        }
    }

    void CParticleEmitter::GetBirthInformationHemisphere( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction )
    {
        //calculation position
        float x = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float y = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float z = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        kmVec3 randomDeriction;
        kmVec3Fill( &randomDeriction, x, y, z );
        kmVec3Normalize( &randomDeriction, &randomDeriction );
        kmScalar radius = m_fHemisphereRadius;
        if ( !bAtShell )
        {
            //Calculation position at the ball shell
            radius = RANGR_RANDOM_FLOAT( 0.0f, m_fHemisphereRadius );
        }
        kmVec3Scale( &position, &randomDeriction, radius );

        //calculation deriction
        if ( bRandom )
        {
            //if random deriction, there must random again
            float rx = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            float ry = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            float rz = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            kmVec3Fill( &deriction, rx, ry, rz );
            kmVec3Normalize( &deriction, &deriction );
        }
        else
        {
            kmVec3Fill( &deriction, randomDeriction.x, randomDeriction.y, randomDeriction.z );
        }
    }

    void CParticleEmitter::GetBirthInformationSphere( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction )
    {
        //calculation position
        float x = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float y = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float z = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        kmVec3 randomDeriction;
        kmVec3Fill( &randomDeriction, x, y, z );
        kmVec3Normalize( &randomDeriction, &randomDeriction );
        kmScalar radius = m_fSphereRadius;
        if ( !bAtShell )
        {
            //Calculation position at the ball shell
            radius = RANGR_RANDOM_FLOAT( 0.0f, m_fSphereRadius );
        }
        kmVec3Scale( &position, &randomDeriction, radius );

        //calculation deriction
        if ( bRandom )
        {
            //if random deriction, there must random again
            float rx = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            float ry = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            float rz = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            kmVec3Fill( &deriction, rx, ry, rz );
            kmVec3Normalize( &deriction, &deriction );
        }
        else
        {
            kmVec3Fill( &deriction, randomDeriction.x, randomDeriction.y, randomDeriction.z );
        }
    }

    void CParticleEmitter::GetBirthInformationBox( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction )
    {
        float x = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float y = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float z = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        if ( !bAtShell )
        {
            //Calculation position in the box
            x *= m_boxSize.x;
            y *= m_boxSize.y;
            z *= m_boxSize.z;
            kmVec3Fill( &position, x * 0.5f, y * 0.5f, z * 0.5f );
        }
        else
        {
            kmVec3 randomDeriction;
            kmVec3Fill( &randomDeriction, x, y, z );
            kmVec3Normalize( &randomDeriction, &randomDeriction );
            kmVec3Fill( &position, randomDeriction.x * m_boxSize.x * 0.5f, randomDeriction.y * m_boxSize.y * 0.5f, randomDeriction.z * m_boxSize.z * 0.5f );
        }

        //calculation deriction
        if ( bRandom )
        {
            //if random deriction, there must random again
            float rx = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            float ry = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            float rz = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
            kmVec3Fill( &deriction, rx, ry, rz );
            kmVec3Normalize( &deriction, &deriction );
        }
        else
        {
            kmVec3Fill( &deriction, x, y, z );
            kmVec3Normalize( &deriction, &deriction );
        }
    }

    void CParticleEmitter::GetBirthInformationCone( bool bAtShell, bool bRandom, kmVec3& position, kmVec3& deriction )
    {
        //calc cone bottom face position ==> y = 0
        float x = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float y = 0.0f;
        float z = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );

        kmVec3 pPosition;
        kmVec3Fill( &pPosition, x, y, z );
        kmVec3Normalize( &pPosition, &pPosition );
        kmScalar radius = m_fConeRadius;
        if ( !bAtShell )
        {
            radius = RANGR_RANDOM_FLOAT( 0.0f, m_fConeRadius );
        }
        kmVec3Scale( &position, &pPosition, radius );

        kmScalar tempRadian = kmDegreesToRadians( m_fConeDegree );
        tempRadian /= 2.0;
        if ( bRandom )
        {
            tempRadian = RANGR_RANDOM_FLOAT( 0.0f, tempRadian );
        }
        kmVec3 newUp;
        kmVec3Fill( &deriction, 0.0f, 1.0f, 0.0f );
        kmVec3Fill( &newUp, 0.0f, 0.0f, 1.0f );
        kmQuaternion quaternion;
        kmQuaternionRotationAxis( &quaternion, &deriction, ((float)rand() / (float) RAND_MAX ) * MATH_PI_DOUBLE );
        kmQuaternionMultiplyVec3( &newUp, &quaternion, &newUp );
        kmQuaternionRotationAxis( &quaternion, &newUp, tempRadian );
        kmQuaternionMultiplyVec3( &deriction, &quaternion, &deriction );
    }

    void CParticleEmitter::SetParticleWidth( float fWidth )
    {
        m_fWidth = fWidth;
    }

    float CParticleEmitter::GetParticleWidth() const
    {
        return m_fWidth;
    }

    void CParticleEmitter::SetParticleHeight( float fHeight )
    {
        m_fHeight = fHeight;
    }

    float CParticleEmitter::GetParticleHeight() const
    {
        return m_fHeight;
    }

    void CParticleEmitter::SetEmitterType( EEmitterType type )
    {
        m_eEmitterType = type;
    }

    NBeyondEngine::EEmitterType CParticleEmitter::GetEmittertype() const
    {
        return m_eEmitterType;
    }

    void CParticleEmitter::CreateDefautMaterial()
    {
        m_pMaterialDefaut = new CMaterial();
        m_pMaterialDefaut->SetSharders( _T("Billboard.vs"), _T("Billboard.ps"));
        m_pMaterialDefaut->SetBlendEnable( true );
        m_pMaterialDefaut->SetBlendDest( GL_ONE_MINUS_SRC_COLOR );
        m_pMaterialDefaut->SetBlendSource( GL_SRC_COLOR );
        m_pMaterialDefaut->SetDepthTest( true );
    }

    bool CParticleEmitter::IsDied() const
    {
        return m_bIsDied;
    }

    void CParticleEmitter::Stop()
    {
        m_bStop = true;
    }

};

