#ifndef BEYOND_ENGINE_SKELETALANIMATION_ANIMATIONIMPORTER_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_ANIMATIONIMPORTER_H__INCLUDE

struct Json;
namespace _2DSkeletalAnimation
{
    class CBoneData;
    class CMoveData;
    struct SMoveBoneData;
    class CDisplayData;
    class CArmatureData;
    class CAnimationData;
    class CAnimationImporter
    {
        BEATS_DECLARE_SINGLETON(CAnimationImporter)

    public:
        void ImportData(const TString& strFilePath);
        void DecodeArmature(const TString& strFilePath, CArmatureData* pArmatureData);
        void DecodeAnimation(const TString& strFilePath, CAnimationData* pAnimationData);
    private:
        void DecodeArmature(Json* json);
        void DecodeBoneData(Json* json, CArmatureData* pArmatureData);
        void DecodeDisplayData(Json* json, CBoneData* pBoneData);
        void DecodeSkinData(Json* json, CDisplayData* pDisplayData);

        void DecodeAnimation(Json* json);
        void DecodeMovData(Json* json, CAnimationData* pAnimationData);
        void DecodeMovBoneData(Json *json, CMoveData* pMoveData);
        void DecodeFrameData(Json* json, SMoveBoneData* pMoveBoneData);

        void DecodeTexture(Json* json);

        TString GetStringFromFile(const TCHAR* pszFilePath);
        bool CheckFile(const TString& file);
    private:
        std::map<TString, TString> m_filesMap;
    };
}
#endif//SKELETALANIMATION_CDATAIMPORTER_H__INCLUDE