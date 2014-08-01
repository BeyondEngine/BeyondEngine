#ifndef BEYOND_ENGINE_SKELETALANIMATION_SKELETONANIMATIONCONTROLLER_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_SKELETONANIMATIONCONTROLLER_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

namespace _2DSkeletalAnimation
{
    class CMoveData;
    struct SFrameData;
    class CSkeletonAnimationController : public CComponentInstance
    {
        DECLARE_REFLECT_GUID(CSkeletonAnimationController, 0x849DA7D0, CComponentInstance)
        typedef std::map<std::string, SFrameData> TFrameDataMap;
    public:
        CSkeletonAnimationController();
        virtual ~CSkeletonAnimationController();
        virtual void ReflectData(CSerializer& serializer) override;
        virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
        virtual void Initialize() override;
        virtual void Uninitialize() override;
        void SetMoveData(CMoveData* pMoveData);
        const CMoveData* GetCurrMove() const;
        TFrameDataMap& GetCurrFrameBonesData();
        size_t GetCurrFrame();

        void Play();
        void Update(float fDeltaTime);
        void GoToFrame(size_t frame);
        void Pause();
        void Resume();
        void Stop();
        bool IsPlaying();
        void Restore();

    private:
        void InsertFrameData(SFrameData& result,  const SFrameData& beforeData, const SFrameData& afterData, int iFrame);
        bool IsBlendState();
        void GetMoveFrameData(TFrameDataMap& frameDataMap, CMoveData* pMoveData);
        void GetFrameDataByFrame(TFrameDataMap& frameDataMap, CMoveData* pMoveData, int iFrame);
        void Interpolation(const TFrameDataMap& preFrameDataMap, const TFrameDataMap& currFrameDataMap);

    private:
        CMoveData* m_pPreMove;
        CMoveData* m_pCurrMove;
        TFrameDataMap m_frameDataMap;

        std::vector<std::string> m_boneNames;
        bool m_bLoop;
        size_t m_uLoopCount;
        bool m_bPlaying;
        float m_fPlayingTime;
        float m_fBlendTime;
        float m_fDuration;
        size_t m_uFrameCount;
        size_t m_uFps;
    };
}
#endif//SKELETALANIMATION_SKELETONANIMATIONCONTROLLER_H__INCLUDE
