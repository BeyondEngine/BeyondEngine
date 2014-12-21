#include "stdafx.h"
#include "Node.h"
#include "EnginePublic/CompWrapper.h"
#include "NodeEvent.h"
#include "Render/RenderManager.h"
#include "NodeAnimation/NodeAnimationManager.h"
#include "Event/TouchEvent.h"
#ifdef EDITOR_MODE
#include "Render/CoordinateRenderObject.h"
#endif

SAnimationProperty::SAnimationProperty(CNode* pOwner)
{
    m_pOwner = pOwner;
    BEATS_ASSERT(pOwner != nullptr);
    m_scaleForAnimation.Fill(1.0f, 1.0f, 1.0f);
}

SAnimationProperty::~SAnimationProperty()
{

}

void SAnimationProperty::Reset()
{
    m_posForAnimation.Zero();
    m_scaleForAnimation.Fill(1.0f, 1.0f, 1.0f);
    m_rotationForAnimation.Zero();
}

void SAnimationProperty::SetPos(const CVec3& pos)
{
    BEATS_ASSERT(m_pOwner != nullptr);
    if (m_posForAnimation != pos)
    {
        m_posForAnimation = pos;
        m_pOwner->InvalidateLocalTM();
    }
}

const CVec3& SAnimationProperty::GetPos() const
{
    return m_posForAnimation;
}

void SAnimationProperty::SetRotation(const CVec3& rotate)
{
    BEATS_ASSERT(m_pOwner != nullptr);
    if (m_rotationForAnimation != rotate)
    {
        m_rotationForAnimation = rotate;
        m_pOwner->InvalidateLocalTM();
    }
}

const CVec3& SAnimationProperty::GetRotation() const
{
    return m_rotationForAnimation;
}

void SAnimationProperty::SetScale(const CVec3& scale)
{
    BEATS_ASSERT(m_pOwner != nullptr);
    if (m_scaleForAnimation != scale)
    {
        m_scaleForAnimation = scale;
        m_pOwner->InvalidateLocalTM();
    }
}

const CVec3& SAnimationProperty::GetScale() const
{
    return m_scaleForAnimation;
}

CNode::CNode()
    : m_bActive(true)
    , m_bLocalTransformInvalid(true)
    , m_bWorldTransformInvalid(true)
    , m_pAnimationProperty(NULL)
    , m_bVisible(true)
    , m_pParentNode(nullptr)
{
    m_colorScale.r = 100; // scale is 100 / 100 = 1.0f, means no scale. max scale is 2.55f
    m_colorScale.g = 100;
    m_colorScale.b = 100;
    m_colorScale.a = 100;
    m_pos.Fill( 0.0f, 0.0f, 0.0f );
    m_scale.Fill( 1.0f, 1.0f, 1.0f );
    m_rotation.Fill(0.0f, 0.0f, 0.0f );
    m_mat4LocalTransform.Identity();
    m_mat4WorldTransform.Identity();
}

CNode::~CNode()
{
    CNodeAnimationManager::GetInstance()->RemoveNode(this);
    BEATS_SAFE_DELETE(m_pAnimationProperty);
}

void CNode::SetVisible( bool bVisible )
{
    m_bVisible = bVisible;
}

bool CNode::IsVisible(bool bInheritFromParent) const
{
    bool bRet = m_bVisible;
    if (bRet && bInheritFromParent && m_pParentNode != nullptr)
    {
        bRet = m_pParentNode->IsVisible(true);
    }
    return bRet;
}

void CNode::Activate(bool bApplyToChild/* = true*/)
{
    if(!m_bActive)
    {
        m_bActive = true;
        if (bApplyToChild)
        {
            for (auto pChild : m_nodeChildren)
            {
                pChild->Activate();
            }
        }
    }
}

void CNode::Deactivate(bool bApplyToChild/* = true*/)
{
    if(m_bActive)
    {
        m_bActive = false;
        if (bApplyToChild)
        {
            for (auto pChild : m_nodeChildren)
            {
                pChild->Deactivate();
            }
        }
    }
}

bool CNode::IsActive(bool bInheritFromParent) const
{
    bool bRet = m_bActive;
    if (bRet && bInheritFromParent && m_pParentNode != nullptr)
    {
        bRet = m_pParentNode->IsActive(true);
    }
    return bRet;
}

void CNode::AddChild( CNode* pChild )
{
    BEATS_ASSERT(pChild != NULL, _T("Can't add null as a node child."));
    if (pChild != NULL)
    {
#ifdef _DEBUG
        BEATS_ASSERT(m_childrenMapForDebug.find(pChild) == m_childrenMapForDebug.end(), _T("Node can't be added twice in the same parent!"));
#endif
        BEATS_ASSERT(pChild->GetParentNode() == NULL, _T("Child Can't be added when it has parent"));
        BEATS_ASSERT(pChild != this);
        if(IsActive())
        {
            pChild->Activate();
        }
#ifdef _DEBUG
        m_childrenMapForDebug.insert(pChild);
#endif
        m_nodeChildren.push_back( pChild );
        pChild->SetParentNode( this );
    }
}

CNode *CNode::GetChildByName(const TString &name) const
{
    CNode *pRet = nullptr;
    for(CNode *pChild : m_nodeChildren)
    {
        if (pChild->GetName() == name)
        {
            pRet = pChild;
            break;
        }
        else
        {
            pRet = pChild->GetChildByName(name);
        }
    }
    return pRet;
}

const std::vector<CNode*>& CNode::GetChildren() const
{
    return m_nodeChildren;
}

bool CNode::RemoveChild( CNode* pNode )
{
    bool ret = false;
    BEATS_ASSERT(pNode != nullptr);
    if (pNode->IsInitialized())
    {
#ifdef _DEBUG
        BEATS_ASSERT(m_childrenMapForDebug.find(pNode) != m_childrenMapForDebug.end(), _T("Can't remove node which seems not exist"));
        m_childrenMapForDebug.erase(pNode);
#endif
        for (auto iter = m_nodeChildren.begin(); iter != m_nodeChildren.end(); ++iter)
        {
            if ( *iter == pNode )
            {
                pNode->SetParentNode( nullptr );
                m_nodeChildren.erase( iter );
                ret = true;
                break;
            }
        }
    }

    return ret;
}

const TString& CNode::GetName() const
{
    return m_strName;
}

void CNode::SetName(const TString& strName )
{
    m_strName = strName;
}

CNode* CNode::GetParentNode() const
{
    return m_pParentNode;
}

CNode* CNode::GetRootNode() const
{
    CNode* pRoot = const_cast<CNode*>(this);
    while (pRoot->GetParentNode() != nullptr)
    {
        pRoot = pRoot->GetParentNode();
    }
    return pRoot;
}

void CNode::SetParentNode( CNode* pParent )
{
    if ( m_pParentNode != pParent )
    {
        BEATS_ASSERT( ( nullptr == m_pParentNode ) || ( pParent == nullptr ));
        m_pParentNode = pParent;
        if (IsInitialized())
        {
            InvalidateWorldTM();
        }
        OnParentNodeChanged(pParent);
    }
}

void CNode::OnParentNodeChanged(CNode* /*pParent*/)
{

}

ENodeType CNode::GetType() const
{
    return eNT_Count;
}

bool CNode::Unload()
{
    for ( auto iter : m_nodeChildren )
    {
        iter->SetParentNode( nullptr );
    }
    if (m_pParentNode != NULL)
    {
        m_pParentNode->RemoveChild(this);
    }
#ifdef EDITOR_MODE
    if (CRenderManager::GetInstance()->GetCoordinateRenderObject()->GetRenderNode() == this)
    {
        CRenderManager::GetInstance()->GetCoordinateRenderObject()->SetRenderNode(NULL);
    }
#endif
    return super::Unload();
}

void CNode::ReflectData( CSerializer& serializer )
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY( serializer, m_strName, true, 0xFFFFFFFF, _T( "名字" ), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bVisible, true, 0xFFFFFFFF, _T( "是否可见" ), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_bActive, true, 0xFFFFFFFF, _T("激活"), NULL, NULL, NULL);
    DECLARE_PROPERTY( serializer, m_pos, true, 0xFFFFFFFF, _T( "位置" ), NULL, NULL, _T("SpinStep:0.1") );
    DECLARE_PROPERTY(serializer, m_rotation, true, 0xFFFFFFFF, _T("旋转"), NULL, NULL, _T("SpinStep:1, MinValue:0, MaxValue:360"));
    DECLARE_PROPERTY(serializer, m_scale, true, 0xFFFFFFFF, _T("缩放"), NULL, NULL, _T("SpinStep:0.1"));
    DECLARE_PROPERTY(serializer, m_renderGroupID, true, 0xFFFFFFFF, _T("渲染层"), NULL, NULL, NULL);
    DECLARE_DEPENDENCY_LIST( serializer, m_nodeChildren, _T("子节点"), eDT_Weak);
}
#ifdef EDITOR_MODE
bool CNode::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool ret = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !ret )
    {
        if (&m_strName == pVariableAddr)
        {
            std::string name;
            DeserializeVariable(name, pSerializer, this);
            this->SetName(name);
            ret = true;
        }
        else if(&m_bVisible == pVariableAddr)
        {
            bool bVisible = false;
            DeserializeVariable(bVisible, pSerializer, this);
            SetVisible( bVisible );
            ret = true;
        }
        else if( &m_pos == pVariableAddr )
        {
            CVec3 pos;
            DeserializeVariable(pos, pSerializer, this);
            SetPosition( pos );
            ret = true;
        }
        else if (&m_rotation == pVariableAddr)
        {
            CVec3 rotation;
            DeserializeVariable(rotation, pSerializer, this);
            SetRotation(rotation);
            ret = true;
        }
        else if(pVariableAddr == &m_scale)
        {
            CVec3 data;
            DeserializeVariable(data, pSerializer, this);
            SetScale(data);
            ret = true;
        }
    }
    return ret;
}

bool CNode::OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent)
{
    bool ret = super::OnDependencyListChange( pComponentAddr, action, pComponent );
    if (!ret)
    {
        if ( &m_nodeChildren == pComponentAddr )
        {
            BEATS_ASSERT(dynamic_cast<CNode*>(pComponent) != NULL || action == eDCA_Delete);
            if (action == eDCA_Add)
            {
                AddChild(down_cast<CNode*>(pComponent));
                ret = true;
            }
            else if ( action == eDCA_Delete )
            {
                RemoveChild(down_cast<CNode*>(pComponent));
                ret = true;
            }
        }
    }
    return ret;
}
#endif
#if defined(DEVELOP_VERSION) && !defined(EDITOR_MODE)
void CNode::Initialize()
{
#if (BEYONDENGINE_PLATFORM != PLATFORM_LINUX)
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "Node::Initialize can only be invoked in main thread!");
#endif
    super::Initialize();
}

void CNode::Uninitialize()
{
#if (BEYONDENGINE_PLATFORM != PLATFORM_LINUX)
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "Node::Initialize can only be invoked in main thread!");
#endif
    super::Uninitialize();
}
#endif

bool CNode::Load()
{
    for ( auto iter : m_nodeChildren )
    {
        iter->SetParentNode( this );
#ifdef _DEBUG
        m_childrenMapForDebug.insert(iter);
#endif
    }
    return super::Load();
}

bool CNode::SetPosition(const CVec3& position)
{
    bool bRet = false;
    if (position != m_pos)
    {
        m_pos = position;
        InvalidateLocalTM();
        bRet = true;
    }
    return bRet;
}

const CVec3& CNode::GetPosition() const
{
    return m_pos;
}

CVec3 CNode::GetWorldPosition() const
{
    const CMat4& worldMat = GetWorldTM();
    return worldMat.GetTranslate();
}

bool CNode::SetScale(const CVec3& scale)
{
    bool bRet = false;
    if (m_scale != scale)
    {
        m_scale = scale;
        InvalidateLocalTM();
        bRet = true;
    }
    return bRet;
}

const CVec3& CNode::GetScale() const
{
    return m_scale;
}

bool CNode::SetRotation(const CVec3& rotation)
{
   bool bRet = false;
   if (rotation != m_rotation)
    {
        m_rotation = rotation;
        InvalidateLocalTM();
        bRet = true;
    }
    return bRet;

}

const CVec3& CNode::GetRotation() const
{
    return m_rotation;
}

bool CNode::IsIgnoreParentRotation() const
{
    return m_bIgnoreParentRotation;
}

void CNode::SetIgnoreParentRotation(bool bIgnore)
{
    m_bIgnoreParentRotation = bIgnore;
}

void CNode::Render()
{
    BEATS_ASSERT(IsInitialized(), _T("Rendering an unintialize node!"));
    if(IsVisible())
    {
        PreRender();
        DoRender();
        PostRender();
    }
}

void CNode::Update( float dtt )
{
    if(IsActive())
    {
        const std::vector<CNode*>& children = GetChildren();
        for (size_t i = 0; i < children.size(); ++i) //  NOTICE: you better not change children in iter->Update(dtt)
        {
            children[i]->Update(dtt);
        }
    }
}

bool CNode::OnTouchEvent(CTouchEvent *event)
{
    bool bRet = false;
    for (uint32_t i = 0; i < m_nodeChildren.size(); ++i)
    {
        if (m_nodeChildren[i]->IsVisible())
        {
            if (m_nodeChildren[i]->OnTouchEvent(event))
            {
                event->StopPropagation();
                bRet = true;
                break;
            }
        }
    }
    return bRet;
}

bool CNode::HandleTouchEvent(CTouchEvent* /*event*/)
{
    return false;
}

bool CNode::HitTest(const CVec2& /*pt*/)
{
    return false;
}

void CNode::InvalidateLocalTM()
{
    m_bLocalTransformInvalid = true;
    InvalidateWorldTM();
}

void CNode::InvalidateWorldTM()
{
    m_bWorldTransformInvalid = true;
    for (uint32_t i = 0; i < m_nodeChildren.size(); ++i)
    {
        BEATS_ASSERT(m_nodeChildren[i] != nullptr);
        m_nodeChildren[i]->InvalidateWorldTM();
    }
}

const CMat4& CNode::GetLocalTM() const
{
    if (m_bLocalTransformInvalid)
    {
        CVec3 scaleVec = m_scale;
        CVec3 rotationVec = m_rotation;
        CVec3 posVec = m_pos;

        if (m_pAnimationProperty != NULL)
        {
            scaleVec *= m_pAnimationProperty->GetScale();
            rotationVec += m_pAnimationProperty->GetRotation();
            posVec += m_pAnimationProperty->GetPos();
        }
        if (BEATS_FLOAT_EQUAL(scaleVec.X(), 0))
        {
            scaleVec.X() = 1.0f;
        }
        if (BEATS_FLOAT_EQUAL(scaleVec.Y(), 0))
        {
            scaleVec.Y() = 1.0f;
        }
        if (BEATS_FLOAT_EQUAL(scaleVec.Z(), 0))
        {
            scaleVec.Z() = 1.0f;
        }
        m_mat4LocalTransform.Build(posVec, CVec3(DegreesToRadians(rotationVec.X()), DegreesToRadians(rotationVec.Y()), DegreesToRadians(rotationVec.Z())), scaleVec);
        m_bLocalTransformInvalid = false;
    }
    return m_mat4LocalTransform;
}

void CNode::SetLocalTM(const CMat4& localTM)
{
    m_mat4LocalTransform = localTM;
    m_bLocalTransformInvalid = false;
    InvalidateWorldTM();
}

const CMat4& CNode::GetWorldTM() const
{
    const CMat4* pValue = &m_mat4WorldTransform;
    if (m_pParentNode == NULL)
    {
        pValue = &GetLocalTM();
    }
    else
    {
        if (m_bWorldTransformInvalid)
        {
            if (IsIgnoreParentRotation())
            {
                CMat4 parentWorldTM = m_pParentNode->GetWorldTM();
                parentWorldTM.RemoveRotate();
                m_mat4WorldTransform = parentWorldTM * GetLocalTM();
            }
            else
            {
                m_mat4WorldTransform = m_pParentNode->GetWorldTM() * GetLocalTM();
            }
            m_bWorldTransformInvalid = false;
        }
    }
    return *pValue;
}

SAnimationProperty* CNode::GetAnimationProperty() const
{
    return m_pAnimationProperty;
}

void CNode::SetAnimationProperty(SAnimationProperty* pProperty)
{
    m_pAnimationProperty = pProperty;
    InvalidateLocalTM();
}

CColor CNode::GetColorScale(bool bInherit) const
{
    CColor ret = m_colorScale;
    if (bInherit)
    {
        if (m_pParentNode != NULL)
        {
            CColor inheritColor = m_pParentNode->GetColorScale(true);
            ret.r = (unsigned char)(ret.r * (inheritColor.r * 0.01f));
            ret.g = (unsigned char)(ret.g * (inheritColor.g * 0.01f));
            ret.b = (unsigned char)(ret.b * (inheritColor.b * 0.01f));
            ret.a = MIN((unsigned char)(ret.a * (inheritColor.a * 0.01f)), 100);
        }
    }
    return ret;
}

void CNode::SetColorScale(CColor color)
{
    m_colorScale = color;
}

void CNode::PreRender()
{
    if (m_renderGroupID != LAYER_UNSET)
    {
        m_bNeedPopGroupID = true;
        CRenderGroupManager::GetInstance()->PushRenderGroupID(m_renderGroupID);
    }
    else
    {
        if (CRenderGroupManager::GetInstance()->GetRenderGroupIDStack().size() == 0)
        {
            m_bNeedPopGroupID = true;
            BEATS_ASSERT(m_defaultGroupID != LAYER_UNSET);
            CRenderGroupManager::GetInstance()->PushRenderGroupID(m_defaultGroupID);
        }
    }
}

void CNode::DoRender()
{
}

void CNode::PostRender()
{
    const std::vector<CNode*>& children = GetChildren();
    for (int i = children.size() - 1; i >= 0; --i)
    {
        children[i]->Render();
    }
    if (m_bNeedPopGroupID)
    {
        m_bNeedPopGroupID = false;
        CRenderGroupManager::GetInstance()->PopRenderGroupID();
    }
}

CNode* CNode::CloneNode(bool bResurcive, bool bOnlyCloneComponent, std::vector<CComponentInstance*>* pNewNodes, std::vector<CComponentInstance*>* pClonedNodes)
{
    CNode* pRet = NULL;
    if (!bOnlyCloneComponent || GetId() != 0xFFFFFFFF)
    {
        pRet = down_cast<CNode*>(CloneInstance());
        if (pNewNodes != NULL)
        {
            pNewNodes->push_back(pRet);
        }
        if (pClonedNodes != NULL)
        {
            pClonedNodes->push_back(this);
        }
        if (bResurcive)
        {
            for (uint32_t i = 0; i < m_nodeChildren.size(); ++i)
            {
                if (!bOnlyCloneComponent || m_nodeChildren[i]->GetId() != 0xFFFFFFFF)
                {
                    CNode* pChild = m_nodeChildren[i]->CloneNode(true, bOnlyCloneComponent, pNewNodes, pClonedNodes);
                    pRet->AddChild(pChild);
                }
            }
        }
        BEATS_ASSERT(pNewNodes == NULL || pClonedNodes == NULL || pClonedNodes->size() == pNewNodes->size());
    }

    return pRet;
}

CQuaternion CNode::GetWorldQuaternion() const
{
    //Don't call worldTM.RemoveScale, since the scale contains sign
    CMat4 worldTM = GetWorldTM();
    worldTM[0] /= m_scale.X();
    worldTM[1] /= m_scale.X();
    worldTM[2] /= m_scale.X();

    worldTM[4] /= m_scale.Y();
    worldTM[5] /= m_scale.Y();
    worldTM[6] /= m_scale.Y();


    worldTM[8] /= m_scale.Z();
    worldTM[9] /= m_scale.Z();
    worldTM[10] /= m_scale.Z();
    return worldTM.ToQuaternion();
}

void CNode::NodeAnimationUpdate(ENodeAnimationElementType type, uint32_t /*uCurrFrame*/, const CVec3& currValue)
{
    SAnimationProperty* pProperty = this->GetAnimationProperty();
    if (pProperty == NULL)
    {
        pProperty = new SAnimationProperty(this);
        this->SetAnimationProperty(pProperty);
    }
    switch (type)
    {
    case eNAET_Scale:
        pProperty->SetScale(currValue);
        break;
    case eNAET_Pos:
        pProperty->SetPos(currValue);
        break;
    case eNAET_Rotation:
        pProperty->SetRotation(currValue);
        break;
    case eNAET_AlphaScale:
    {
        CColor colorScale = this->GetColorScale(false);
        colorScale.a = (unsigned char)currValue.X();
        BEATS_ASSERT(colorScale.a <= 100, "alpha value should not greater than 100 in alpha scale.");
        this->SetColorScale(colorScale);
    }
        break;
    case eNAET_ColorScale:
    {
        CColor color;
        color.r = (unsigned char)currValue.X();
        color.g = (unsigned char)currValue.Y();
        color.b = (unsigned char)currValue.Z();
        color.a = 100;//means no scale in alpha for now.
        this->SetColorScale(color);
    }
        break;
    default:
        break;
    }
}

void CNode::NodeAnimationReset(ENodeAnimationElementType /*type*/)
{
    // Do nothing
}

void CNode::SetRenderGroupID(ERenderGroupID id)
{
    m_renderGroupID = id;
}

ERenderGroupID CNode::GetRenderGroupID() const
{
    return m_renderGroupID;
}

CRenderGroup* CNode::GetRenderGroup() const
{
    return CRenderGroupManager::GetInstance()->GetRenderGroup();
}
