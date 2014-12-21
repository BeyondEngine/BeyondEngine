#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_COLORMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_COLORMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomColor.h"
class CColorModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CColorModule, 0xDD41B098, CComponentInstance)

public:
    CColorModule();
    virtual ~CColorModule();

    virtual void ReflectData(CSerializer& serializer) override;
    CColor GetColor(float fProgress) const;

private:
    CRandomColor m_color;
};

#endif