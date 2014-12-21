#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_NEWPARTICLE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_NEWPARTICLE_H__INCLUDE

#include "Scene/Node3D.h"
#include "ParticleManager.h"
class CParticleEmitter;
class CTexture;

#define PARTICLE_RAND_RANGE(minValue, maxValue) CParticleManager::GetInstance()->GetParticleRandValueByRange(minValue, maxValue)

class CParticle : public CNode3D
{
public:
    CParticle();
    virtual ~CParticle();

    void SetEmitter(CParticleEmitter* pEmitter);
    void Reset();

    virtual void Update(float dtt) override;
    virtual void Render() override;
    void InitByEmitter();
#ifdef _DEBUG
    bool m_bDeleteFlag = false;
#endif
private:
    CVec3 LimitVelocity(const CVec3& curVelocity, const CQuaternion& quaterionToLocal);
    float CalculateValueByDampen(float fOriginalValue, float fLimitValue, uint32_t uElapsedTimeMS, float fDampen);
private:
    float m_fLifeTime = 0;
    float m_fElapsedLifeTime = 0;
    float m_fEmitterScale = 1.0f;
    uint32_t m_uRandomSeed = 0;
    CVec3 m_initVelocity;
    CVec3 m_currVelocity;
    CColor m_initColor;
    float m_fInitSize = 1.0f;
    float m_fParticleRotationInRadians = 0;
    CParticleEmitter* m_pEmitter = nullptr;
    CVertexPTC m_vertices[4];
};

#endif