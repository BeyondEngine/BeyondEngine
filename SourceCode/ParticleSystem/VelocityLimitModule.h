#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_VELOCITYLIMITMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_VELOCITYLIMITMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomValue.h"
class CVelocityLimitModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CVelocityLimitModule, 0x7440ECA9, CComponentInstance)

public:
    CVelocityLimitModule();
    virtual ~CVelocityLimitModule();

    virtual void ReflectData(CSerializer& serializer) override;
    bool IsLocal() const;
    bool IsSeparateAxis() const;
    float GetDampen() const;
    const SRandomValue& GetSpeed() const;
    const SRandomValue& GetX() const;
    const SRandomValue& GetY() const;
    const SRandomValue& GetZ() const;

private:
    bool m_bLocalOrWorld = true;
    bool m_bSeparateAxis = false;
    SRandomValue m_velocityX = 1.0f;
    SRandomValue m_velocityY = 1.0f;
    SRandomValue m_velocityZ = 1.0f;
    SRandomValue m_speed = 1.0f;
    float m_fDampen = 1.0f;
};

#endif