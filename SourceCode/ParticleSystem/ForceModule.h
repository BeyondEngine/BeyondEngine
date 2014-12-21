#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_FORCEMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_FORCEMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomValue.h"

class CForceModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CForceModule, 0xB41D109A, CComponentInstance)

public:
    CForceModule();
    virtual ~CForceModule();

    virtual void ReflectData(CSerializer& serializer) override;
    bool IsLocal() const;
    bool IsRandomizeEveryFrame() const;
    const SRandomValue& GetX() const;
    const SRandomValue& GetY() const;
    const SRandomValue& GetZ() const;

private:
    SRandomValue m_forceX;
    SRandomValue m_forceY;
    SRandomValue m_forceZ;
    bool m_bLocalOrWorld = true;
    bool m_bRandomize = false;
};

#endif