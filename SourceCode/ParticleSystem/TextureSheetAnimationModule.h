#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_TEXTURESHEETANIMATIONMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_TEXTURESHEETANIMATIONMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "RandomValue.h"
class CTextureSheetAnimationModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CTextureSheetAnimationModule, 0xF1704BA8, CComponentInstance)

public:
    CTextureSheetAnimationModule();
    virtual ~CTextureSheetAnimationModule();

    virtual void ReflectData(CSerializer& serializer) override;
    uint32_t GetRow() const;
    uint32_t GetCol() const;
    bool IsWholeSheetOrSingleRow() const;
    bool IsRandomRow() const;
    uint32_t GetSpecificRow() const;
    const SRandomValue& GetFrame() const;
    uint32_t GetLoopTimes() const;
private:
    uint32_t m_uRow = 1;
    uint32_t m_uCol = 1;
    bool m_bWholeSheetOrSingleRow = true;
    bool m_bRandomRow = true;
    uint32_t m_uRowIndex = 0;
    SRandomValue m_frameOverTime = 0;
    uint32_t m_uCycles = 1;
};

#endif