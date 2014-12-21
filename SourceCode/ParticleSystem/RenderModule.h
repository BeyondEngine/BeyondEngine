#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_RENDERMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_RENDERMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "Render/ReflectTextureInfo.h"
#include "Render/ShaderUniform.h"

enum class EParticleRenderMode
{
    ePRM_Billboard,
    ePRM_StretchedBillboard,
    ePRM_HorizontalBillboard,
    ePRM_VerticalBillboard,
    ePRM_Mesh,

    ePRM_Count
};

enum class EParticleSortMode
{
    ePSM_None,
    ePSM_ByDistance,
    ePSM_YoungestFirst,
    ePSM_OldestFirst,

    ePSM_Count
};
class CMaterial;
class CRenderBatch;
class CRenderModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CRenderModule, 0xB614005E, CComponentInstance)

public:
    CRenderModule();
    virtual ~CRenderModule();
    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool Load() override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet) override;
#endif
    EParticleRenderMode GetRenderMode() const;
    EParticleSortMode GetSortMode() const;
    SharePtr<CMaterial> GetMaterial(bool bEnableDepthTest) const;
    const CVec4& GetTextureOffset() const;
    float GetLengthScale() const;
    float GetSpeedScale() const;
    CRenderBatch* RequestRenderBatch(bool bEnableDepthTest, ERenderGroupID renderGroupId);
    SharePtr<CTextureFrag> GetTextureFrag();
private:
    bool m_bBlendOrAdd = true;
    EParticleRenderMode m_mode = EParticleRenderMode::ePRM_Billboard;
    float m_fSpeedScale = 0;
    float m_fLengthScale = 2.0f;
    CColor m_tintColor = 0xFFFFFFFF;
    CVec4 m_textureOffset;
    EParticleSortMode m_sortMode = EParticleSortMode::ePSM_None;
    SReflectTextureInfo m_texture;
    std::map<unsigned char, SharePtr<CTexture> > m_renderTextureMap;
    std::map<TString, CShaderUniform> m_uniformMap;
};

#endif