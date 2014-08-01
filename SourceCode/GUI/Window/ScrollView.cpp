#include "stdafx.h"
#include "ScrollView.h"
#include "GUI/WindowManager.h"
#include "Event/TouchEvent.h"
#include "Render/RenderManager.h"

#define MINMOVESCALE 5
#define MINIERTIADIS 2.0F
int CScrollView::nNum = 0;

CScrollView::CScrollView()
    : m_uItemCount( 0 )
    , m_fBackForce( 5.0f )
    , m_fInertiaForce( 15.0f )
    , m_uBackCount( 2 )
    , m_fItemWidth( 0.2f )
    , m_fItemOffset( 0.0f )
    , m_fItemSpace( 5.0f )
    , m_fMoveDelta( 0.0f )
    , m_eScrollDerictionType( eSD_Top_Bottom )
    , m_bBeginMove( false )
    , m_bTouch( false )
    , m_bSpringBack( false )
    , m_bInertia( false )
    , m_fBackDistance( 0.0f )
    , m_pClickItem( nullptr )
    , m_pScrollItem( nullptr )
{
    m_vec2Size = CVec2( 800.0f, 600.0f );
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"),_T("ScrollView"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CScrollView::~CScrollView()
{

}

void CScrollView::Initialize()
{
    super::Initialize();

    ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_pScrollItem)
    m_pScrollItem->SetScrollViewInitialzeFlag( true );
    if ( m_pScrollItem->GetRootFlag() )
    {
        CWindowManager::GetInstance()->RemoveFromRoot( m_pScrollItem );
    }
    for ( size_t i = 0; i < m_uItemCount; ++i)
    {
        CreateItem();
    }
    LayeroutItems();
    ASSUME_VARIABLE_IN_EDITOR_END
}

void CScrollView::Uninitialize()
{
    for ( auto item : m_Items )
    {
        CWindowManager::GetInstance()->DestoryControl( item );
    }
    super::Uninitialize();
}

void CScrollView::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_uItemCount, true, 0xFFFFFFFF, _T("滑动项个数"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_eScrollDerictionType, true, 0xFFFFFFFF, _T("排列方式"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fItemWidth, true, 0xFFFFFFFF, _T("项宽(高)百分比"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fItemOffset, true, 0xFFFFFFFF, _T("项宽(高)"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fItemSpace, true, 0xFFFFFFFF, _T("项间距"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fBackForce, true, 0xFFFFFFFF, _T("回弹力"), NULL, NULL, _T("MinValue:0") );
    DECLARE_PROPERTY( serializer, m_fInertiaForce, true, 0xFFFFFFFF, _T("惯性力"), NULL, NULL, _T("MinValue:0") );
    DECLARE_PROPERTY( serializer, m_uBackCount, true, 0xFFFFFFFF, _T("触发回弹距离"),NULL, NULL, NULL );
    DECLARE_DEPENDENCY( serializer, m_pScrollItem, _T("滑动区项"), eDT_Strong );
}

bool CScrollView::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_uItemCount == pVariableAddr )
        {
            size_t count;
            DeserializeVariable( count, pSerializer );
            SetItemCount( count );
            bRet = true;
        }
        else if( &m_eScrollDerictionType == pVariableAddr )
        {
            DeserializeVariable( m_eScrollDerictionType, pSerializer );
            LayeroutItems();
            bRet = true;
        }
        else if( &m_fItemWidth == pVariableAddr )
        {
            DeserializeVariable( m_fItemWidth, pSerializer );
            LayeroutItems();
            bRet = true;
        }
        else if( &m_fItemOffset == pVariableAddr )
        {
            DeserializeVariable( m_fItemOffset, pSerializer );
            LayeroutItems();
            bRet = true;
        }
        else if( &m_fItemSpace == pVariableAddr )
        {
            DeserializeVariable( m_fItemSpace, pSerializer );
            LayeroutItems();
            bRet = true;
        }
    }
    return bRet;
}

bool CScrollView::OnDependencyChange( void* pComponentAddr, CComponentBase* pComponent )
{
    bool bRet = super::OnDependencyChange( pComponentAddr, pComponent );
    if ( !bRet )
    {
        if ( &m_pScrollItem == pComponentAddr )
        {
            CScrollItem* pTempItem = m_pScrollItem;
            m_pScrollItem = down_cast<CScrollItem*>( pComponent );
            size_t size = m_Items.size();
            for ( size_t i = 0; i < size; ++i)
            {
                DeleteItem();
            }
            if ( m_pScrollItem )
            {
                if ( m_pScrollItem->GetRootFlag() )
                {
                    CWindowManager::GetInstance()->RemoveFromRoot( m_pScrollItem );
                }
                for ( size_t i = 0; i < m_uItemCount; ++i)
                {
                    CreateItem();
                }
                LayeroutItems();
            }
            else
            {
                if ( pTempItem )
                {
                    CWindowManager::GetInstance()->AddToRoot( pTempItem );
                }
            }
            bRet = true;
        }
    }
    return bRet;
}

void CScrollView::UpdateQuadP()
{
    super::UpdateQuadP();
    UpdateScissorRect();
}

void CScrollView::Update( float dtt )
{
    super::Update( dtt );
    if ( !m_bTouch )
    {
        if ( m_bInertia )
        {
            m_fMoveDelta *= (m_fInertiaForce * dtt);
            if ( BEATS_FLOAT_EQUAL( m_fMoveDelta, 0.0f ))
            {
                m_fMoveDelta = 0.0f;
                m_bInertia = false;
            }
            bool bCheck = m_fMoveDelta <= 0 ? CheckBack() : CheckFront();
            if( !bCheck )
            {
                m_bSpringBack = true;
                m_bInertia = false;
            }
            else
            {
                Scroll( m_fMoveDelta );
            }
        }
    }
    if ( m_bSpringBack && !m_bTouch )
    {
        SpringBack( dtt );
    }
}

void CScrollView::OnTouchBegin( float x, float y )
{
    m_bTouch = true;
    m_vec2StartPoint = CVec2( x, y );
    m_vec2LastPoint = m_vec2StartPoint;
    m_bSpringBack = false;
    for ( auto item : m_Items )
    {
        item->SetResponseClickEvent( false );
    }
}

void CScrollView::OnTouchEnded( float x, float y )
{
    m_bTouch = false;
    if( m_pClickItem )
    {
        if ( !m_bBeginMove )
        {
            m_pClickItem->SetResponseClickEvent( true );
        }
        else
        {
            m_pClickItem->StopClickAnimation();
        }
        m_pClickItem = nullptr;
    }
    m_bBeginMove = false;
    if( !CheckFront() || !CheckBack() )
    {
        m_bSpringBack = true;
        m_bInertia = false;
    }
}

void CScrollView::OnTouchMove( float x, float y )
{
    if ( m_bTouch )
    {
        m_vec2CurrentPoint = CVec2( x, y );
        CVec2 temp = m_vec2CurrentPoint - m_vec2LastPoint;
        m_fMoveDelta = m_eScrollDerictionType == eSD_Top_Bottom ? temp.y : temp.x;
        if ( !m_bBeginMove )
        {
            m_bBeginMove = ( m_fMoveDelta > MINMOVESCALE || m_fMoveDelta < MINMOVESCALE * -1.0f );
        }
        else
        {
            m_bSpringBack = m_fMoveDelta <= 0 ? !CheckBack() : !CheckFront();
            if( !m_bSpringBack )
            {
                Scroll( m_fMoveDelta );
                m_bInertia = fabs( m_fMoveDelta ) > MINIERTIADIS;
            }
        }
        m_vec2LastPoint = m_vec2CurrentPoint;
    }
}

CScrollItem* CScrollView::GetScrollItem( size_t index )
{
    BEATS_ASSERT( index < m_Items.size(), _T("didn't have the item of the index %d\n"), index );
    return m_Items[ index ];
}

CScrollItem* CScrollView::CreateItem()
{
    BEATS_ASSERT( m_pScrollItem , _T("please dependce the item first "));
    CScrollItem* pItem = m_pScrollItem->CloneItem();
    AddChild( pItem );
    pItem->SetResponseClickEvent( false );
    m_Items.push_back( pItem );
    return pItem;
}

void CScrollView::DeleteItem()
{
    BEATS_ASSERT( !m_Items.empty() );
    auto iter = m_Items.end() - 1;
    RemoveChild( *iter );
    CWindowManager::GetInstance()->DestoryControl( *iter );
    m_Items.erase( iter );
}

void CScrollView::LayeroutItems()
{
    float fItemSize = m_vecRealSize.x * m_fItemWidth + m_fItemOffset;
    bool bVertical = m_eScrollDerictionType == eSD_Top_Bottom;
    CVec2 percentSize = bVertical ? CVec2( 1.0f, 0.0f ) : CVec2( 0.0f, 1.0f );
    CVec2 size = bVertical ? CVec2(0.0f, fItemSize ) : CVec2(fItemSize, 0.0f );
    CVec2 percentPosition = CVec2(0, 0);
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        m_Items[ i ]->SetPercentSize(percentSize);
        m_Items[ i ]->SetSize(size);
        m_Items[ i ]->SetPercentPosition(percentPosition);
        const CVec2& anchor = m_Items[ i ]->GetAnchor();
        if (bVertical)
        {
            m_Items[ i ]->SetAbsolutePosition(CVec2( anchor.x * m_vecRealSize.x , i * (fItemSize + m_fItemSpace) + anchor.y * fItemSize ));
        }
        else
        {
            m_Items[ i ]->SetAbsolutePosition(CVec2( i * (fItemSize + m_fItemSpace) + anchor.x * fItemSize, anchor.y * m_vecRealSize.y ) );
        }
    }
    UpdateQuadP();
}

void CScrollView::Scroll( float distance )
{
    for ( size_t i = 0; i < m_Items.size(); ++i )
    {
        CVec2 positon = m_Items[ i ]->GetAbsolutePosition();
        m_eScrollDerictionType == eSD_Top_Bottom ? positon.y += distance : positon.x += distance;
        m_Items[ i ]->SetAbsolutePosition( CVec2(positon.x, positon.y)); 
    }
    UpdateItemVisibility();
}

bool CScrollView::CheckBack()
{
    bool bRet = true;
    CVec2 rightBottomCorner;
    GetCorner( eCP_BottomRight, rightBottomCorner.x, rightBottomCorner.y );
    auto itemBack = m_Items.rbegin();
    CVec2 itemRBCorner;
    (*itemBack)->GetCorner( eCP_BottomRight, itemRBCorner.x, itemRBCorner.y);
    BEATS_ASSERT(m_eScrollDerictionType == eSD_Top_Bottom || m_eScrollDerictionType == eSD_Left_Right);
    bool bVerticalMove = m_eScrollDerictionType == eSD_Top_Bottom;
    float fCurrValue = bVerticalMove ? itemRBCorner.y : itemRBCorner.x;
    float fCheckValue = bVerticalMove ? rightBottomCorner.y :rightBottomCorner.x;
    if (m_bBeginMove)
    {
        fCheckValue = bVerticalMove ? rightBottomCorner.y - m_uBackCount * (*itemBack)->GetRealSize().y :
                                    rightBottomCorner.x - m_uBackCount * (*itemBack)->GetRealSize().x;
    }
    if (fCurrValue < fCheckValue)
    {
        m_fBackDistance = bVerticalMove ? rightBottomCorner.y - fCurrValue : rightBottomCorner.x - fCurrValue;
        bRet = false;
    }
    return bRet;
}

bool CScrollView::CheckFront()
{
    bool bRet = true;
    CVec2 leftTopCorner;
    GetCorner( eCP_TopLeft, leftTopCorner.x, leftTopCorner.y );
    auto itemFront = m_Items.begin();
    CVec2 itemLTCorner;
    (*itemFront)->GetCorner( eCP_TopLeft, itemLTCorner.x, itemLTCorner.y);
    bool bVerticalMove = m_eScrollDerictionType == eSD_Top_Bottom;
    float fCurrentValue = bVerticalMove ? itemLTCorner.y - leftTopCorner.y : itemLTCorner.x - leftTopCorner.x;
    float fCheckValue = 0.0f;
    if ( m_bBeginMove )
    {
        fCheckValue = bVerticalMove ? m_uBackCount * (*itemFront)->GetRealSize().y : m_uBackCount * (*itemFront)->GetRealSize().x;
    }
    if ( fCurrentValue > fCheckValue )
    {
        m_fBackDistance = -fCurrentValue;
        bRet = false;
    }
    return bRet;
}

void CScrollView::SpringBack( float dtt )
{
    float scroll = m_fBackDistance * (m_fBackForce * dtt);
    m_fBackDistance = m_fBackDistance - scroll;
    Scroll( scroll );
    if ( BEATS_FLOAT_EQUAL( m_fBackDistance, 0.0f ) )
    {
        m_fBackDistance = 0.0f;
        m_bSpringBack = false;
    }
}

void CScrollView::OnWorldTransformUpdate()
{
    super::OnWorldTransformUpdate();
    UpdateScissorRect();
}

bool CScrollView::OnTouchEvent( CTouchEvent* event )
{
    bool bRet = HandleTouchEvent( event );
    if ( bRet )
    {
        for ( auto item : m_Items )
        {
            bRet = item->OnTouchEvent( event );
            if ( bRet )
            {
                if( event->GetType() == eET_EVENT_TOUCH_BEGAN )
                {
                    m_pClickItem = item;
                }
                event->StopPropagation();
                break;
            }
        }
    }
    return bRet;
}

void CScrollView::SetItemCount( size_t count )
{
    if ( count != m_uItemCount )
    {
        if ( count > m_uItemCount )
        {
            for ( size_t i = 0; i < count - m_uItemCount; ++i )
            {
                CreateItem();
            }
        }
        else if( m_uItemCount > count )
        {
            for ( size_t i = 0; i < m_uItemCount - count; ++i )
            {
                DeleteItem();
            }
        }
        LayeroutItems();
        m_uItemCount = count;
    }
}

void CScrollView::OnParentSizeChange( float width, float height )
{
    super::OnParentSizeChange( width, height );
    LayeroutItems();
    UpdateScissorRect();
}

void CScrollView::UpdateItemVisibility()
{
    CVec2 rightCorner;
    GetCorner( eCP_BottomRight, rightCorner.x, rightCorner.y );
    for ( auto pItem : m_Items )
    {
        bool bRet = true;
        CVec2 itemLeftCorner;
        pItem->GetCorner( eCP_TopLeft, itemLeftCorner.x, itemLeftCorner.y );
        CVec2 itemRightCorner;
        pItem->GetCorner( eCP_BottomRight, itemRightCorner.x , itemRightCorner.y );
        switch ( m_eScrollDerictionType )
        {
        case eSD_Left_Right:
            {
                bRet = itemRightCorner.x >= 0.0f && itemLeftCorner.x <= rightCorner.x;
            }
            break;
        case eSD_Top_Bottom:
            {
                bRet = itemRightCorner.y >= 0 && itemLeftCorner.y <= rightCorner.y;
            }
            break;
        default:
            BEATS_ASSERT( false, _T("Bad type"));
            break;
        }
        pItem->SetVisible( bRet );
    }
}

void CScrollView::UpdateScissorRect()
{
    CVec2 ltCorner;
    GetCorner( eCP_TopLeft, ltCorner.x, ltCorner.y);
    CVec2 rbCorner;
    GetCorner( eCP_BottomRight, rbCorner.x, rbCorner.y );
    for ( size_t i = 0; i < m_Items.size(); ++i )
    {
        m_Items[ i ]->SetClip( true, CRect( ltCorner, rbCorner - ltCorner ));
    }
}
