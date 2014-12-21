#ifndef BEYOND_ENGINE_RENDER_SKELETONBONE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SKELETONBONE_H__INCLUDE
#include "MathExt/Location.h"

class CSkeleton;

class CSkeletonBone
{
public:
    CSkeletonBone(uint8_t uIndex, const TString& strName, CSkeleton* pSkeleton);
    ~CSkeletonBone();

    CSkeletonBone* GetParent() const;
    void SetParent(CSkeletonBone* parent);

    const std::vector<CSkeletonBone*>& GetChildrenBone() const;
    void AddChildBone(CSkeletonBone*);

    uint8_t GetIndex() const;
    const TString& GetName() const;

    void SetVisible(bool bRender);
    bool GetVisible()const;
    bool GetCoordinateVisible()const;
    void SetCoordinateVisible(bool bRender);

    void SetTPosWorldTM(const CMat4& worldTM);
    const CMat4& GetTPosWorldTM();
    const CMat4& GetCurrentWorldTM();
    void SetCurrentWorldTM(const CMat4& mat);
private:
    bool m_bVisible;
    bool m_bCoordinateVisible;
    uint8_t m_uIndex;
    TString m_strName;
    CSkeletonBone* m_pParent;
    std::vector<CSkeletonBone*> m_childrenBone;
    CMat4 m_TPosWorldTM;
    CMat4 m_currWorldTM;
};

#endif