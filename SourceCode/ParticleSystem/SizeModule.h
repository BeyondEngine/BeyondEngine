#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_SIZEMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_SIZEMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomValue.h"
class CSizeModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CSizeModule, 0xB107995A, CComponentInstance)

public:
    CSizeModule();
    virtual ~CSizeModule();

    virtual void ReflectData(CSerializer& serializer) override;
    const SRandomValue& GetSize() const;
private:
    SRandomValue m_size;
};

#endif