#ifndef BEYOND_ENGINE_RENDER_MODEL_H__INCLUDE
#define BEYOND_ENGINE_RENDER_MODEL_H__INCLUDE

#include "Scene/Node3D.h"
#include "Component/ComponentPublic.h"
#include "ModelData.h"

class CSkeleton;
class CSkin;
class CTexture;
class CAnimationController;
class CSkeletonAnimation;
class CShaderProgram;
class CMaterial;
class CShaderUniform;
class CRenderBatch;
class CActionBase;

class CModel : public CNode3D
{
    DECLARE_REFLECT_GUID(CModel, 0x1547DBEC, CNode3D)

public:
    CModel();
    virtual ~CModel();

    virtual bool Load() override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual void ReflectData(CSerializer& serializer) override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    virtual void Update(float dtt) override;
    void PlayAnimationByName(const TString& strName, float fBlendTime, bool bLoop);
    CAnimationController* GetAnimationController() const;
    void SetCurrAnimation(const TString& strFileName);

    virtual void DoRender() override;

    void SetAnimationFilePathList(std::vector<SReflectFilePath>& strAnimationFileList);
    void RefreshBySkin();
    const std::map<TString, CSkeletonAnimation* > GetAnimations() const;
    const TString& GetCurrentAnimationName() const;

    CSkeleton* GetSkeleton() const;

    SharePtr<CModelData> GetModelData() const;
    void LoadFile(const TString& strFile);
#ifdef EDITOR_MODE
    void SendDataToGraphics();
#endif

private:
#ifdef EDITOR_MODE
    void RenderSkeleton();
#endif
    void LoadModelData();

private:
    CAnimationController* m_pAnimationController;
    SharePtr<CModelData> m_pModelData;
    std::vector<CRenderBatch*> m_renderBatchList;

    TString m_strCurrAnimationName;
#ifdef EDITOR_MODE
    std::vector<TString> m_animationNameList; //This member used to hold all string in m_animationNamePathList, so we can use it as a combo property of m_strSelAnimtaionName
#endif
    bool m_bFrustumTest = false;
    bool m_bPlayAnimation;
    float m_fPlaySpeed = 1.0f;
    SReflectFilePath m_modelFile;
};

#endif