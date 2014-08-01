#include "stdafx.h"
#include "Control.h"
#include "GUI/WindowManager.h"
#include "Event/TouchEvent.h"
#include "Event/Touch.h"
#include "Render/RenderManager.h"
#include "Render/RenderGroupManager.h"
#include "Render/CommonTypes.h"
#include "Render/RenderGroup.h"
#include "Render/VertexFormat.h"
#include "Render/RenderBatch.h"
#include "Render/RenderTarget.h"
#include "GUI/Event/WindowEvent.h"
#include "Render/RenderWindow.h"
#include "NodeAnimation/NodeAnimation.h"
#include "NodeAnimation/NodeAnimationManager.h"

int CControl::iIndex = 0;
#define CONTROLMINSCALE 0.15f

CControl::CControl()
    : m_strDefaultName( _T("Control"))
    , m_vec2Size( CVec2( 200.0f, 100.0f ))
    , m_color( CColor( 0x000000FF ))
    , m_vecRealSize( CVec2( 10.0f, 10.0f ) )
    , m_bUseInheritColorScale( true )
    , m_fTargetWidth( 0.0f )
    , m_fTargetHeight( 0.0f )
    , m_bIsClip( false )
    , m_bScaleAnimation( false )
    , m_bFoucs( false )
    , m_bClick( false )
    , m_bRelease( false)
    , m_bEnable( true )
    , m_bRoot(false)
    , m_fButtonScale( 0.0f )
    , m_bTurnScale( false)
    , m_bResponseClickEvent( true )
    , m_bResiveTouchEvent( true )
    , m_bDispatchEventToParent( false )
    , m_bTouchInRect( false )
    , m_uLayerID( GUI_LAYER_ID )
    , m_pUserData( nullptr )
    , m_pClickAction( nullptr )
    , m_pClickedAction( nullptr )
    , m_pPressAnimation( nullptr )
    , m_pReleaseAnimation( nullptr )
{
    m_strName = DEFAULT_CONTROL_NAME;
    m_colorInherit = m_color;

    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("Control"), iIndex );
    m_strDefaultName = szName;
    iIndex++;

#ifdef EDITOR_MODE
    m_bEditorSelect = false;
#endif
}

CControl::~CControl()
{
    BEATS_SAFE_DELETE(m_pPressAnimation);
    BEATS_SAFE_DELETE(m_pReleaseAnimation);
}

void CControl::OnParentSizeChange( float width, float height )
{
    m_fTargetWidth = width;
    m_fTargetHeight = height;

    CVec2 position;
    CalcRealPosition( position );
    SetPosition( position.x, position.y, 0.0f );

    CalcRealSize();

    UpdateQuadP();
    for ( auto childWnd : GetChildren() )
    {
        if( childWnd->GetType() == eNT_NodeGUI )
        {
            CControl* child = down_cast<CControl*>(childWnd);
            child->OnParentSizeChange( m_vecRealSize.x, m_vecRealSize.y );
        }
    }
}

void CControl::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );

    DECLARE_PROPERTY( serializer, m_vec2Anchor, true, 0xFFFFFFFF, _T("锚点"), NULL, NULL, 
    _T("SpinStep:0.1f, MinValue:0.f, MaxValue:1.f") );
    DECLARE_PROPERTY( serializer, m_vec2AbsolutePosition, true, 0xFFFFFFFF, _T("绝对位置"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_vec2PercentPosition, true, 0xFFFFFFFF, _T("百分比位置"), NULL, NULL,
    _T("SpinStep:0.1f, MinValue:-1.f, MaxValue:1.f") );
    DECLARE_PROPERTY( serializer, m_vec2Size, true, 0xFFFFFFFF, _T("尺寸"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_vec2PercentSize, true, 0xFFFFFFFF, _T("百分比尺寸"), NULL, NULL, 
    _T("SpinStep:0.1f, MinValue:0.f, MaxValue:1.f") );
    DECLARE_PROPERTY( serializer, m_color, true, 0xFFFFFFFF, _T("颜色"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bUseInheritColorScale, true, 0xFFFFFFFF, _T("颜色继承"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bResiveTouchEvent, true, 0xFFFFFFFF, _T("接收触摸事件"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bResponseClickEvent, true, 0xFFFFFFFF, _T("响应点击事件"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bDispatchEventToParent, true, 0xFFFFFFFF, _T("事件传递到父级"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bEnable, true, 0xFFFFFFFF, _T("是否可用"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_strPressAnimationName, true, 0xFFFFFFFF, _T("按下动画"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strReleaseAimationName, true, 0xFFFFFFFF, _T("弹起动画"), NULL, NULL, NULL);
    DECLARE_DEPENDENCY( serializer, m_pClickAction, _T("点击事件"), eDT_Weak );
    DECLARE_DEPENDENCY( serializer, m_pClickedAction, _T("点击弹起事件"), eDT_Weak );
    HIDE_PROPERTY(m_pos);
}

void CControl::SetName( const TString& strName )
{
    CControl* pControl = CWindowManager::GetInstance()->GetControl( strName );
    BEATS_ASSERT( nullptr == pControl );
    if ( nullptr == pControl )
    {
        CWindowManager::GetInstance()->UnregisterControl( this );
        super::SetName( strName );
        CWindowManager::GetInstance()->RegisterControl( this );
    }
}

void CControl::Initialize()
{
    super::Initialize();
    m_bUninitialize = false;
    if ( strcmp( m_strName.c_str(), DEFAULT_CONTROL_NAME ) == 0)
    {
        SetName( GetDefaultName() );
    }
    else
    {
        CWindowManager::GetInstance()->RegisterControl( this );
    }
    CalcInheritColor();
    CRenderTarget* pMainRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT( pMainRenderTarget );
    CNode* pParentNode = GetParentNode();
    if ( nullptr == pParentNode)
    {
        CWindowManager::GetInstance()->AddToRoot( this );
    }
    if (nullptr == pParentNode || pParentNode->GetType() != eNT_NodeGUI )
    {
        //update control vertex by render target
        OnParentSizeChange( (float)pMainRenderTarget->GetWidth(), (float)pMainRenderTarget->GetHeight());
    }
}

bool CControl::OnTouchEvent( CTouchEvent* event )
{
    bool bRet = false;
    for ( auto child : GetChildren() )
    {
        if ( child->IsVisible() && child->OnTouchEvent(event) )
        {
            bRet = true;
            break;
        }
    }
    if ( !bRet )
    {
        bRet = HandleTouchEvent( event );
    }
    if ( bRet )
    {
        event->StopPropagation();
    }
    return bRet;
}

bool CControl::HitTest( float x, float y )
{
    CVec2 leftTopConner;
    GetCorner( eCP_TopLeft, leftTopConner.x, leftTopConner.y);
    CVec2 rightBottom;
    GetCorner( eCP_BottomRight, rightBottom.x, rightBottom.y );
    bool bRet = x > leftTopConner.x && x < rightBottom.x 
        && y > leftTopConner.y && y < rightBottom.y;
    return bRet;
}


bool CControl::HandleTouchEvent( CTouchEvent* event )
{
    bool bRet = false;
    if ( m_bResiveTouchEvent )
    {
        const CVec2& pos = event->GetTouchPoint();
        if ( event->GetType() == eET_EVENT_TOUCH_MOVED )
        {
            m_bTouchInRect = HitTest(pos.x, pos.y);
            if ( m_bTouchInRect )
            {
                OnTouchMove( pos.x, pos.y );
                bRet = true;
            }
            else
            {
                if ( m_bClick )
                {
                    StopClickAnimation();
                }
            }
        }
        else
        {
            if( event->GetType() == eET_EVENT_TOUCH_ENDED && m_bClick )
            {
                OnTouchEnded( pos.x, pos.y );
            }
            else
            {
                bRet = HitTest(pos.x, pos.y);
                if(bRet)
                {
                    if ( m_bDispatchEventToParent )
                    {
                        CNode* pParent = GetParentNode();
                        bRet = pParent && pParent->GetType() == eNT_NodeGUI;
                        if (bRet)
                        {
                            down_cast<CControl*>(pParent)->DispatchEvent( event->GetType(), pos.x, pos.y, event->GetDelta() );
                        }
                    }
                    else
                    {
                        DispatchEvent( event->GetType(), pos.x, pos.y, event->GetDelta() );
                    }
                }
            }
        }
    }
    return bRet;
}

void CControl::GetCorner( ECornerPosition type, float& x, float& y )
{
    CVec3 corner;
    switch ( type )
    {
    case eCP_BottomLeft:
        corner = m_quadp.bl;
        break;
    case eCP_BottomRight:
        corner = m_quadp.br;
        break;
    case eCP_TopLeft:
        corner = m_quadp.tl;
        break;
    case eCP_TopRight:
        corner = m_quadp.tr;
        break;
    default:
        BEATS_ASSERT( false, _T("the error ECornerPosition") );
        break;
    }
    corner.Transform( GetWorldTM());
    x = corner.x;
    y = corner.y;
}

void CControl::GetAnchorPosition( float& x, float& y )
{
    x = GetAnchor().x * GetRealSize().x;
    y = GetAnchor().y * GetRealSize().y;
}

void CControl::DispatchEvent( EEventType type , float x, float y, int delta )
{
    switch ( type )
    {
    case eET_EVENT_TOUCH_BEGAN:
        OnTouchBegin( x, y );
        break;
    case eET_EVENT_TOUCH_ENDED:
        OnTouchEnded( x, y );
        break;
    case eET_EVENT_TOUCH_CANCELLED:
        OnTouchCancelled( x, y );
        break;
    case eET_EVENT_TOUCH_DOUBLE_TAPPED:
        OnTouchDoubleTapped( x, y );
        break;
    case eET_EVENT_TOUCH_SINGLE_TAPPED:
        OnTouchSingleTapped( x, y );
        break;
    case eET_EVENT_TOUCH_SCORLLED:
        OnTouchScorll( x, y, delta );
        break;
    case eET_EVENT_TOUCH_PINCHED:
        OnTouchSingleTapped( x, y );
        break;
    default:
        BEATS_ASSERT( false, _T("unknow event type!!") );
        break;
    }
}


void CControl::OnTouchBegin( float x, float y )
{
    m_bTouchInRect = true;
    CWindowManager::GetInstance()->SetFocusControl( this );
    m_bClick = true;
    if (!m_strPressAnimationName.empty())
    {
        if ( nullptr == m_pPressAnimation )
        {
            m_pPressAnimation = CreateNodeAnimation( m_strPressAnimationName );
            BEATS_ASSERT( m_pPressAnimation );
            m_pPressAnimation->GetEventDispatcher()->SubscribeEvent(eET_EVENT_NODE_ANIMATION_FINISHED, &CControl::PressAnimationFinishCallBack, this);
        }
        m_pPressAnimation->GetEventDispatcher()->SetEnable( m_bResponseClickEvent && m_bEnable );
        m_pPressAnimation->SetPlayType( eNAPT_ONCE );
        m_pPressAnimation->Play();
    }
    else
    {
        if ( m_bResponseClickEvent && m_bEnable )
        {
            OnClickBegin();
        }
    }
}

void CControl::OnTouchMove( float x, float y )
{

}

void CControl::OnTouchEnded( float x, float y )
{
    if ( m_bClick )
    {
        m_bClick = false;
        m_bTouchInRect = false;
        m_bRelease = true;
        if ( !m_strReleaseAimationName.empty() )
        {
            if ( nullptr == m_pReleaseAnimation )
            {
                m_pReleaseAnimation = CreateNodeAnimation( m_strReleaseAimationName );
                BEATS_ASSERT( m_pReleaseAnimation );
                m_pReleaseAnimation->GetEventDispatcher()->SubscribeEvent(eET_EVENT_NODE_ANIMATION_FINISHED, &CControl::ReleaseAnimationFinishCallBack, this);
            }
            m_pReleaseAnimation->GetEventDispatcher()->SetEnable( m_bResponseClickEvent && m_bEnable );
            m_pReleaseAnimation->SetPlayType( eNAPT_ONCE );
            m_pReleaseAnimation->Play();
        }
        else
        {
            if ( m_bResponseClickEvent && m_bEnable )
            {
                OnClickEnd();
            }
        }
    }
}

void CControl::OnTouchCancelled( float x, float y )
{
    OnTouchEnded( x, y );
}

void CControl::OnTouchDoubleTapped( float x, float y )
{

}

void CControl::OnTouchSingleTapped( float x, float y )
{

}

void CControl::OnTouchScorll( float x, float y, int delta )
{

}

void CControl::OnTouchPinched( float x, float y )
{

}

void CControl::SetColor( const CColor& color )
{
    if( color != m_color )
    {
        m_color = color;
        CalcInheritColor();
        for ( auto child : GetChildren() )
        {
            if( child->GetType() == eNT_NodeGUI )
            {
                down_cast<CControl*>(child)->CalcInheritColor();
            }
        }
    }
}

void CControl::CalcInheritColor()
{
    CNode* pParent = GetParentNode();
    if ( pParent && pParent->GetType() == eNT_NodeGUI )
    {
        CControl* m_pParentNode = (CControl*)pParent;
        CColor inheritColor = m_pParentNode->GetColor( false );
        m_colorInherit.r = (unsigned char)(m_color.r * 0.5f + inheritColor.r * 0.5f);
        m_colorInherit.g = (unsigned char)(m_color.g * 0.5f + inheritColor.g * 0.5f);
        m_colorInherit.b = (unsigned char)(m_color.b * 0.5f + inheritColor.b * 0.5f);
        m_colorInherit.a = (unsigned char)(m_color.a * 0.5f + inheritColor.a * 0.5f);
    }
    else
    {
        m_colorInherit = m_color;
    }
}

bool CControl::IsInheritColorScale() const
{
    return m_bUseInheritColorScale;
}

void CControl::CalcRealPosition( CVec2& relPosition )
{
    relPosition.x = m_fTargetWidth * m_vec2PercentPosition.x + m_vec2AbsolutePosition.x;
    relPosition.y = m_fTargetHeight * m_vec2PercentPosition.y + m_vec2AbsolutePosition.y;
}

const CColor& CControl::GetColor( bool bInherit ) const
{
    return bInherit ? m_colorInherit : m_color;
}

void CControl::SetAnchor( const CVec2& anchor )
{
    if ( anchor != m_vec2Anchor )
    {
        m_vec2Anchor = anchor;
        UpdateQuadP();
    }
}

void CControl::SetAbsolutePosition( const CVec2& position )
{
    if ( m_vec2AbsolutePosition != position )
    {
        m_vec2AbsolutePosition = position;
        CVec2 position;
        CalcRealPosition( position );
        SetPosition( position.x ,position.y, 0.0f );
    }
}

const CVec2& CControl::GetAbsolutePosition() const
{
    return m_vec2AbsolutePosition;
}

void CControl::SetPercentPosition( const CVec2& position )
{
    if ( position != m_vec2PercentPosition)
    {
        m_vec2PercentPosition = position;
        CVec2 position;
        CalcRealPosition( position );
        SetPosition( position.x ,position.y, 0.0f );
    }
}

void CControl::SetSize( const CVec2& size )
{
    if ( m_vec2Size != size )
    {
        m_vec2Size = size;
        CalcRealSize();
        UpdateQuadP();
        for ( auto childWnd : GetChildren() )
        {
            if ( childWnd->GetType() == eNT_NodeGUI )
            {
                CControl* child =( CControl*) childWnd;
                child->OnParentSizeChange( m_vec2Size.x , m_vec2Size.y );
            }
        }
    }
}

void CControl::SetPercentSize( const CVec2& size )
{
    if ( m_vec2PercentSize != size )
    {
        m_vec2PercentSize = size;
        CalcRealSize();
        UpdateQuadP();
        for ( auto childWnd : GetChildren() )
        {
            if ( childWnd->GetType() == eNT_NodeGUI )
            {
                CControl* child =( CControl*) childWnd;
                child->OnParentSizeChange( m_vec2Size.x , m_vec2Size.y );
            }
        }
    }
}

void CControl::CalcRealSize()
{
    m_vecRealSize.x = m_vec2Size.x + m_vec2PercentSize.x * m_fTargetWidth;
    m_vecRealSize.y = m_vec2Size.y + m_vec2PercentSize.y * m_fTargetHeight;
}

bool CControl::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_vec2AbsolutePosition == pVariableAddr )
        {
            CVec2 absolutePosition;
            DeserializeVariable( absolutePosition, pSerializer );
            SetAbsolutePosition( absolutePosition );
            bRet = true;
        }
        else if ( &m_vec2PercentPosition == pVariableAddr )
        {
            CVec2 percentPosition;
            DeserializeVariable( percentPosition, pSerializer );
            SetPercentPosition( percentPosition );
            bRet = true;
        }
        else if ( &m_vec2PercentSize == pVariableAddr )
        {
            CVec2 percentSize;
            DeserializeVariable( percentSize, pSerializer );
            SetPercentSize( percentSize );
            bRet = true;
        }
        else if( &m_vec2Size == pVariableAddr )
        {
            CVec2 size;
            DeserializeVariable( size, pSerializer );
            SetSize( size );
            bRet = true;
        }
        else if( &m_vec2Anchor == pVariableAddr )
        {
            CVec2 anchor;
            DeserializeVariable( anchor, pSerializer );
            SetAnchor( anchor );
            bRet = true;
        }
        else if( &m_color == pVariableAddr )
        {
            CColor color;
            DeserializeVariable( color, pSerializer );
            SetColor( color );
            bRet = true;
        }
        else if( &m_strPressAnimationName == pVariableAddr )
        {
            TString animationName;
            DeserializeVariable( animationName, pSerializer );
            SetAnimationName( animationName );
            bRet = true;
        }
    }
    return bRet;
}

void CControl::SetParentNode( CNode* pParent )
{
    super::SetParentNode( pParent );
    if( pParent )
    {
        if ( GetRootFlag() )
        {
            CWindowManager::GetInstance()->RemoveFromRoot( this );
        }
        if ( pParent->GetType() == eNT_NodeGUI )
        {
            CControl* pControl = (CControl*)pParent;
            SetLayerId( pControl->GetLayerID() + 1);
            CVec2 size = pControl->GetRealSize();
            OnParentSizeChange( size.x, size.y );
        }
        else
        {
            CWindowManager::GetInstance()->AttachToNode( this );
        }
    }
    else
    {
        if ( !m_bUninitialize )
        {
            SetLayerId( GUI_LAYER_ID);
            CRenderTarget* pMainRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
            BEATS_ASSERT( pMainRenderTarget );
            CWindowManager::GetInstance()->AddToRoot( this );
            CWindowManager::GetInstance()->DetachToNode( this );
            float fWidth = (float)pMainRenderTarget->GetWidth();
            float fHeight = (float)pMainRenderTarget->GetHeight();
            OnParentSizeChange( fWidth, fHeight );
        }
    }    
}

void CControl::DoRender()
{
#ifdef EDITOR_MODE
    if( IsEditorSelect())
    {
        CWindowManager::GetInstance()->RenderEditorSelect( &m_quadp , GetWorldTM() );
    }
#endif
}

const TString& CControl::GetDefaultName() const
{
    return m_strDefaultName;
}

const CVec2& CControl::GetRealSize() const
{
    return m_vecRealSize;
}

const CVec2& CControl::GetAnchor() const
{
    return m_vec2Anchor;
}

void CControl::Uninitialize()
{
    super::Uninitialize();
    m_bUninitialize = true;
    CWindowManager::GetInstance()->LogoutControl( this );
    // TODO: There is a worse solution, wait for the frame fix
    // bug describe
    // If delete the component , the child didn't render again
    for ( auto child : GetChildren() )
    {
        CControl* control = down_cast<CControl*>(child);
        if ( control && control->GetRootFlag() )
        {
            CWindowManager::GetInstance()->RemoveFromRoot( control );
        }
    }
}

void CControl::OnClickBegin()
{
    CWindowEvent event( eET_EVENT_MOUSE_CLICK , this );
    CWindowManager::GetInstance()->DispatchEventToRecipient( this, &event);
    if (m_pClickAction != NULL)
    {
        SActionContext context;
        context.m_pUserData = this;
        m_pClickAction->Execute(&context);
    }
}

void CControl::OnClickEnd()
{
    CWindowEvent event( eET_EVENT_MOUSE_CLICKED , this );
    CWindowManager::GetInstance()->DispatchEventToRecipient( this, &event);
    if (m_pClickedAction != NULL)
    {
        SActionContext context;
        context.m_pUserData = this;
        m_pClickedAction->Execute(&context);
    }
}

void CControl::SetResponseClickEvent( bool bResponse )
{
    m_bResponseClickEvent = bResponse;
}

bool CControl::IsResponseClickEvent() const
{
    return m_bResponseClickEvent;
}

bool CControl::IsEnable() const
{
    return m_bEnable;
}

void CControl::SetEnable( bool bEnable )
{
    m_bEnable = bEnable;
}

void CControl::UpdateQuadP()
{
    const CVec2& realSize = GetRealSize();
    CVec2 anchorPos = GetAnchor() * realSize;

    //set vertex
    m_quadp.tl = CVec3(-anchorPos);
    m_quadp.br = CVec3(realSize - anchorPos);
    m_quadp.tr.x = m_quadp.br.x;
    m_quadp.tr.y = m_quadp.tl.y;
    m_quadp.bl.x = m_quadp.tl.x;
    m_quadp.bl.y = m_quadp.br.y;
}

const CQuadP& CControl::GetQuadP() const
{
    return m_quadp;
}

size_t CControl::GetLayerID() const
{
    return m_uLayerID;
}

void CControl::SetLayerId( size_t layerid )
{
    m_uLayerID = layerid;
    for ( auto child : GetChildren() )
    {
        CControl* pChild = ( CControl*) child;
        if ( pChild )
        {
            pChild->SetLayerId( m_uLayerID + 1 );
        }
    }
}

void CControl::SetUserData( void* pData )
{
    m_pUserData = pData;
}

void* CControl::GetUserData() const
{
    return m_pUserData;
}

void CControl::SetEventDispatchToParent( bool bDispatch )
{
    m_bDispatchEventToParent = bDispatch;
}

void CControl::SetClip( bool bClip , const CRect& rect )
{
    m_bIsClip = bClip;
    m_rectClip = rect;
    //Y axis is bottom-up in OpenGL
    CRenderTarget* pTarget= CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pTarget != NULL);
    m_rectClip.position.y = pTarget->GetHeight() -
        m_rectClip.position.y - m_rectClip.size.y;
    float fScale = pTarget->GetScaleFactor();
    m_rectClip.position *= fScale;
    m_rectClip.size *= fScale;
    for ( auto child : GetChildren() )
    {
        CControl* pChild = (CControl*)child;
        pChild->SetClip( bClip, rect );
    }
}

const CRect& CControl::GetClipRect() const
{
    return m_rectClip;
}

bool CControl::IsClip()
{
    return m_bIsClip;
}

void CControl::StopClickAnimation()
{
    m_bClick = false;
    m_bRelease = false;
    if ( m_pPressAnimation )
    {
        m_pPressAnimation->GetEventDispatcher()->SetEnable( false );
        m_pPressAnimation->SetPlayType( eNAPT_REVERSE );
        m_pPressAnimation->Play();
    }
}

void CControl::SetFocus( bool bFocus )
{
    m_bFoucs = bFocus;
}

bool CControl::IsFoucus() const
{
    return m_bFoucs;
}

void CControl::LocalToWorld(float &x, float &y)
{
    LocalToParent(x, y);

    CNode* pParent = GetParentNode();
    if( pParent && pParent->GetType() == eNT_NodeGUI )
    {
        down_cast<CControl*>(pParent)->LocalToWorld(x, y);
    }
}

void CControl::LocalToParent(float &x, float &y)
{
    CVec3 pos(x, y, 0.f);
    pos.Transform(GetLocalTM());
    x = pos.x;
    y = pos.y;
}

void CControl::WorldToLocal(float &x, float &y)
{
    CNode* pParent = GetParentNode();
    if( pParent && pParent->GetType() == eNT_NodeGUI )
    {
       down_cast<CControl*>(pParent)->WorldToLocal(x, y);
    }
    ParentToLocal(x, y);
}

void CControl::ParentToLocal(float &x, float &y)
{
    CVec3 pos(x, y, 0.f);
    kmMat4 inverse;
    kmMat4Inverse(&inverse, &GetLocalTM());
    pos.Transform(inverse);
    x = pos.x;
    y = pos.y;
}

void CControl::SetRootFlag( bool bRoot )
{
    m_bRoot = bRoot;
}

bool CControl::GetRootFlag() const
{
    return m_bRoot;
}

void CControl::SetAnimationName( const TString& animationName )
{
    m_strPressAnimationName = animationName;
    if (!animationName.empty() && m_pPressAnimation != NULL)
    {
        CNodeAnimationData* pData = CNodeAnimationManager::GetInstance()->GetNodeAnimationData(animationName);
        BEATS_ASSERT(pData != NULL);
        m_pPressAnimation->SetData(pData);
    }
}

void CControl::PressAnimationFinishCallBack( CBaseEvent* event )
{
    OnClickBegin();
}

void CControl::ReleaseAnimationFinishCallBack( CBaseEvent* event )
{
    OnClickEnd();
}

CNodeAnimation* CControl::CreateNodeAnimation( const TString& animationName )
{
    CNodeAnimation* pAnimation = new CNodeAnimation;
    pAnimation->SetOwner(this);
    pAnimation->SetFPS(CEngineCenter::GetInstance()->GetFPS());
    CNodeAnimationData* pData = CNodeAnimationManager::GetInstance()->GetNodeAnimationData(animationName);
    BEATS_ASSERT(pData != NULL);
    pAnimation->SetData(pData);
    return pAnimation;
}

#ifdef EDITOR_MODE
CVec2 CControl::CalcPosFromMouse(float x, float y) const
{
    CVec2 pos(x, y);
    CNode* pParent = GetParentNode();
    if( pParent && pParent->GetType() == eNT_NodeGUI )
    {
        ((CControl*)pParent)->WorldToLocal(x, y);
        pos.x = x - ((CControl*)pParent)->GetRealSize().x * m_vec2PercentPosition.x;
        pos.y = y - ((CControl*)pParent)->GetRealSize().y * m_vec2PercentPosition.y;
        CVec3 paretnPosition = pParent->GetPosition();
        ((CControl*)pParent)->LocalToWorld( paretnPosition.x, paretnPosition.y );
        CVec2 parentRealAnchor( paretnPosition.x, paretnPosition.y );
        pos.x += parentRealAnchor.x;
        pos.y += parentRealAnchor.y;
    }
    return pos;
}

CVec2 CControl::CalcSizeFromMouse(float x, float y, EHitTestResult hitPos)
{
    CVec2 anchor = m_vec2Anchor;
    WorldToLocal(x, y);

    CVec2 realSize = m_vecRealSize;
    CVec2 newRealSize = realSize;
    switch(hitPos)
    {
    case eHTR_HIT_TOP_LEFT:
        if(anchor.x > 0.f)
            newRealSize.x = - x / anchor.x;
        if(anchor.y > 0.f)
            newRealSize.y = - y / anchor.y;
        break;
    case eHTR_HIT_TOP_CENTER:
        newRealSize.x = realSize.x;
        if(anchor.y > 0.f)
            newRealSize.y = - y / anchor.y;
        break;
    case eHTR_HIT_TOP_RIGHT:
        if(anchor.x < 1.f)
            newRealSize.x = x / (1 - anchor.x);
        if(anchor.y > 0.f)
            newRealSize.y = - y / anchor.y;
        break;
    case eHTR_HIT_LEFT_CENTER:
        if(anchor.x > 0.f)
            newRealSize.x = - x / anchor.x;
        newRealSize.y = realSize.y;
        break;
    case eHTR_HIT_RIGHT_CENTER:
        if(anchor.x < 1.f)
            newRealSize.x = x / (1 - anchor.x);
        newRealSize.y = realSize.y;
        break;
    case eHTR_HIT_BOTTOM_LEFT:
        if(anchor.x > 0.f)
            newRealSize.x = - x / anchor.x;
        if(anchor.y < 1.f)
            newRealSize.y = y / (1 - anchor.y);
        break;
    case  eHTR_HIT_BOTTOM_CENTER:
        newRealSize.x = realSize.x;
        if(anchor.y < 1.f)
            newRealSize.y = y / (1 - anchor.y);
        break;
    case eHTR_HIT_BOTTOM_RIGHT:
        if(anchor.x < 1.f)
            newRealSize.x = x / (1 - anchor.x);
        if(anchor.y < 1.f)
            newRealSize.y = y / (1 - anchor.y);
        break;
    default:
        BEATS_ASSERT(false);
    }

    CVec2 newSize = newRealSize;
    CNode* pParent = GetParentNode();
    if( pParent && pParent->GetType() == eNT_NodeGUI )
    {
        CVec2 realSizePercentPart = m_vecRealSize - m_vec2Size;
        newSize = newRealSize - realSizePercentPart;
    }
    return newSize;
}
CControl::EHitTestResult CControl::GetHitResult( float x, float y )
{
    EHitTestResult result = eHTR_HIT_NONE;
    if( HitTest( x, y ))
    {
        CVec3 realPosition = GetPosition();
        LocalToWorld( realPosition.x, realPosition.y );
        static const float DEVIATION = 7.f;
        float toLeft = x - m_quadp.tl.x;
        float toRight = m_quadp.tl.x + m_vecRealSize.x - x;
        float toTop = y - m_quadp.tl.y;
        float toBottom = m_vecRealSize.y + m_quadp.tl.y - y;
        if(toLeft <= DEVIATION)
        {
            if(toTop <= DEVIATION)
            {
                result = EHitTestResult::eHTR_HIT_TOP_LEFT;
            }
            else if(toBottom <= DEVIATION)
            {
                result = eHTR_HIT_BOTTOM_LEFT;
            }
            else if(abs(toBottom - toTop) <= DEVIATION*2)
            {
                result = eHTR_HIT_LEFT_CENTER;
            }
        }
        else if(toRight <= DEVIATION)
        {
            if(toTop <= DEVIATION)
            {
                result = eHTR_HIT_TOP_RIGHT;
            }
            else if(toBottom <= DEVIATION)
            {
                result = eHTR_HIT_BOTTOM_RIGHT;
            }
            else if(abs(toBottom - toTop) <= DEVIATION*2)
            {
                result = eHTR_HIT_RIGHT_CENTER;
            }
        }
        else if(toTop <= DEVIATION)
        {
            if(abs(toRight - toLeft) <= DEVIATION*2)
            {
                result = eHTR_HIT_TOP_CENTER;
            }
        }
        else if(toBottom <= DEVIATION)
        {
            if(abs(toRight - toLeft) <= DEVIATION*2)
            {
                result = eHTR_HIT_BOTTOM_CENTER;
            }
        }
        else if(abs(toLeft - realPosition.x) <= DEVIATION*2
            &&  abs(toTop - realPosition.y) <= DEVIATION*2)
        {
            result = eHTR_HIT_ANCHOR;
        }

        if(result == eHTR_HIT_CONTENT)
        {
            if (toLeft <= DEVIATION ||
                toRight <= DEVIATION ||
                toTop <= DEVIATION ||
                toBottom <= DEVIATION)
            {
                result = eHTR_HIT_EDGE;
            }
        }
    }
    return result;
}

void CControl::SetEditorSelect( bool bSelect )
{
    m_bEditorSelect = bSelect;
}

bool CControl::IsEditorSelect() const
{
    return m_bEditorSelect;
}
#endif // EDITOR_MODE


