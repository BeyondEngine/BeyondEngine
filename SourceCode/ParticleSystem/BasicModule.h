#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_BASICMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_BASICMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomValue.h"
#include "RandomColor.h"

class CBasicModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CBasicModule, 0xFE441B03, CComponentInstance)

public:
    CBasicModule();
    virtual ~CBasicModule();

    virtual void ReflectData(CSerializer& serializer) override;
    bool IsParticleAttach() const;
    bool IsLoop() const;
    bool IsPlayOnAwake() const;
    bool IsStopWhenDeactive() const;
    void SetDepthTest(bool bEnable);
    bool IsDepthTestEnable() const;
    bool Is3DEmitter() const;
    void Set3DEmitterFlag(bool b3DOr2D);
    float GetDuration() const;
    float GetStartDelay() const;
    float GetGravityModifier() const;
    uint32_t GetMaxParticleCount() const;
    const SRandomValue& GetInitLifeTime() const;
    const SRandomValue& GetInitSpeed() const;
    const SRandomValue& GetInitSize() const;
    const SRandomValue& GetInitRotation() const;
    const CRandomColor& GetInitColor() const;

private:
    float m_fDuration = 5.0f;
    bool m_bLoop = true;
    bool m_bPrewarm = false;
    bool m_b3D = true;
    bool m_bEnableDepth = false;
    bool m_bStopWhenDeactive = false;
    float m_fStartDelay = 0;
    float m_fGravityModifier = 0;
    SRandomValue m_startLifeTime = 5.0f;
    SRandomValue m_startSpeed = 5.0f;
    SRandomValue m_startSize = 1.0f;
    SRandomValue m_startRotation = 0;
    CRandomColor m_startColor;
    bool m_bAttachToEmitter = true;
    bool m_bPlayOnAwake = true;
    uint32_t m_uMaxParticle = 1000;
};

#endif