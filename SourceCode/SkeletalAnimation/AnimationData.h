#ifndef BEYOND_ENGINE_SKELETALANIMATION_ANIMATIONDATA_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_ANIMATIONDATA_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

namespace _2DSkeletalAnimation
{
    class CMoveData;
    struct SFrameData;
    class CSkeletonAnimationController;
    class  CAnimationData : public CComponentInstance
    {
        DECLARE_REFLECT_GUID(CAnimationData, 0x09238D7C, CComponentInstance)
        typedef std::map<std::string, CMoveData*> MoveDataMap;
        typedef std::map<std::string, SFrameData> TFrameDataMap;
    public:
        CAnimationData();
        virtual ~CAnimationData();
        virtual void ReflectData(CSerializer& serializer) override;
        virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
        virtual bool OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent) override;
        virtual void Initialize() override;
        void AddMoveData(CMoveData *moveData);
        CMoveData *GetMovData(const std::string& moveName);
        void SetName(const std::string name);
        const std::string& GetName() const;
        const std::vector<std::string>& GetMoveNameList();
        std::map<std::string, SFrameData>& GetInitBonesData();
        std::map<std::string, SFrameData>& GetCurrFrameBonesData() const;
        CSkeletonAnimationController* GetAnimationController() const;
        void LoadData();
        void ReLoadData();
    private:
        void InsertFrameData(SFrameData& result,  const SFrameData& beforeData, const SFrameData& afterData, int iFrame);
        std::string m_strName;
        MoveDataMap m_movedataMap;
        std::vector<std::string> m_moveNameList;

        SReflectFilePath m_strCfgFilePath;
        TString m_strMoveName;
        std::vector<TString> m_moveNames;
        CSkeletonAnimationController* m_pAnimationController;
        TFrameDataMap m_frameDataMap;
    };
}

#endif//SKELETALANIMATION_CANIMATIONDATA_H__INCLUDE