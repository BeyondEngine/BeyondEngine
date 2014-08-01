#ifndef BEYOND_ENGINE_RENDER_MODEL_H__INCLUDE
#define BEYOND_ENGINE_RENDER_MODEL_H__INCLUDE

#include "Scene/Node3D.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"

class CSkeleton;
class CSkin;
class CTexture;
class CAnimationController;
class CAnimation3D;
class CShaderProgram;
class CMaterial;
class CModel : public CNode3D
{
    DECLARE_REFLECT_GUID(CModel, 0x1547DBEC, CNode3D)

public:
    CModel();
    virtual ~CModel();

    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
    virtual void Update(float dtt) override;
    void PlayAnimationById(long id, float fBlendTime, bool bLoop);
    void PlayAnimationByName(const char *name, float fBlendTime, bool bLoop);
    CAnimationController* GetAnimationController();

    virtual void PreRender() override;
    virtual void DoRender() override;

    void SetAnimaton(SharePtr<CAnimation3D> pAnimation);
    void SetSkeleton(SharePtr<CSkeleton> pSkeleton);
    SharePtr<CSkeleton> GetSkeleton() const;
    void SetSkin(SharePtr<CSkin> pSkin);

private:
    void RenderSkeleton();

private:
    bool m_bRenderSkeleton;
    CShaderProgram* m_pSkinProgram;
    CAnimationController* m_pAnimationController;
    SharePtr<CMaterial> m_pMaterial;
    SharePtr<CSkeleton> m_pSkeleton;
    SharePtr<CSkin> m_pSkin;
    std::vector<SharePtr<CTexture> > m_textures;
    std::map<long, SharePtr<CAnimation3D> > m_animations;

    TString m_strSelAnimtaionName;
    std::vector<SReflectFilePath> m_animationNamePathList;
    std::vector<TString> m_animationNameList;
    bool m_bPlayAnimation;

    SReflectFilePath m_strSkeleton;
    SReflectFilePath m_strSkin;
};

#endif