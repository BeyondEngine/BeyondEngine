#ifndef BEYOND_ENGINE_SCENE_NODE_H__INCLUDE
#define BEYOND_ENGINE_SCENE_NODE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "NodeAnimation/NodeAnimationElement.h"

class CTouchEvent;
class CRenderGroup;
enum ENodeType 
{
    eNT_Node2D,
    eNT_Node3D,

    eNT_Count,
    eNT_Force32Bit = 0xFFFFFFFF
};

struct SAnimationProperty
{
    SAnimationProperty(CNode* pOwner);
    ~SAnimationProperty();
    void Reset();
    void SetPos(const CVec3& pos);
    const CVec3& GetPos() const;
    void SetRotation(const CVec3& rotate);
    const CVec3& GetRotation() const;
    void SetScale(const CVec3& scale);
    const CVec3& GetScale() const;
private:
    CNode* m_pOwner = nullptr;
    CVec3 m_posForAnimation;
    CVec3 m_scaleForAnimation;
    CVec3 m_rotationForAnimation;
};

class CNode : public CComponentInstance
{
    DECLARE_REFLECT_GUID( CNode, 0x1458a517, CComponentInstance )
public:
    CNode();
    virtual ~CNode();

#if defined(DEVELOP_VERSION) && !defined(EDITOR_MODE)
    virtual void Initialize() override;
    virtual void Uninitialize() override;
#endif
    virtual bool Load() override;
    virtual bool Unload() override;
    virtual void ReflectData( CSerializer& serializer ) override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;
    virtual bool OnDependencyListChange(void* pComponentAddr, enum EDependencyChangeAction action, CComponentBase* pComponent) override;
#endif
    virtual void SetVisible( bool bVisible );
    bool IsVisible(bool bInheritFromParent = false) const;
    virtual void Activate(bool bApplyToChild = true);
    virtual void Deactivate(bool bApplyToChild = true);
    bool IsActive(bool bInheritFromParent = false) const;

    virtual bool SetPosition(const CVec3& position);
    const CVec3& GetPosition() const;
    CVec3 GetWorldPosition() const;

    virtual bool SetScale(const CVec3& scale);
    const CVec3& GetScale() const;

    bool SetRotation(const CVec3& rotation);
    const CVec3& GetRotation() const;

    bool IsIgnoreParentRotation() const;
    void SetIgnoreParentRotation(bool bIgnore);

    virtual void AddChild( CNode* pChild );
    CNode *GetChildByName(const TString &name) const;

    const std::vector<CNode*>& GetChildren() const;

    virtual bool RemoveChild( CNode* pNode );
    const TString& GetName() const;
    virtual void SetName(const TString& strName);
    virtual CNode* GetParentNode() const;
    CNode* GetRootNode() const;

    virtual ENodeType GetType() const;

    virtual void Update( float dtt );
    virtual void Render();

    void SetParentNode( CNode* pParent );
    virtual void OnParentNodeChanged(CNode* pParent);

    virtual bool OnTouchEvent(CTouchEvent *event);
    virtual bool HandleTouchEvent(CTouchEvent *event);
    virtual bool HitTest(const CVec2& pt);

    void InvalidateLocalTM();
    virtual void InvalidateWorldTM();
    virtual const CMat4& GetLocalTM() const;
    void SetLocalTM(const CMat4& localTM);
    virtual const CMat4& GetWorldTM() const;

    SAnimationProperty* GetAnimationProperty() const;
    void SetAnimationProperty(SAnimationProperty* pProperty);

    CColor GetColorScale(bool bInherit) const;
    void SetColorScale(CColor color);

    CNode* CloneNode(bool bResurcive, bool bOnlyCloneComponent, std::vector<CComponentInstance*>* pNewNodes = NULL, std::vector<CComponentInstance*>* pClonedNodes = NULL);
    CQuaternion GetWorldQuaternion() const;
    virtual void NodeAnimationUpdate(ENodeAnimationElementType type, uint32_t uCurrFrame, const CVec3& currValue);
    virtual void NodeAnimationReset(ENodeAnimationElementType type);

    virtual void SetRenderGroupID(ERenderGroupID id);
    ERenderGroupID GetRenderGroupID() const;
    CRenderGroup* GetRenderGroup() const;

protected:
    virtual void PreRender( );
    virtual void DoRender( );
    virtual void PostRender( );

protected:
    mutable bool m_bLocalTransformInvalid;
    mutable bool m_bWorldTransformInvalid;
    CVec3 m_pos;
    CVec3 m_scale;
    CVec3 m_rotation; // degree
    SAnimationProperty* m_pAnimationProperty;
    TString m_strName;
    mutable CMat4 m_mat4LocalTransform;
    mutable CMat4 m_mat4WorldTransform;
    bool m_bNeedPopGroupID = false;
    ERenderGroupID m_renderGroupID = LAYER_UNSET;
    ERenderGroupID m_defaultGroupID = LAYER_UNSET;
protected:
    bool m_bActive;
    bool m_bVisible;
    bool m_bIgnoreParentRotation = false;
    CColor m_colorScale;// This is a temp solution for scale color, the min scale is 0, max scale is 0xFF / 100 for each component
    CNode* m_pParentNode;
    std::vector<CNode*> m_nodeChildren;
#ifdef _DEBUG
    std::set<CNode*> m_childrenMapForDebug;
#endif
};

#endif//NODE_H_INCLUDE