#ifndef BEYOND_ENGINE_SKELETALANIMATION_SKELETONANIMATIONMANAGER_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_SKELETONANIMATIONMANAGER_H__INCLUDE

#include "AnimationData.h"
namespace _2DSkeletalAnimation
{
    class CArmatureData;
    class CTextureData;
    class CDisplayData;
    class CSkeletonAnimationController;
    class CAnimationModel;
    class  CSkeletonAnimationManager
    {
        BEATS_DECLARE_SINGLETON(CSkeletonAnimationManager)
    public:
        void AddAnimationDataFromFile(const TString &file);
        CAnimationData* GetAnimationData(const std::string& name) const;
        CArmatureData* GetArmatureData(const std::string& name) const;
        void AddAnimationData(CAnimationData* pAnimationData);
        void AddArmatureData(CArmatureData* pArmatureData);
        std::vector<CTextureData*>& GetTextureData();
        void ReplaceDisplayData(const std::string& boneName, const CDisplayData& displayData);
        const std::string GetLastAnimationName() const;
        const std::string GetLastArmatureName() const;
        void Update(float fDeltaTime);
        void RegisterController(CSkeletonAnimationController* pController);
        void UnRegisterController(CSkeletonAnimationController* pController);
        void RegisterAnimationModel(CAnimationModel* pAniModel);
        void UnregisterAnimationModel(CAnimationModel* pAniModel);
    private:
        std::vector<CTextureData*> m_textureList;
        std::map<std::string, CAnimationData*> m_animationDataMap;
        std::map<std::string, CArmatureData*> m_armatureDataMap;

        std::list<CSkeletonAnimationController*> m_controllerList;
        std::list<CAnimationModel*> m_animationModelList;
        std::map<std::string, std::string> m_animationNameMap;

        std::string m_strLastAnimationName;
        std::string m_strLastArmatureName;
    };
}

#endif//SKELETALANIMATION_CANIMATIONMANAGER_H__INCLUDE