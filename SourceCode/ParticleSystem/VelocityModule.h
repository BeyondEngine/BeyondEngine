#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_VELOCITYMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_VELOCITYMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomValue.h"
class CVelocityModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CVelocityModule, 0x71B4005A, CComponentInstance)

public:
    CVelocityModule();
    virtual ~CVelocityModule();

    virtual void ReflectData(CSerializer& serializer) override;
    bool IsLocal() const;
    const SRandomValue& GetX() const;
    const SRandomValue& GetY() const;
    const SRandomValue& GetZ() const;
private:
    bool m_bLocalOrWorld = true;
    SRandomValue m_velocityX;
    SRandomValue m_velocityY;
    SRandomValue m_velocityZ;
};

#endif