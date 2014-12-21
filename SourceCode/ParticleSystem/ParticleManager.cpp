#include "stdafx.h"
#include "ParticleManager.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Scene/SceneManager.h"
#include "ShapeModule.h"
#include "BasicModule.h"

CParticleManager* CParticleManager::m_pInstance = nullptr;
CParticleManager::CParticleManager()
{
}

CParticleManager::~CParticleManager()
{
#ifdef DEVELOP_VERSION
    for (auto iter = m_particleDetailMap.begin(); iter != m_particleDetailMap.end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    for (auto iter : m_particleDataPool)
    {
        BEATS_SAFE_DELETE(iter);
    }
#endif
    CleanUp();
}

CParticle* CParticleManager::RequestParticle()
{
    CParticle* pRet = nullptr;
    if (m_particlePool.size() > 0)
    {
        m_particlePoolMutex.lock();
        pRet = m_particlePool.back();
        m_particlePool.pop_back();
        m_particlePoolMutex.unlock();
        pRet->Reset();
    }
    else
    {
        pRet = new CParticle;
        pRet->Initialize();
    }
    return pRet;
}

void CParticleManager::RecycleParticle(CParticle* pParticle)
{
#ifdef _DEBUG
    pParticle->m_bDeleteFlag = true;
#endif
    m_particlePoolMutex.lock();
    m_particlePool.push_back(pParticle);
    m_particlePoolMutex.unlock();
}

void CParticleManager::RegisterEmitterTemplate(CParticleEmitter* pEmitter)
{
    BEATS_ASSERT(pEmitter != nullptr && !pEmitter->GetName().empty());
    BEATS_ASSERT(pEmitter->GetTemplateFlag());
    BEATS_ASSERT(m_emitterTemplateMap.find(pEmitter->GetName()) == m_emitterTemplateMap.end(), "Particle emitter %s has already been registered!\nNew Id:%d Existing Id:%d\n",
        pEmitter->GetName().c_str(), pEmitter->GetId(), m_emitterTemplateMap[pEmitter->GetName()]->GetId());
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(!pEmitter->GetName().empty())
        m_emitterTemplateMap[pEmitter->GetName()] = pEmitter;
    ASSUME_VARIABLE_IN_EDITOR_END
}

void CParticleManager::UnregisterEmitterTemplate(const TString& strName)
{
    BEATS_ASSERT(!strName.empty());
    BEATS_ASSERT(m_emitterTemplateMap.find(strName) != m_emitterTemplateMap.end());
    m_emitterTemplateMap.erase(strName);
    auto emitterPoolIter = m_emitterPool.find(strName);
    if (emitterPoolIter != m_emitterPool.end())
    {
        for (auto subIter = emitterPoolIter->second.begin(); subIter != emitterPoolIter->second.end(); ++subIter)
        {
            (*subIter)->ClearModules();
            BEATS_SAFE_DELETE_COMPONENT((*subIter));
        }
        m_emitterPool.erase(emitterPoolIter);
    }
}

CParticleEmitter* CParticleManager::GetEmitterTemplate(const TString& strName) const
{
    CParticleEmitter* pRet = nullptr;
    if (!strName.empty())
    {
        auto iter = m_emitterTemplateMap.find(strName);
        if (iter != m_emitterTemplateMap.end())
        {
            pRet = iter->second;
        }
    }
    return pRet;
}

CParticleEmitter* CParticleManager::RequestEmitter(const TString& strName)
{
    CParticleEmitter* pRet = nullptr;
    auto emitterPoolIter = m_emitterPool.find(strName);
    if (emitterPoolIter != m_emitterPool.end())
    {
        BEATS_ASSERT(emitterPoolIter->second.size() > 0);
        pRet = emitterPoolIter->second.back();
        pRet->Reset();
        emitterPoolIter->second.pop_back();
        if (emitterPoolIter->second.size() == 0)
        {
            m_emitterPool.erase(emitterPoolIter);
        }
    }
    return pRet;
}

void CParticleManager::RecycleEmitter(CParticleEmitter* pEmitter)
{
    BEATS_ASSERT(pEmitter != nullptr);
    BEATS_ASSERT(!pEmitter->GetName().empty());
    BEATS_ASSERT(pEmitter->GetParentEmitter() == nullptr);
    if (pEmitter->IsPlaying())
    {
        pEmitter->Stop();
    }
    if (pEmitter->GetParentNode() != nullptr)
    {
        pEmitter->GetParentNode()->RemoveChild(pEmitter);
    }
    std::vector<CParticleEmitter*>& emitter = m_emitterPool[pEmitter->GetName()];
    if (std::find(emitter.begin(), emitter.end(), pEmitter) == emitter.end())
    {
        emitter.push_back(pEmitter);
    }
}

CParticleEmitter* CParticleManager::PlayEmitter(const TString& strName)
{
    CParticleEmitter* pRet = RequestEmitter(strName);
    if (pRet == nullptr)
    {
        CParticleEmitter* pEmitterTemplate = GetEmitterTemplate(strName);
        BEATS_ASSERT(pEmitterTemplate != nullptr, "Can't find template emitter: %s", strName.c_str());
        if (pEmitterTemplate != nullptr)
        {
            pRet = down_cast<CParticleEmitter*>(pEmitterTemplate->CloneInstance());
            pRet->SetTemplateFlag(false);
            pRet->Initialize();
        }
    }
    if (pRet)
    {
        BEATS_ASSERT(pRet != nullptr && pRet->GetBasicModule() != nullptr);
        pRet->Play();
    }
    return pRet;
}

bool CParticleManager::AddPlayingEmitter(CParticleEmitter* pEmitter)
{
    bool bRet = std::find(m_playingEmitter.begin(), m_playingEmitter.end(), pEmitter) == m_playingEmitter.end();
    if (bRet)
    {
        m_playingEmitter.push_back(pEmitter);
    }
    return bRet;
}

void CParticleManager::RemovePlayingEmitter(CParticleEmitter* pEmitter)
{
    auto iter = std::find(m_playingEmitter.begin(), m_playingEmitter.end(), pEmitter);
    if (iter!= m_playingEmitter.end())
    {
        m_playingEmitter.erase(iter);
    }
}

const std::vector<CParticleEmitter*>& CParticleManager::GetPlayingEmitter() const
{
    return m_playingEmitter;
}

std::vector<CParticleEmitter*> CParticleManager::GetPlayingEmitter(const TString& strName, CNode* pOwner) const
{
    std::vector<CParticleEmitter*> ret;
    for (size_t i = 0; i < m_playingEmitter.size(); ++i)
    {
        if (strName.empty() || m_playingEmitter[i]->GetName() == strName)
        {
            if (pOwner == nullptr || m_playingEmitter[i]->GetOwner() == pOwner)
            {
                ret.push_back(m_playingEmitter[i]);
            }
        }
    }
    return ret;
}

std::vector<CParticleEmitter*> CParticleManager::StopAllEmitter(const TString& strName, CNode* pOwner /*= nullptr*/)
{
    std::vector<CParticleEmitter*> ret;
    std::vector<CParticleEmitter*> tmp = m_playingEmitter;
    for (size_t i = 0; i < tmp.size(); ++i)
    {
        if (strName.empty() || tmp[i]->GetName() == strName)
        {
            if (pOwner == nullptr || pOwner == tmp[i]->GetOwner())
            {
                tmp[i]->Stop();
                ret.push_back(tmp[i]);
            }
        }
    }
    return ret;
}

void CParticleManager::StopAllEmitter(CNode* pOwner, bool bOnlyStopRenderWithEmitter)
{
    std::vector<CParticleEmitter*> tmp = m_playingEmitter;
    BEATS_ASSERT(pOwner != nullptr);
    for (size_t i = 0; i < tmp.size(); ++i)
    {
        bool bCheck = (tmp[i]->IsRenderWithOwner() || !bOnlyStopRenderWithEmitter);
        if (bCheck && tmp[i]->GetOwner() == pOwner)
        {
            tmp[i]->Stop();
        }
    }
}

void CParticleManager::Update(float dtt)
{
    if (CSceneManager::GetInstance()->GetSwitchSceneState() == false)
    {
        std::vector<CParticleEmitter*> playingEmitter = m_playingEmitter;
        for (size_t i = 0; i < playingEmitter.size(); ++i)
        {
            CParticleEmitter* pCurrEmitter = playingEmitter[i];
#ifdef EDITOR_MODE
            dtt *= pCurrEmitter->m_fPlaySpeed;
#endif
            pCurrEmitter->Update(dtt);
            if (!pCurrEmitter->IsPlaying() && pCurrEmitter->IsAutoRecycle())
            {
                RecycleEmitter(pCurrEmitter);
            }
        }
    }
}

void CParticleManager::Render()
{
    if (CSceneManager::GetInstance()->GetSwitchSceneState() == false)
    {
        for (size_t i = 0; i < m_playingEmitter.size(); ++i)
        {
            CParticleEmitter* pEmitter = m_playingEmitter[i];
            BEATS_ASSERT(pEmitter->GetParentEmitter() == nullptr);
            ASSUME_VARIABLE_IN_EDITOR_BEGIN(pEmitter && pEmitter->GetBasicModule() != nullptr)
            pEmitter->Render();
            ASSUME_VARIABLE_IN_EDITOR_END
        }
    }
#ifdef EDITOR_MODE
    if (m_pCurrSelectEmitter)
    {
        m_pCurrSelectEmitter->RenderShape();
    }
#endif
}

void CParticleManager::CleanUp()
{
    StopAllEmitter("");
    for (auto iter = m_emitterPool.begin(); iter != m_emitterPool.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            CParticleEmitter* pEmitter = *subIter;
            BEATS_ASSERT(!pEmitter->IsPlaying());
            BEATS_SAFE_DELETE_COMPONENT(pEmitter);
        }
    }
    m_emitterPool.clear();
    m_particlePoolMutex.lock();
    for (size_t i = 0; i < m_particlePool.size(); ++i)
    {
        BEATS_SAFE_DELETE_COMPONENT(m_particlePool[i]);
    }
    m_particlePool.clear();
    m_particlePoolMutex.unlock();
}

uint32_t CParticleManager::GetParticleRandValue()
{
    uint32_t uRet = (((m_uCurrentRandomSeed = m_uCurrentRandomSeed * 214013L
        + 2531011L) >> 16) & 0x7fff);
    return uRet;
}

float CParticleManager::GetParticleRandValueByRange(float fMin, float fMax)
{
    uint32_t uRandValue = GetParticleRandValue();
    BEATS_ASSERT(uRandValue <= 0x7ffff);
    return (((float)uRandValue / 0x7fff) * (fMax - fMin) + fMin);
}

void CParticleManager::SetCurrentRandSeed(uint32_t uSeed)
{
    m_uCurrentRandomSeed = uSeed;
}

