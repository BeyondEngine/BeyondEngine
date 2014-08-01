#ifndef BEYOND_ENGINE_RENDER_SKELETONBONE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SKELETONBONE_H__INCLUDE

class CSkeletonBone
{
public:
    CSkeletonBone(SharePtr<CSkeletonBone> pParent, ESkeletonBoneType type);
    virtual ~CSkeletonBone();
public:
    SharePtr<CSkeletonBone> GetParent();
    void SetParent(SharePtr<CSkeletonBone> parent);

    //TODO: these 2 functions are temp, should removed.
    void SetParentType(ESkeletonBoneType type);
    ESkeletonBoneType GetParentType() const;

    const std::vector<SharePtr<CSkeletonBone>>& GetChild() const;
    ESkeletonBoneType GetBoneType() const;
    const kmMat4& GetTPosMatrix() const; // In bone coordinate.
    void SetTPosMatrix(const kmMat4& matrix);
    void SetVisible(bool bRender);
    bool GetVisible()const;
    void SetCoordinateVisible(bool bRender);
    bool GetCoordinateVisible()const;

private:
    bool m_bVisible;
    bool m_bCoordinateVisible;
    ESkeletonBoneType m_type;
    ESkeletonBoneType m_parentType;

    SharePtr<CSkeletonBone> m_pParent;
    kmMat4 m_TPosMatrix;
    std::vector<SharePtr<CSkeletonBone>> m_child;
};

#endif