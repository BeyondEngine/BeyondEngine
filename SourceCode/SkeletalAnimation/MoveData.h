#ifndef BEYOND_ENGINE_SKELETALANIMATION_MOVEDATA_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_MOVEDATA_H__INCLUDE
namespace _2DSkeletalAnimation
{
    struct SFrameData
    {
        int dI;
        float x;
        float y;
        int zOrder;
        float skewX;
        float skewY;
        float scaleX;
        float scaleY;
        int frameID;
        int tweenType;
        bool isTween;
    };

    struct SMoveBoneData
    {
        float delay;
        float scale;
        float duration;
        std::string name;
        std::vector<SFrameData*> frameList;
    };

    class CMoveData
    {
        typedef std::map<std::string, SMoveBoneData*> TMoveBoneDataMap;
    public:
        CMoveData();
        ~CMoveData();
        void AddMoveBoneData(SMoveBoneData *boneData);
        SMoveBoneData *GetMovBoneData(const std::string boneName);
        const TMoveBoneDataMap& GetMoveBoneDataMap() const;
        const std::string& GetName() const;
        void SetName(const std::string& name);
        int GetDuration() const;
        void SetDuration(int duration);
        int GetDurationTo() const;
        void SetDurationTo(int durationto);
        int GetDurationTween() const;
        void SetDuraionTween(int durationtween);
        bool GetLoop() const;
        void SetLoop(bool bLoop);

    private:
        std::string m_strName;
        int m_iDuration;
        float m_fScale;
        int m_iDurationTo;
        int m_iDurationTween;
        bool m_bLoop;
        TMoveBoneDataMap m_movBoneDataMap;
    };
}
#endif//SKELETALANIMATION_CMOVEDATA_H__INCLUDE
