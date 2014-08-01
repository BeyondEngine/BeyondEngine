#include "stdafx.h"
#include "Node.h"
#include "CompWrapper.h"
#include "Event/EventDispatcher.h"
#include "NodeEvent.h"
#include "Render/RenderManager.h"
#include "Event/TouchEvent.h"
#include "NodeAnimation/NodeAnimationManager.h"


CNode::CNode()
    : m_bActive(true)
    , m_bLocalTransformInvalid(true)
    , m_bWorldTransformInvalid(true)
    , m_pAnimationProperty(NULL)
    , m_bVisible(true)
    , m_bRenderWorldTransform(false)
    , m_type( eNT_NodeGUI )
    , m_pParentNode(nullptr)
{
    m_colorScale.r = 100; // scale is 100 / 100 = 1.0f, means no scale. max scale is 2.55f
    m_colorScale.g = 100;
    m_colorScale.b = 100;
    m_colorScale.a = 100;
    kmVec3Fill( &m_pos, 0.0f, 0.0f, 0.0f );
    kmVec3Fill( &m_scale, 1.0f, 1.0f, 1.0f );
    kmVec3Fill( &m_rotation, 0.0f, 0.0f, 0.0f );
    kmMat4Identity(&m_mat4LocalTransform );
    kmMat4Identity(&m_mat4WorldTransform);

    m_pEventDispatcher = new CCompWrapper<CEventDispatcher, CNode>(this);
}


CNode::CNode(ENodeType type)
    : m_bActive(true)
    , m_bLocalTransformInvalid(true)
    , m_bWorldTransformInvalid(true)
    , m_pAnimationProperty(NULL)
    , m_bVisible(true)
    , m_bRenderWorldTransform(false)
    , m_type(type)
    , m_pParentNode(nullptr)
{
    m_colorScale.r = 100; // scale is 100 / 100 = 1.0f, means no scale. max scale is 2.55f
    m_colorScale.g = 100;
    m_colorScale.b = 100;
    m_colorScale.a = 100;
    kmVec3Fill( &m_pos, 0.0f, 0.0f, 0.0f );
    kmVec3Fill( &m_scale, 1.0f, 1.0f, 1.0f );
    kmVec3Fill( &m_rotation, 0.0f, 0.0f, 0.0f );
    kmMat4Identity(&m_mat4LocalTransform );
    kmMat4Identity(&m_mat4WorldTransform);

    m_pEventDispatcher = new CCompWrapper<CEventDispatcher, CNode>(this);
}

CNode::~CNode()
{
    CNodeAnimationManager::GetInstance()->RemoveNodeAnimation(this);
    BEATS_SAFE_DELETE(m_pEventDispatcher);
    BEATS_SAFE_DELETE(m_pAnimationProperty);
}

void CNode::SetVisible( bool bVisible )
{
    m_bVisible = bVisible;
}

bool CNode::IsVisible() const
{
    return m_bVisible;
}

void CNode::Activate()
{
    if(!m_bActive)
    {
        m_bActive = true;
        for(auto pChild : m_nodeChildren)
        {
            pChild->Activate();
        }
    }
}

void CNode::Deactivate()
{
    if(m_bActive)
    {
        for(auto pChild : m_nodeChildren)
        {
            pChild->Deactivate();
        }
        m_bActive = false;
    }
}

bool CNode::IsActive() const
{
    return m_bActive;
}

void CNode::AddChild( CNode* pChild )
{
    BEATS_ASSERT(pChild != NULL, _T("Can't add null as a node child."));
    if (pChild != NULL)
    {
        BEATS_ASSERT(m_childrenMapForDebug.find(pChild) == m_childrenMapForDebug.end(), _T("Node can't be added twice in the same parent!"));
        BEATS_ASSERT(pChild->GetParentNode() == NULL, _T("Child Can't be added when it has parent"));
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
        pRet = pChild->GetName() == name ? pChild : pChild->GetChildByName(name);
        if(pRet)
            break;
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
    BEATS_ASSERT(m_childrenMapForDebug.find(pNode) != m_childrenMapForDebug.end(), _T("Can't remove node which seems not exist"));
#ifdef _DEBUG
    m_childrenMapForDebug.erase(pNode);
#endif
    for (auto iter = m_nodeChildren.begin(); iter != m_nodeChildren.end(); ++iter)
    {
        if ( *iter == pNode )
        {
            if (pNode->IsInitialized())
            {
                pNode->SetParentNode( nullptr );
            }
            m_nodeChildren.erase( iter );
            ret = true;
            break;
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
    }
}

ENodeType CNode::GetType() const
{
    return m_type;
}

void CNode::Uninitialize()
{
    for ( auto iter : m_nodeChildren )
    {
        iter->SetParentNode( nullptr );
    }
    super::Uninitialize();
}

void CNode::ReflectData( CSerializer& serializer )
{
    super::ReflectData(serializer);

    DECLARE_PROPERTY( serializer, m_strName, true, 0xFFFFFFFF, _T( "名字" ), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bVisible, true, 0xFFFFFFFF, _T( "是否可见" ), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_pos, true, 0xFFFFFFFF, _T( "位置" ), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_rotation, true, 0xFFFFFFFF, _T("旋转"), NULL, NULL, _T("SpinStep:1, MinValue:0, MaxValue:360"));
    DECLARE_PROPERTY(serializer, m_scale, true, 0xFFFFFFFF, _T("缩放"), NULL, NULL, _T("SpinStep:0.1"));

    DECLARE_DEPENDENCY_LIST( serializer, m_nodeChildren, _T("子节点"), eDT_Weak);
}

bool CNode::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool ret = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !ret )
    {
        if(&m_bVisible == pVariableAddr)
        {
            bool bVisible = false;
            DeserializeVariable( bVisible, pSerializer );
            SetVisible( bVisible );
            ret = true;
        }
        else if( &m_pos == pVariableAddr )
        {
            CVec3 pos;
            DeserializeVariable( pos, pSerializer );
            SetPosition( pos.x, pos.y, pos.z );
            ret = true;
        }
        else if (&m_rotation == pVariableAddr)
        {
            CVec3 rotation;
            DeserializeVariable( rotation, pSerializer );
            SetRotation(rotation.x, rotation.y, rotation.z);
            ret = true;
        }
        else if(pVariableAddr == &m_scale)
        {
            CVec3 data;
            DeserializeVariable(data, pSerializer);
            SetScale(data.x, data.y, data.z);
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
                AddChild((CNode*)pComponent);
                ret = true;
            }
            else if ( action == eDCA_Change || action == eDCA_Delete )
            {
                RemoveChild( (CNode*)pComponent );
                ret = true;
            }
        }
    }
    return ret;
}

void CNode::Initialize()
{
    super::Initialize();
    for ( auto iter : m_nodeChildren )
    {
        iter->SetParentNode( this );
#ifdef _DEBUG
        m_childrenMapForDebug.insert(iter);
#endif
    }
}

CEventDispatcher *CNode::EventDispatcher() const
{
    return m_pEventDispatcher;
}

bool CNode::SetPosition( float x, float y, float z )
{
    bool bRet = false;
    if (!BEATS_FLOAT_EQUAL(m_pos.x, x) ||
        !BEATS_FLOAT_EQUAL(m_pos.y, y) ||
        !BEATS_FLOAT_EQUAL(m_pos.z, z))
    {
        kmVec3Fill( &m_pos, x, y, z );
        InvalidateLocalTM();
        bRet = true;
        UPDATE_PROPERTY_PROXY(m_pos);
    }
    return bRet;
}

const CVec3& CNode::GetPosition() const
{
    return m_pos;
}

CVec3 CNode::GetWorldPosition()
{
    const kmMat4& worldMat = GetWorldTM();
    return CVec3(worldMat.mat[12], worldMat.mat[13], worldMat.mat[14]);
}

bool CNode::SetScale( float x, float y, float z)
{
    bool bRet = false;
    if (!BEATS_FLOAT_EQUAL(m_scale.x, x) ||
        !BEATS_FLOAT_EQUAL(m_scale.y, y) ||
        !BEATS_FLOAT_EQUAL(m_scale.z, z))
    {
        kmVec3Fill( &m_scale, x, y, z );
        InvalidateLocalTM();
        bRet = true;
    }
    return bRet;
}

const CVec3& CNode::GetScale() const
{
    return m_scale;
}

bool CNode::SetRotation( float x, float y, float z)
{
   bool bRet = false;
    if (!BEATS_FLOAT_EQUAL(m_rotation.x, x) ||
        !BEATS_FLOAT_EQUAL(m_rotation.y, y) ||
        !BEATS_FLOAT_EQUAL(m_rotation.z, z))
    {
        kmVec3Fill( &m_rotation, x, y, z );
        InvalidateLocalTM();
        bRet = true;
    }
    return bRet;

}

const CVec3& CNode::GetRotation() const
{
    return m_rotation;
}

void CNode::Render()
{
    BEATS_ASSERT(IsInitialized(), _T("Rendering an unintialize node!"));
    if(IsActive() && IsVisible())
    {
        PreRender();
        DoRender();
        PostRender();
        if (m_bRenderWorldTransform)
        {
            CRenderManager::GetInstance()->RenderCoordinate(&GetWorldTM());
        }
    }
}

bool CNode::IsRenderingWorldTM()
{
    return m_bRenderWorldTransform;
}

void CNode::SetRenderWorldTM(bool bRender)
{
    m_bRenderWorldTransform = bRender;
}

void CNode::Update( float dtt )
{
    if(IsActive())
    {
        const std::vector<CNode*>& children = GetChildren();
        for ( auto iter : children )
        {
            if ( iter->IsVisible())
            {
                iter->Update( dtt );
            }
        }
    }
}

bool CNode::HitTest(float x, float y) const
{
    return false;
}

bool CNode::OnTouchEvent(CTouchEvent *event)
{
    bool bRet = false;
    for (size_t i = 0; i < m_nodeChildren.size(); ++i)
    {
        if (m_nodeChildren[i]->OnTouchEvent(event))
        {
            event->StopPropagation();
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool CNode::HandleTouchEvent(CTouchEvent* /*event*/)
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
    OnWorldTransformUpdate();
    for (size_t i = 0; i < m_nodeChildren.size(); ++i)
    {
        m_nodeChildren[i]->InvalidateWorldTM();
    }
}

const kmMat4& CNode::GetLocalTM()
{
    if (m_bLocalTransformInvalid)
    {
        CVec3 scaleVec = m_scale;
        CVec3 rotationVec = m_rotation;
        CVec3 posVec = m_pos;

        if (m_pAnimationProperty != NULL)
        {
            scaleVec.x *= m_pAnimationProperty->m_scaleForAnimation.x;
            scaleVec.y *= m_pAnimationProperty->m_scaleForAnimation.y;
            scaleVec.z *= m_pAnimationProperty->m_scaleForAnimation.z;

            rotationVec.x += m_pAnimationProperty->m_rotationForAnimation.x;
            rotationVec.y += m_pAnimationProperty->m_rotationForAnimation.y;
            rotationVec.z += m_pAnimationProperty->m_rotationForAnimation.z;

            posVec.x += m_pAnimationProperty->m_posForAnimation.x;
            posVec.y += m_pAnimationProperty->m_posForAnimation.y;
            posVec.z += m_pAnimationProperty->m_posForAnimation.z;
        }
        CMat4 scaleMatrix;
        scaleMatrix.Scaling(scaleVec.x, scaleVec.y, scaleVec.z);
        CMat4 rotationMatrix;
        rotationMatrix.RotationPitchYawRoll(kmDegreesToRadians(rotationVec.x), kmDegreesToRadians(rotationVec.y), kmDegreesToRadians(rotationVec.z));
        CMat4 transMatrix;
        transMatrix.Translation(posVec.x, posVec.y, posVec.z);
        m_mat4LocalTransform = transMatrix * (rotationMatrix * scaleMatrix);
        m_bLocalTransformInvalid = false;
    }
    return m_mat4LocalTransform;
}

void CNode::SetLocalTM(const kmMat4& localTM)
{
    kmMat4Assign(&m_mat4LocalTransform, &localTM);
    m_bLocalTransformInvalid = false;
    InvalidateWorldTM();
}

const kmMat4& CNode::GetWorldTM()
{
    const kmMat4* pValue = &m_mat4WorldTransform;
    if (m_pParentNode == NULL)
    {
        pValue = &GetLocalTM();
    }
    else
    {
        if (m_bWorldTransformInvalid)
        {
            kmMat4Multiply(&m_mat4WorldTransform, &m_pParentNode->GetWorldTM(), &GetLocalTM());
            m_bWorldTransformInvalid = false;
        }
    }
    return *pValue;
}

void CNode::OnWorldTransformUpdate()
{
    // Do nothing
}

SAnimationProperty* CNode::GetAnimationProperty() const
{
    return m_pAnimationProperty;
}

void CNode::SetAnimationProperty(SAnimationProperty* pProperty)
{
    m_pAnimationProperty = pProperty;
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
            ret.a = (unsigned char)(ret.a * (inheritColor.a * 0.01f));
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
}

void CNode::DoRender()
{
}

void CNode::PostRender()
{
    const std::vector<CNode*>& children = GetChildren();
    for (int i = children.size() - 1; i >=0; --i)
    {
        children[i]->Render();
    }
}

bool CNode::HasChild( CNode* pNode )
{
    return std::find(m_nodeChildren.begin(), m_nodeChildren.end(), pNode) != m_nodeChildren.end();
}

CNode* CNode::CloneNode(bool bResurcive, bool bOnlyCloneComponent, std::vector<CNode*>* pNewNodes, std::vector<CNode*>* pClonedNodes)
{
    CNode* pRet = NULL;
    if (!bOnlyCloneComponent || GetId() != 0xFFFFFFFF)
    {
        pRet = (CNode*)CloneInstance();
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
            for (size_t i = 0; i < m_nodeChildren.size(); ++i)
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