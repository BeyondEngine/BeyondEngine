#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_EMISSIONMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_EMISSIONMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"

class CParticle;
class CParticleEmitter;
class CEmissionModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CEmissionModule, 0xB51AD061, CComponentInstance)

public:
    CEmissionModule();
    virtual ~CEmissionModule();
    virtual void ReflectData(CSerializer& serializer) override;
    void Update(CParticleEmitter* pEmitter, std::vector<CParticle*>& emitParticle, uint32_t uMaxCount);

private:
    float m_fRate = 10.f;
    std::map<float, uint32_t> m_extraBurst; // Key is the time, value is the extra particle count to burst.
};

#endif