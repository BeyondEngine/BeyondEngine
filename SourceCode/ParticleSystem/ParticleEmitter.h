#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_NEWPARTICLEEMITTER_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_NEWPARTICLEEMITTER_H__INCLUDE

#include "Scene/Node3D.h"

class CParticle;
class CShapeModule;
class CEmissionModule;
class CColorModule;
class CForceModule;
class CRenderModule;
class CRotationModule;
class CSizeModule;
class CTextureSheetAnimationModule;
class CVelocityModule;
class CVelocityLimitModule;
class CBasicModule;
class CScene;
class CParticleEmitter : public CNode3D
{
    DECLARE_REFLECT_GUID(CParticleEmitter, 0x91BB0A41, CNode3D)
public:
    CParticleEmitter();
    virtual ~CParticleEmitter();
    virtual void ReflectData(CSerializer& serializer) override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual void Update(float dtt) override;
    virtual void Render() override;
    virtual CComponentInstance* CloneInstance() const override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    virtual void Deactivate(bool bApplyToChild = true) override;
    CNode* GetOwner() const;
    void SetOwner(CNode* pOwner);
    CScene* GetAttachScene() const;
    void SetAttachScene(CScene* pScene);
    void Play(bool bForceReplay = false);
    void Pause();
    void Stop();
    bool IsPlaying() const;
    bool IsPaused() const;
    void SetPlayingFlag(bool bFlag);
    float GetPlayingTime(bool bContainDelay) const;
    void SetTemplateFlag(bool bFlag);
    bool GetTemplateFlag() const;
    void SetDepthTestEnable(bool bEnable);
    void SetRemoveFromParentWhenStop(bool bFlag);
    void Reset();
    CBasicModule* GetBasicModule();
    CEmissionModule* GetEmissionModule();
    CShapeModule* GetShapeModule();
    CVelocityModule* GetVelocityModule();
    CVelocityLimitModule* GetVelocityLimitModule();
    CForceModule* GetForceModule();
    CRenderModule* GetRenderModule();
    CSizeModule* GetSizeModule();
    CRotationModule* GetRotationModule();
    CColorModule* GetColorModule();
    CTextureSheetAnimationModule* GetTextureSheetAnimationModule();
    CParticleEmitter* GetParentEmitter() const;

    void RemoveParticle(CParticle* pParticle, bool bDelete);
    void AddParticle(CParticle* pParticle);

    uint32_t RequestRandomSeed();
    void ResetRandomSeed();

    float GetLastEmitTime() const;
    void SetLastEmitTime(float fTime);
    float GetLastBurstTime() const;
    void SetLastBurstTime(float fTime);
    const std::vector<CParticleEmitter*>& GetChildrenEmitter() const;
    void CopyModulesToEmitter(CParticleEmitter* pEmitter) const;
    void ClearModules();
    bool IsAutoRecycle() const;
    void SetAutoRecycle(bool bAutoRecycle);
    bool IsRenderWithOwner() const;
    void SetRenderWithOwner(bool bRenderWithOwner);
    void SetRenderWithOwnerFlag(bool bOwnerRenderFlag);
    void SetParticleScale(float fScale);
    void SetParticleScaleFactor(float fScaleFactor);
    float GetParticleScale(bool bInherit = true);
    void RefreshAllParticle();
    void SetPathFindingGridIndex(int32_t nIndex);
#ifdef EDITOR_MODE
    float m_fPlaySpeed = 1.0f;
    bool m_bRenderShape = false;
    void ParseRandomValue(CPropertyDescriptionBase* pProperty, rapidxml::xml_node<>* pNode, bool bConvertRadianToDegree = false);
    void ParseColorSplineValue(CPropertyDescriptionBase* pProperty, rapidxml::xml_node<>* pNode);
    virtual bool OnPropertyImport(const TString& strPropertyName, const TString& strFile);
    void ImportFromUnityXML(rapidxml::xml_node<>* pNode);
    void RenderShape();
#endif
#ifdef DEVELOP_VERSION
    uint32_t m_uLastUpdateFrameCount = 0;
    uint32_t m_uLastRenderFrameCount = 0;
    bool m_bModuleIsCopy = false;
#endif
private:
    bool IsFinished();
    void InitChildEmitter();
    void Set3DRender(bool b3DRender);
private:
    bool m_bDelayPlay = false;
    bool m_bRenderWithOwner = false;
    bool m_bOwnerRenderFlag = false;
    bool m_bPlaying = false;
    bool m_bPaused = false;
    bool m_bTemplate = false;
    bool m_bAutoRecycle = false;
    bool m_bRemoveParentWhenStop = false;
    float m_fPlayingTime = 0;
    float m_fLastEmitTime = 0;
    float m_fLastBurstTime = -1.0f;
    float m_fParticleScale = 1.0f;
    float m_fParticleScaleFactor = 1.0f;
    uint32_t m_uRandomSeed = 0;
    uint32_t m_uCurrSeedForParticle = 0;
    int32_t m_nPathFindingGridIndex = -1;
    CNode* m_pOwner = nullptr;
    CScene* m_pAttachScene = nullptr;
    CBasicModule* m_pBasicModule = nullptr;
    CShapeModule* m_pShapeModule = nullptr;
    CEmissionModule* m_pEmissionModule = nullptr;
    CColorModule* m_pColorModule = nullptr;
    CForceModule* m_pForceModule = nullptr;
    CRenderModule* m_pRenderModule = nullptr;
    CRotationModule* m_pRotationModule = nullptr;
    CSizeModule* m_pSizeModule = nullptr;
    CTextureSheetAnimationModule* m_pTextureSheetAnimationModule = nullptr;
    CVelocityModule* m_pVelocityModule = nullptr;
    CVelocityLimitModule* m_pVelocityLimitModule = nullptr;
    CParticleEmitter* m_pParentEmitter = nullptr;
    std::set<CParticle*> m_particleList;
    std::vector<CParticleEmitter*> m_childrenEmitter;
};

#endif