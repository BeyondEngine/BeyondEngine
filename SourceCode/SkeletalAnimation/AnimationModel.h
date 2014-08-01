#ifndef BEYOND_ENGINE_SKELETALANIMATION_ANIMATIONMODEL_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_ANIMATIONMODEL_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

namespace _2DSkeletalAnimation
{
    class CSkeletonAnimationController;
    class CDisplayData;
    class CArmatureData;
    class CAnimationData;
    class CAnimationModel : public CComponentInstance
    {
        DECLARE_REFLECT_GUID(CAnimationModel, 0xB4ADE69A, CComponentInstance)
    public:
        CAnimationModel();
        virtual ~CAnimationModel();

        virtual void ReflectData(CSerializer& serializer) override;
        virtual bool OnDependencyChange(void* pVariableAddr, CComponentBase* pComponent) override;
        virtual void Initialize() override;
        virtual void Uninitialize() override;
        void ReLoad();
        void CreateModel();
        void Update();
        void CheckBoneChange();
        void ReplaceDisplayData();
    private:
        void DestroyModel();
    private:
        bool m_bCreateModel;
        CArmatureData* m_pArmatureData;
        CAnimationData* m_pAnimationData;
    };
}

#endif//SKELETALANIMATION_CARMATURE_H__INCLUDE
