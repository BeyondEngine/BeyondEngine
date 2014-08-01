#include "stdafx.h"
#include "TopWindow.h"
#include "GUI/WindowManager.h"
#include "GUI/Window/Button.h"
#include "Render/RenderManager.h"
#include "Render/RenderTarget.h"
#include "Event/BaseEvent.h"
#include "Event/TouchEvent.h"

int CTopWindow::nNum = 0;
#define TOP_LAYER_ID GUI_LAYER_ID + 128
#define DEFAULT_TOPWINDOW_NAME _T("unnameTopWindow")

CTopWindow::CTopWindow()
    : m_strName( DEFAULT_TOPWINDOW_NAME )
    , m_colorBack( 0x00000055 )
    , m_pRealControl( nullptr )
    , m_bIsVisible( true )
    , m_fWidth( 0.0f )
    , m_fHeight( 0.0f )
{
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("TopWindow"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CTopWindow::~CTopWindow()
{

}

void CTopWindow::Initialize()
{
    super::Initialize();
    CWindowManager::GetInstance()->RegisterTopWindow( this );
    if ( strcmp( m_strName.c_str(), DEFAULT_TOPWINDOW_NAME ) == 0)
    {
        SetName( GetDefaultName() );
    }
    CWindowManager::GetInstance()->AddToRootTopWindow( this );
    CRenderTarget* pMainRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT( pMainRenderTarget );
    OnRenderTargetSizeChange( (float)pMainRenderTarget->GetWidth(), (float)pMainRenderTarget->GetHeight() );
    if ( m_pRealControl )
    {
        BEATS_ASSERT( m_pRealControl->GetParentNode() == nullptr, 
            _T("you can't attach the contrlo whitch have parent node"));
        if ( m_pRealControl->GetRootFlag() )
        {
            CWindowManager::GetInstance()->RemoveFromRoot( m_pRealControl );
        }
        CWindowManager::GetInstance()->RegisterEventRecipient( m_pRealControl, &CTopWindow::OnCloseChildWindow, this );
        m_pRealControl->SetLayerId( TOP_LAYER_ID + 1 );
    }
    SetVisible( m_bIsVisible );
}

void CTopWindow::Uninitialize()
{
    super::Uninitialize();
    CWindowManager::GetInstance()->LogoutTopWindow( this );
}

void CTopWindow::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_strName, true, 0xFFFFFFFF, _T("名字"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_colorBack, true, 0xFFFFFFFF, _T("背景颜色"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bIsVisible, true, 0xFFFFFFFF, _T("是否可见"), NULL, NULL, NULL );
    DECLARE_DEPENDENCY( serializer, m_pRealControl, _T("控件"), eDT_Weak );
}

bool CTopWindow::OnDependencyChange( void* pComponentAddr, CComponentBase* pComponent )
{
    bool bRet = super::OnDependencyChange( pComponentAddr, pComponent );
    if ( !bRet )
    {
        if ( &m_pRealControl == pComponentAddr )
        {
            CControl* pTempControl = m_pRealControl;
            m_pRealControl = down_cast<CControl*>(pComponent);
            if ( m_pRealControl )
            {
                BEATS_ASSERT( m_pRealControl->GetParentNode() == nullptr, 
                    _T("you can't attach the contrlo whitch have parent node"));
                if ( m_pRealControl->GetRootFlag() )
                {
                    CWindowManager::GetInstance()->RemoveFromRoot( m_pRealControl );
                }
                m_pRealControl->OnParentSizeChange( m_fWidth, m_fHeight );
                m_pRealControl->SetLayerId( TOP_LAYER_ID + 1 );
                CWindowManager::GetInstance()->RegisterEventRecipient( m_pRealControl, &CTopWindow::OnCloseChildWindow, this );
            }
            if ( pTempControl )
            {
                CWindowManager::GetInstance()->LogoutEventRecipient( pTempControl, this );
                CWindowManager::GetInstance()->AddToRoot( pTempControl );
                pTempControl->SetLayerId( GUI_LAYER_ID );
            }
            SetVisible( m_pRealControl != NULL);
            bRet = true;
        }
    }
    return bRet;
}

bool CTopWindow::OnPropertyChange( void* pVariableAddr, CSerializer* pNewValueToBeSet )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pNewValueToBeSet );
    if( !bRet )
    {
        if ( &m_bIsVisible == pVariableAddr )
        {
            DeserializeVariable( m_bIsVisible, pNewValueToBeSet );
            SetVisible( m_bIsVisible );
            bRet = true;
        }
    }
    return bRet;
}

void CTopWindow::Render()
{
    //render back ground first
    CQuadP quadp;
    quadp.tl = CVec3( 0.0f, 0.0f, 0.0f );
    quadp.tr = CVec3( m_fWidth, 0.0f, 0.0f );
    quadp.bl = CVec3( 0.0f, m_fHeight, 0.0f );
    quadp.br = CVec3( m_fWidth, m_fHeight, 0.0f );

    CWindowManager::GetInstance()->RenderBaseControl( &quadp, m_colorBack , false, CRect(), nullptr );
    if ( m_pRealControl )
    {
        m_pRealControl->Render();
    }
}

const TString& CTopWindow::GetDefaultName()
{
    return m_strDefaultName;
}

void CTopWindow::SetName( const TString& name )
{
    CWindowManager::GetInstance()->UnregisterTopWindow(this);
    m_strName = name;
    CWindowManager::GetInstance()->RegisterTopWindow(this);
}

const TString& CTopWindow::GetName() const
{
    return m_strName;
}

void CTopWindow::SetVisible( bool bVisible )
{
    m_bIsVisible = bVisible;
    if ( m_pRealControl )
    {
        m_pRealControl->SetVisible( bVisible );
    }
}

bool CTopWindow::IsVisible() const
{
    return m_bIsVisible;
}

bool CTopWindow::OnTouchEvent( CTouchEvent* event )
{
    bool bRet = false;
    if ( m_pRealControl )
    {
        bRet = m_pRealControl->OnTouchEvent( event );
    }
    if ( !bRet )
    {
        if ( event->GetType() == eET_EVENT_TOUCH_ENDED )
        {
            SetVisible( false );
        }
        bRet = true;
    }
    return bRet;
}

void CTopWindow::OnRenderTargetSizeChange( float w, float h )
{
    m_fWidth = w;
    m_fHeight = h;
    if ( m_pRealControl )
    {
        m_pRealControl->OnParentSizeChange( w, h );
    }
}

void CTopWindow::OnCloseChildWindow( CBaseEvent* event )
{
    if( event->GetType() == eET_EVENT_WINDOWCLOSED )
    {
        SetVisible( false );
    }
}

void CTopWindow::Update( float dtt )
{
    if ( m_pRealControl )
    {
        m_pRealControl->Update( dtt );
    }
}