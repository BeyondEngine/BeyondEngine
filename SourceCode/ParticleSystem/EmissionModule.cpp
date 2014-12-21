#include "stdafx.h"
#include "EmissionModule.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "Particle.h"
#include "BasicModule.h"

CEmissionModule::CEmissionModule()
{

}

CEmissionModule::~CEmissionModule()
{

}

void CEmissionModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_fRate, true, 0xFFFFFFFF, "发射频率(每秒)", nullptr, "每秒发射多少个粒子", "MinValue:0");
    DECLARE_PROPERTY(serializer, m_extraBurst, true, 0xFFFFFFFF, "爆发时间点(秒)", nullptr, "在指定的时间点，一次性发射额外的粒子数量", "MinValue:0");
}

void CEmissionModule::Update(CParticleEmitter* pEmitter, std::vector<CParticle*>& emitParticle, uint32_t uMaxCount)
{
    emitParticle.clear();
    float fPlayingTime = pEmitter->GetPlayingTime(false);
    if (m_fRate > 0)
    {
        float fShootIntervalPerSecond = 1.0f / m_fRate;
        float fElapsedTime = fPlayingTime - pEmitter->GetLastEmitTime();
        while (fElapsedTime > fShootIntervalPerSecond && emitParticle.size() < uMaxCount)
        {
            fElapsedTime -= fShootIntervalPerSecond;
            CParticle* pParticle = CParticleManager::GetInstance()->RequestParticle();
            pParticle->SetEmitter(pEmitter);
            emitParticle.push_back(pParticle);
            pEmitter->SetLastEmitTime(fPlayingTime);
        }
    }
    if (m_extraBurst.size() > 0 && emitParticle.size() < uMaxCount)
    {
        float fPlayingTimeInDuration = BEATS_FMOD(fPlayingTime, pEmitter->GetBasicModule()->GetDuration());
        // Check the extra burst time from the last burst time.
        auto iter = m_extraBurst.begin();
        if (BEATS_FLOAT_GREATER_EQUAL(pEmitter->GetLastBurstTime(), 0))
        {
            iter = m_extraBurst.find(pEmitter->GetLastBurstTime());
            ++iter;
        }
        for (; iter != m_extraBurst.end(); ++iter)
        {
            float fBurstTime = iter->first;
            if (BEATS_FLOAT_GREATER_EQUAL(fPlayingTimeInDuration, fBurstTime))
            {
                uint32_t uBurstCount = MIN(iter->second, uMaxCount - emitParticle.size());
                for (size_t i = 0; i < uBurstCount; ++i)
                {
                    CParticle* pParticle = CParticleManager::GetInstance()->RequestParticle();
                    pParticle->SetEmitter(pEmitter);
                    emitParticle.push_back(pParticle);
                }
                pEmitter->SetLastBurstTime(fBurstTime);
            }
            else
            {
                break;
            }
        }
    }
}