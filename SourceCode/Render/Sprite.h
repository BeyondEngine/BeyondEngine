#ifndef BEYOND_ENGINE_RENDER_SPRITE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SPRITE_H__INCLUDE

#include "Scene/Node3D.h"
#include "ReflectTextureInfo.h"

class CTextureFrag;
class CMaterial;

class CSprite : public CNode3D
{
    DECLARE_REFLECT_GUID( CSprite, 0xe3bb1001, CNode3D )
public:
    CSprite();
    virtual ~CSprite();

    virtual void ReflectData(CSerializer& serializer) override;
#ifdef EDITOR_MODE
    virtual bool OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    virtual bool Load() override;
    virtual void SetCurrFrame(uint32_t currFrame);
    virtual uint32_t GetFrameCount() const;

    virtual void DoRender() override;
    virtual void PostRender() override;
    virtual bool HitTest(const CVec2& pt) override;

    void SetAnchor(float anchorX, float anchorY);
    const CVec2& GetAnchor() const;
    void SetSize(float width, float height);
    const CVec2& GetSize() const;
    const CVec2& GetRealSize() const;
    void UpdateVertices();

    const CQuadP &GetQuadP() const;
    virtual const CQuadT &GetQuadT() const;
    virtual SharePtr<CTextureFrag> GetTextureFrag() const;
    virtual CSprite* CloneSprite(bool bResurcive, std::vector<CComponentInstance*>& newNodeList, std::vector<CComponentInstance*>& clonedNodeList);
    virtual void Play(bool bPause, bool bContinuePlay, bool bReversePlay, uint32_t uStartIndex, float fSpeed);
    virtual bool IsAnimationPlaying() const;

    void SetUseInheritColorScale(bool bUse);
    void AddChildSprite(CSprite* pSprite);
    void RemoveChildSprite(CSprite* pChild);

    void SetParentSprite(CSprite* pParentSprite);
    CSprite* GetParentSprite() const;
    virtual CSprite* GetUserDefinePos(const TString& strPointName, bool bWorldOrLocal, CVec3& outPos) const override;
    virtual CRect GetSelfRect() const;
    virtual CRect GetBoundingRect() const;
    virtual void GetCurrentRenderSpriteList(std::vector<CSprite*>& spriteList) const;

    const std::vector<CSprite*>& GetChildSpriteList() const;
    CSprite* GetChildSpriteByName(const TString& name, bool bRecursion = true);
    void SetTextureFragInfo(const SReflectTextureInfo& info);

#ifdef DEVELOP_VERSION
    void RenderEdge() const;
    virtual void RenderUserDefinePos() const;
    bool m_bRenderUserdefinePos = false;
#endif
#ifdef EDITOR_MODE
    void ClearPos();
    void ClearAnchor();
    void ClearScale();
#endif
protected:
    void UpdateRealSize();

protected:
    bool m_bBlendAdd;
    bool m_bUseInheritColorScale;
    CVec2 m_anchor;
    CVec2 m_size;
    CVec2 m_realSize;
    SReflectTextureInfo m_fragInfo;
    CQuadP m_quad;
    CSprite* m_pParentSprite;
    std::vector<CSprite*> m_childrenSprite;
    std::vector<CSprite*> m_childrenSpriteDependencyList;
    std::map<TString, CVec3> m_userDefinePos;
};

#endif