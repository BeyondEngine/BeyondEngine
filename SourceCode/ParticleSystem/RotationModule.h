#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_ROTATIONMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_ROTATIONMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomValue.h"
class CRotationModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CRotationModule, 0xBB1405A2, CComponentInstance)

public:
    CRotationModule();
    virtual ~CRotationModule();

    virtual void ReflectData(CSerializer& serializer) override;
    const SRandomValue& GetAngularSpeed() const;
private:
    SRandomValue m_angularSpeed = 45;
};

#endif