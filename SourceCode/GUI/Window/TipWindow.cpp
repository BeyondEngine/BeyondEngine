#include "stdafx.h"
#include "TipWindow.h"
#include "Render/TextureFrag.h"
#include "Render/RenderManager.h"
#include "Render/RenderTarget.h"
#include "GUI/WindowManager.h"
#include "Event/TouchEvent.h"
#include "Render/Texture.h"

int CTipWindow::nNum = 0;

CTipWindow::CTipWindow()
    : m_fHeadLength( 5.0f )
    , m_eHeadDeriction( eHD_Bottom )
    , xOffset( 0.0f )
    , yOffset( 0.0f )
    , m_pAttachControl( nullptr )
    , m_bUpdateFinish( false )
{
    m_vec2Anchor = CVec2( 0.5f, 1.0f );
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("TipWindow"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CTipWindow::~CTipWindow()
{

}

void CTipWindow::Initialize()
{
    super::Initialize();
    UpdateTextrue();
}

void CTipWindow::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_vec2PosOffset, true, 0xFFFFFFFF, _T("目标点偏移"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fHeadLength, true, 0xFFFFFFFF, _T("箭头高度"), NULL, NULL, _T("MinValue:0") );
    DECLARE_DEPENDENCY( serializer, m_pAttachControl, _T("提示内容"), eDT_Weak );
}

bool CTipWindow::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_fHeadLength == pVariableAddr )
        {
            DeserializeVariable( m_fHeadLength, pSerializer );
            UpdateQuadP();
            bRet = true;
        }
    }
    return bRet;
}

bool CTipWindow::OnDependencyChange( void* pComponentAddr, CComponentBase* pComponent )
{
    bool bRet = super::OnDependencyChange( pComponentAddr, pComponent );
    if ( !bRet )
    {
        if ( &m_pAttachControl == pComponentAddr )
        {
            CControl* pAttachControl = (CControl*)pComponent;
            if ( pAttachControl )
            {
                SetAttachControl( pAttachControl );
            }
            bRet = true;
        }
    }
    return bRet;
}

void CTipWindow::DoRender()
{
    if ( m_bUpdateFinish )
    {
        super::DoRender();
        CTextureFrag* pFrag = m_fragImage.GetTextureFrag();
        if ( pFrag && pFrag->Texture() )
        {
            CWindowManager::GetInstance()->RenderTextureControl( pFrag, false, GetGrid9ScaleOffset(),
                &m_headQuadp, &m_headQuadt, GetColor( m_bUseInheritColorScale ), IsClip(), !IsEnable(), GetClipRect(), &GetWorldTM() );
        }
    }
}

void CTipWindow::PostRender()
{
    if ( m_pAttachControl && m_bUpdateFinish )
    {
        m_pAttachControl->Render();
    }
}

void CTipWindow::Update( float dtt )
{
    super::Update( dtt );
    if ( IsVisible() )
    {
        float x = 0.0f, y = 0.0f;
        if ( !CheckRectInRenderTarget( x, y ))
        {
            xOffset = x;
            yOffset = y;
            UpdateQuadP();
            AligningTheControl();
            //move
            CVec3 position = GetPosition();
            position.x += x;
            position.y += y;
            SetPosition( position.x, position.y, position.z );
        }
        else
        {
            m_bUpdateFinish = true;
        }
    }
}

bool CTipWindow::CheckRectInRenderTarget( float& x, float& y )
{
    bool bRet = false;
    CNode* pParent = GetParentNode();
    CControl* pParentControl = ( CControl* )pParent;
    if ( pParentControl )
    {
        CVec2 ltCorner;
        GetCorner( eCP_TopLeft, ltCorner.x, ltCorner.y);
        CRenderTarget* pRenderTaget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
        BEATS_ASSERT( pRenderTaget );
        CRect checkRect( CVec2(0.0f, 0.0f), 
            CVec2( (float)pRenderTaget->GetWidth(), (float)pRenderTaget->GetHeight()) );
        if ( ltCorner.x >= checkRect.position.x && ltCorner.y >= checkRect.position.y &&
            ltCorner.x + m_vecRealSize.x <= checkRect.size.x && ltCorner.y + m_vecRealSize.y <= checkRect.size.y )
        {
            bRet = true;
        }
        else
        {
            CVec2 parentSize = pParentControl->GetRealSize();
            CVec2 parentAnchor = pParentControl->GetAnchor();
            //left out
            if( ltCorner.x < checkRect.position.x )
            {
                //move right first
                x = -ltCorner.x;
                //check top
                if ( fabs( x ) < m_vec2Anchor.x* m_vecRealSize.x - m_fHeadLength )
                {
                    if ( ltCorner.y < checkRect.position.y )
                    {
                        //turn buttom
                        m_eHeadDeriction = eHD_Top;
                        y = m_vecRealSize.y + parentSize.y * ( 1.0f - parentAnchor.y );
                    }
                }
                else
                {
                    if ( ltCorner.y < checkRect.position.y )
                    {
                        //turn buttom
                        m_eHeadDeriction = eHD_Top;
                        y = m_vecRealSize.y + parentSize.y * ( 1.0f - parentAnchor.y );
                    }
                    else
                    {
                        x = m_vec2Anchor.x* m_vecRealSize.x - m_fHeadLength + (1.0f - parentAnchor.x) * parentSize.x;
                        m_eHeadDeriction = eHD_Left;
                        y = m_vecRealSize.y * 0.5f;
                        if ( ltCorner.y + y > checkRect.size.y )
                        {
                            y = ltCorner.y + y - checkRect.size.y;
                        }
                    }
                }
            }
            //right out
            else if(  ltCorner.x + m_vecRealSize.x > checkRect.size.x )
            {
                //move left first
                x = checkRect.size.x - (ltCorner.x + m_vecRealSize.x) ;
                //check top
                if ( fabs( x ) < (1.0f - m_vec2Anchor.x)* m_vecRealSize.x - m_fHeadLength )
                {
                    if ( ltCorner.y < checkRect.position.y )
                    {
                        //turn buttom
                        m_eHeadDeriction = eHD_Top;
                        y = m_vecRealSize.y + parentSize.y * ( 1.0f - parentAnchor.y );
                    }
                }
                else
                {
                    if ( ltCorner.y < checkRect.position.y )
                    {
                        //turn buttom
                        m_eHeadDeriction = eHD_Top;
                        y = m_vecRealSize.y + parentSize.y * ( 1.0f - parentAnchor.y );
                    }
                    else
                    {
                        x = -(1.0f - m_vec2Anchor.x)* m_vecRealSize.x - parentAnchor.x * parentSize.x;
                        m_eHeadDeriction = eHD_Right;
                        y = m_vecRealSize.y * 0.5f;
                        if ( ltCorner.y + y > checkRect.size.y )
                        {
                            y = ltCorner.y + y - checkRect.size.y;
                        }
                    }
                }
                
            }
            //top out
            else if(  ltCorner.y < checkRect.position.y )
            {
                m_eHeadDeriction = eHD_Top;
                y = m_vecRealSize.y + parentSize.y * ( 1.0f - parentAnchor.y );
            }
        }
    }
    else
    {
        bRet = false;
        x = m_vecRealSize.x * 0.5f;
        y = m_vecRealSize.y;
    }
    return bRet;
}

void CTipWindow::ResetTip()
{
    m_eHeadDeriction = eHD_Bottom;
    m_vec2Anchor = CVec2( 0.5f, 1.0f );
    SetPosition( m_vec2PosOffset.x, m_vec2PosOffset.y, 0.0f );
}

void CTipWindow::SetAttachControl( CControl* pControl )
{
    m_pAttachControl = pControl;
    xOffset = 0.0f;
    yOffset = 0.0f;
    SetPercentSize( CVec2( 0.0f, 0.0f ));
    SetSize( m_pAttachControl->GetRealSize() );
    m_bUpdateFinish = false;
    ResetTip();
}

void CTipWindow::OnParentSizeChange( float width, float height )
{
    super::OnParentSizeChange( width, height );
    ResetTip();
}

bool CTipWindow::OnTouchEvent( CTouchEvent* event )
{
    bool bRet = false;
    if ( m_pAttachControl )
    {
        bRet = m_pAttachControl->OnTouchEvent( event );
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

void CTipWindow::AligningTheControl()
{
    if( m_pAttachControl )
    {
        CVec2 anchor = m_pAttachControl->GetAnchor();
        CVec2 size = m_pAttachControl->GetRealSize();
        float x = -m_vecRealSize.x * 0.5f;
        float y = -m_vecRealSize.y;
        switch ( m_eHeadDeriction )
        {
        case eHD_Bottom:
            {
                x += anchor.x * size.x;
                y += anchor.y * size.y;
            }
            break;
        case eHD_Top:
            {
                x += anchor.x * size.x;
                y += anchor.y * size.y;
                y += m_fHeadLength;
            }
            break;
        case eHD_Left:
            {
                x += anchor.x * size.x;
                x += m_fHeadLength;
                y += anchor.y * size.y;
            }
            break;
        case eHD_Right:
            {
                x += anchor.x * size.x;
                y += anchor.y * size.y;
            }
            break;
        default:
            BEATS_ASSERT( false, _T("error"));
            break;
        }
        m_pAttachControl->SetAbsolutePosition( CVec2( x, y));
    }
}

void CTipWindow::UpdateQuadP()
{
    super::UpdateQuadP();
    switch ( m_eHeadDeriction )
    {
    case eHD_Bottom:
        {
            m_quadp.bl.y -= m_fHeadLength;
            m_quadp.br.y -= m_fHeadLength;

            m_headQuadp.bl.x = m_vec2Anchor.x * m_vecRealSize.x - xOffset;
            if( m_headQuadp.bl.x < m_fHeadLength )
            {
                m_headQuadp.bl.x = m_fHeadLength;
            }
            m_headQuadp.bl.y = m_vecRealSize.y;

            m_headQuadp.tl = m_headQuadp.bl;
            m_headQuadp.tl.y -= m_fHeadLength;

            m_headQuadp.tr = m_headQuadp.tl;
            m_headQuadp.tr.x += 0.5f * m_fHeadLength;

            m_headQuadp.br = m_headQuadp.bl;
        }
        break;
    case eHD_Top:
        {
            m_quadp.tl.y += m_fHeadLength;
            m_quadp.tr.y += m_fHeadLength;

            m_headQuadp.tr.x = m_quadp.tl.x + m_vecRealSize.x * m_vec2Anchor.x - xOffset;
            if( m_headQuadp.tr.x < m_fHeadLength )
            {
                m_headQuadp.tr.x = m_fHeadLength;
            }
            m_headQuadp.tr.y = -m_vecRealSize.y;

            m_headQuadp.br = m_headQuadp.tr;
            m_headQuadp.br.y = -m_vecRealSize.y + m_fHeadLength;

            m_headQuadp.bl = m_headQuadp.br;
            m_headQuadp.bl.x -= 0.5f * m_fHeadLength;

            m_headQuadp.tl = m_headQuadp.tr;
        }
        break;
    case eHD_Left:
        {
            m_quadp.tl.x += m_fHeadLength;
            m_quadp.bl.x += m_fHeadLength;

            m_headQuadp.bl.x = 0.0f;
            m_headQuadp.bl.y = m_vec2Anchor.y * m_vecRealSize.y - yOffset;

            m_headQuadp.br = m_headQuadp.bl;
            m_headQuadp.br.x = m_fHeadLength;

            m_headQuadp.tr = m_headQuadp.br;
            m_headQuadp.tr.y -= m_fHeadLength * 0.5f;

            m_headQuadp.tl = m_headQuadp.bl;
        }
        break;
    case eHD_Right:
        {
            m_quadp.tr.x -= m_fHeadLength;
            m_quadp.br.x -= m_fHeadLength;

            m_headQuadp.tr.x = m_vecRealSize.x;
            m_headQuadp.tr.y = m_vec2Anchor.y * m_vecRealSize.y - yOffset;

            m_headQuadp.br = m_headQuadp.tr;

            m_headQuadp.tl = m_headQuadp.tr;
            m_headQuadp.tl.x -= m_fHeadLength;

            m_headQuadp.bl = m_headQuadp.tl;
            m_headQuadp.bl.y += m_fHeadLength * 0.5f;
        }
        break;
    default:
        BEATS_ASSERT( false, _T("error"));
        break;
    }
}

void CTipWindow::UpdateTextrue()
{
    super::UpdateTextrue();
    m_headQuadt.tl.u = m_quadt.tl.u + ( m_quadt.tr.u - m_quadt.tl.u ) * 0.5f;
    m_headQuadt.tl.v = m_quadt.tl.v;

    m_headQuadt.bl.u = m_quadt.bl.u + ( m_quadt.br.u - m_quadt.bl.u ) * 0.5f;
    m_headQuadt.bl.v = m_quadt.bl.v;

    m_headQuadt.br.u = m_quadt.bl.u + ( m_quadt.br.u - m_quadt.bl.u ) * 0.7f;
    m_headQuadt.br.v = m_quadt.bl.v;

    m_headQuadt.tr = m_headQuadt.tl;
}