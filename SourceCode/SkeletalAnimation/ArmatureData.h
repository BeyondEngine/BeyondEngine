#ifndef BEYOND_ENGINE_SKELETALANIMATION_ARMATUREDATA_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_ARMATUREDATA_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

namespace _2DSkeletalAnimation
{
    class CBoneData;
    class CDisplayData;
    class CArmatureData : public CComponentInstance
    {
        DECLARE_REFLECT_GUID(CArmatureData, 0x68DD5AAF, CComponentInstance)
        typedef std::map<std::string, CBoneData*> BonesDataMap;
    public:
        CArmatureData();
        virtual ~CArmatureData();
        virtual void ReflectData(CSerializer& serializer) override;
        virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
        virtual void Initialize() override;
        void AddBoneData(CBoneData *boneData);
        const CBoneData *GetBoneData(const std::string &boneName) const;
        const std::string& GetName() const;
        void SetName(const std::string& name);
        const BonesDataMap& GetBonesMap() const;
        const std::vector<CBoneData*>& GetBoneOrderList();
        const std::vector<std::string>& GetBoneOrderNames();
        CDisplayData ReplaceDisplayData();
        bool CheckReplaceData();
        const std::string GetReplaceDisplayName() const;
        void LoadData();
        void ReLoadData();
    private:
        std::string m_strName;
        BonesDataMap m_boneDataMap;
        std::vector<CBoneData*> m_boneDataOrderList;

        SReflectFilePath m_strCfgFilePath;
        TString m_strBoneName;
        std::vector<TString> m_boneNameList;
        std::vector<std::string> m_boneNames;
        CDisplayData* m_pDisplayData;

        std::string m_strAnimationName;
        std::string m_strArmatureName;
    };
}

#endif//SKELETALANIMATION_CARMATUREDATA_H__INCLUDE
