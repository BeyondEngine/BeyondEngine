#ifndef BEYOND_ENGINE_RENDER_SKELETON_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SKELETON_H__INCLUDE

class CSkeletonBone;

class CSkeleton
{
public:
    CSkeleton();
    virtual ~CSkeleton();

    CSkeletonBone* GetSkeletonBoneById(uint8_t uIndex) const;
    CSkeletonBone* GetSkeletonBoneByName(const TString& strName) const;
    const std::map<uint8_t, CSkeletonBone* >& GetBoneMap() const;

    const TString& GetBoneName(uint8_t uIndex) const;
    uint8_t GetBoneIndex(const TString& strName) const;
    bool Load(CSerializer& serializer);

    uint8_t GetFPS()const;
    void SetFPS(uint8_t uFPS);
#ifdef DEVELOP_VERSION
    void PrintBoneTree(CSkeletonBone* pBone, uint32_t level);
#endif
private:
    uint8_t m_uFPS = 0;
    std::map<uint8_t, CSkeletonBone*> m_bonesIdMap;
};

#endif