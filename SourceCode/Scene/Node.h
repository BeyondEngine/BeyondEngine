#ifndef BEYOND_ENGINE_SCENE_NODE_H_INCLUDE
#define BEYOND_ENGINE_SCENE_NODE_H_INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

class CNode;
class CAttachableObject;
class CEventDispatcher;
class CTouchEvent;
enum ENodeType 
{
    eNT_Node2D,
    eNT_Node3D,
    eNT_RenderBatch,
    eNT_NodeGUI,

    eNT_Count,
    eNT_Force32Bit = 0xFFFFFFFF
};

struct SAnimationProperty
{
    SAnimationProperty()
    {
        m_posForAnimation.Zero();
        m_rotationForAnimation.Zero();
        m_scaleForAnimation.Fill(1.0f, 1.0f, 1.0f);
    }
    ~SAnimationProperty()
    {

    }
    void Reset()
    {
        m_posForAnimation.Zero();
        m_scaleForAnimation.Fill(1.0f, 1.0f, 1.0f);
        m_rotationForAnimation.Zero();
        m_color = 0;
    }
    CVec3 m_posForAnimation;
    CVec3 m_scaleForAnimation;
    CVec3 m_rotationForAnimation;
    CColor m_color;
};

class CNode : public CComponentInstance
{
    DECLARE_REFLECT_GUID( CNode, 0x1458a517, CComponentInstance )
public:
    CNode();
    CNode(ENodeType type);
    virtual ~CNode();

    virtual void Uninitialize() override;
    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;
    virtual bool OnDependencyListChange(void* pComponentAddr, enum EDependencyChangeAction action, CComponentBase* pComponent);
    virtual void Initialize() override;

    CEventDispatcher *EventDispatcher() const;

    virtual void SetVisible( bool bVisible );
    virtual bool IsVisible() const;
    virtual void Activate();
    virtual void Deactivate();
    virtual bool IsActive() const;

    virtual bool SetPosition( float x, float y, float z);
    const CVec3& GetPosition() const;
    CVec3 GetWorldPosition();

    virtual bool SetScale( float x, float y, float z);
    const CVec3& GetScale() const;

    virtual bool SetRotation( float x, float y, float z);
    const CVec3& GetRotation() const;

    virtual void AddChild( CNode* pChild );
    CNode *GetChildByName(const TString &name) const;

    const std::vector<CNode*>& GetChildren() const;

    virtual bool RemoveChild( CNode* pNode );
    const TString& GetName() const;
    virtual void SetName(const TString& strName);
    CNode* GetParentNode() const;
    CAttachableObject* GetAttachedObject() const;

    ENodeType GetType() const;

    virtual void Update( float dtt );
    void Render();
    bool IsRenderingWorldTM();
    void SetRenderWorldTM(bool bRender);

    virtual void SetParentNode( CNode* pParent );

    virtual bool HitTest(float x, float y) const;
    virtual bool OnTouchEvent(CTouchEvent *event);
    virtual bool HandleTouchEvent(CTouchEvent *event);

    void InvalidateLocalTM();
    void InvalidateWorldTM();
    virtual const kmMat4& GetLocalTM();
    void SetLocalTM(const kmMat4& localTM);
    const kmMat4& GetWorldTM();
    virtual void OnWorldTransformUpdate();

    SAnimationProperty* GetAnimationProperty() const;
    void SetAnimationProperty(SAnimationProperty* pProperty);

    CColor GetColorScale(bool bInherit) const;
    void SetColorScale(CColor color);

    bool HasChild( CNode* pNode );

    CNode* CloneNode(bool bResurcive, bool bOnlyCloneComponent, std::vector<CNode*>* pNewNodes = NULL, std::vector<CNode*>* pClonedNodes = NULL);
private:
    virtual void PreRender( );
    virtual void DoRender( );
    virtual void PostRender( );

protected:
    bool m_bLocalTransformInvalid;
    bool m_bWorldTransformInvalid;
    CVec3 m_pos;
    CVec3 m_scale;
    CVec3 m_rotation; // degree
    SAnimationProperty* m_pAnimationProperty;
    TString m_strName;
    kmMat4 m_mat4LocalTransform;
    kmMat4 m_mat4WorldTransform;
private:
    bool m_bActive;
    bool m_bVisible;
    bool m_bRenderWorldTransform;
    ENodeType m_type;
    CColor m_colorScale;// This is a temp solution for scale color, the min scale is 0, max scale is 0xFF / 100 for each component
    CNode* m_pParentNode;
    CEventDispatcher* m_pEventDispatcher;
    std::vector<CNode*> m_nodeChildren;
#ifdef _DEBUG
    std::set<CNode*> m_childrenMapForDebug;
#endif
};

#endif//NODE_H_INCLUDE