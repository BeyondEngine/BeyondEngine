#include "stdafx.h"
#include "ParticleSystem.h"
#include "Emitter.h"

namespace NBeyondEngine
{
    CParticleSystem::CParticleSystem()
        : m_bLoop(true)
        , m_fLiveTime(-1.0f)
        , m_fCurrentTime(0.0f)
    {
    }

    CParticleSystem::~CParticleSystem()
    {
    }

    void CParticleSystem::ReflectData(CSerializer& serializer)
    {
        super::ReflectData(serializer);
        DECLARE_PROPERTY( serializer, m_fLiveTime, true, 0xFFFFFFFF, _T("生存时间"), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_bLoop, true, 0xFFFFFFFF, _T("是否循环"), NULL, NULL, _T("VisibleWhen : m_fLiveTime == -1.0") );
        DECLARE_DEPENDENCY_LIST( serializer, m_emitterVector, _T("粒子发射器"), eDT_Strong );
    }

    void CParticleSystem::Update( float dtt)
    {
        super::Update( dtt);
        if ( m_fLiveTime < 0.0f )//aways live
        {
            if ( m_bLoop )//loop
            {
                bool bIsAllDide = true;
                for ( auto i : m_emitterVector )
                {
                    bIsAllDide &= i->IsDied();
                }
                if ( bIsAllDide )
                {
                    m_fCurrentTime = 0.0f;
                    BeginShoot();
                }
            }
        }
        else
        {
            m_fCurrentTime += dtt;
            if ( m_fCurrentTime > m_fLiveTime )
            {
                m_fCurrentTime = m_fLiveTime;
                for ( auto i : m_emitterVector )
                {
                    i->Stop();
                }
            }
        }

        for ( auto i : m_emitterVector )
        {
            i->Update( dtt );
        }
    }


    void CParticleSystem::DoRender()
    {
        for ( auto i : m_emitterVector )
        {
            i->Visit( &GetLocalTM() );
        }
    }

    bool CParticleSystem::OnDependencyListChange( void* pComponentAddr, EDependencyChangeAction actionType, CComponentBase* pComponent)
    {
        bool bRet = super::OnDependencyListChange( pComponentAddr, actionType, pComponent );
        if ( !bRet )
        {
            if ( &m_emitterVector == pComponentAddr )
            {
                BEATS_ASSERT(dynamic_cast<CParticleEmitter*>(pComponent) != NULL);
                CParticleEmitter* pEmitter = (CParticleEmitter*)pComponent;
                if (actionType == eDCA_Add)
                {
                    AddEmitter( pEmitter );
                }
                else if ( actionType == eDCA_Change || actionType == eDCA_Delete )
                {
                    RemoveEmitter( pEmitter );
                }
                bRet = true;
            }
        }
        return bRet;
    }

    bool CParticleSystem::AddEmitter( CParticleEmitter* pEmitter )
    {
        bool bRet = true;
        for ( auto iter : m_emitterVector )
        {
            if ( iter == pEmitter )
            {
                bRet = false;
                break;
            }
        }
        if ( bRet )
        {
            m_emitterVector.push_back( pEmitter );
            pEmitter->BeginShoot();
        }
        return bRet;
    }

    void CParticleSystem::RemoveEmitter( CParticleEmitter* pEmitter )
    {
        auto iter = m_emitterVector.begin();
        for ( ; iter != m_emitterVector.end(); ++iter )
        {
            if ( *iter == pEmitter )
            {
                m_emitterVector.erase( iter );
                break;
            }
        }
    }

    void CParticleSystem::BeginShoot()
    {
        for ( auto iter : m_emitterVector )
        {
            iter->BeginShoot();
        }
    }

    void CParticleSystem::Initialize()
    {
        super::Initialize();
        BeginShoot();
    }

};
