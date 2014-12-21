#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_PARTICLEMANAGER_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_PARTICLEMANAGER_H__INCLUDE

class CParticle;
class CParticleEmitter;
class CNode;
#ifdef DEVELOP_VERSION
struct SParticleDetail
{
    uint32_t emitterUpdateCount;
    uint32_t particleUpdateCount;
    uint32_t RenderCount;
};
#endif
class CParticleManager
{
    BEATS_DECLARE_SINGLETON(CParticleManager)
public:
    CParticle* RequestParticle();
    void RecycleParticle(CParticle* pParticle);

    void RegisterEmitterTemplate(CParticleEmitter* pEmitter);
    void UnregisterEmitterTemplate(const TString& strName);
    CParticleEmitter* GetEmitterTemplate(const TString& strName) const;
    CParticleEmitter* RequestEmitter(const TString& strName);
    void RecycleEmitter(CParticleEmitter* pEmitter);

    CParticleEmitter* PlayEmitter(const TString& strName);
    bool AddPlayingEmitter(CParticleEmitter* pEmitter);
    void RemovePlayingEmitter(CParticleEmitter* pEmitter);
    const std::vector<CParticleEmitter*>& GetPlayingEmitter() const;
    std::vector<CParticleEmitter*> GetPlayingEmitter(const TString& strName, CNode* pOwner) const;
    std::vector<CParticleEmitter*> StopAllEmitter(const TString& strName, CNode* pOwner = nullptr);
    void StopAllEmitter(CNode* pOwner, bool bOnlyStopRenderWithEmitter);
    void Update(float dtt);
    void Render();

    void CleanUp();
    uint32_t GetParticleRandValue();
    float GetParticleRandValueByRange(float fMin, float fMax);
    void SetCurrentRandSeed(uint32_t uSeed);
#ifdef DEVELOP_VERSION
    bool m_bRenderSwitcher = true;
    bool m_bUpdateSwitcher = true;
    bool m_bRenderParticleEdge = false;
    CParticleEmitter* m_pCurrSelectEmitter = nullptr;
#endif
#ifdef DEVELOP_VERSION
    std::map<TString, SParticleDetail*> m_particleDetailMap;
    std::vector<SParticleDetail*> m_particleDataPool;
    std::set<CParticleEmitter*> m_emitterRegisterSet;
    std::set<CParticle*> m_particleRegisterSet;
    uint32_t m_uParticleRenderedCount = 0;
    uint32_t m_uParticleUpdatedCount = 0;
    uint32_t m_uRenderCountLastFrame = 0;
    uint32_t m_uUpdateCountLastFrame = 0;
#endif
private:
    uint32_t m_uCurrentRandomSeed = 0;
    std::mutex m_particlePoolMutex;
    std::vector<CParticle*> m_particlePool;
    std::map<TString, CParticleEmitter*> m_emitterTemplateMap;
    std::vector<CParticleEmitter*> m_playingEmitter;
    std::map<TString, std::vector<CParticleEmitter*> > m_emitterPool;
};

#endif