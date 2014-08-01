#include "stdafx.h"
#include "Window.h"
#include "Render/TextureFrag.h"
#include "GUI/WindowManager.h"
#include "Button.h"
#include "Event/BaseEvent.h"

#define CLOSEBUTTONMINSCALE 0.15f

int CWindow::nNum = 0;

CWindow::CWindow()
    : m_pCloseButton( nullptr )
{
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("Window"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CWindow::~CWindow()
{

}

void CWindow::Initialize()
{
    super::Initialize();
    TCHAR szCloseName[MAX_PATH];
    _stprintf( szCloseName, _T("%s_closeButton"), GetName().c_str() );
    m_pCloseButton = CWindowManager::GetInstance()->CreateControl<CButton>( szCloseName, this );
    CWindowManager::GetInstance()->RegisterEventRecipient( m_pCloseButton, &CWindow::OnCloseButtonClicked , this );
    if ( m_pCloseButton )
    {
        m_pCloseButton->SetPercentPosition( m_vec2CloseButtonPosition - m_vec2Anchor );
        m_pCloseButton->SetSize( m_vec2CloseButtonSize );
        UpdateCloseButton();
        if ( m_fragCloseButton.GetTextureFrag() )
        {
            m_pCloseButton->SetTextrueFrag( m_fragCloseButton );
        }
        m_pCloseButton->OnParentSizeChange( m_vecRealSize.x, m_vecRealSize.y );
    }
}

void CWindow::Uninitialize()
{
    CWindowManager::GetInstance()->DestoryControl( m_pCloseButton );
    super::Uninitialize();
}

void CWindow::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_fragCloseButton, true, 0xFFFFFFFF, _T("关闭图片"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_vec2CloseButtonSize, true, 0xFFFFFFFF, _T("关闭按钮大小"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_vec2CloseButtonPosition, true, 0xFFFFFFFF, _T("关闭按钮百分比位置"), NULL, NULL, NULL );
}

void CWindow::OnCloseButtonClicked( CBaseEvent* event )
{
    if ( m_bEnable )
    {
        SetVisible( false );
        CWindowEvent event( eET_EVENT_WINDOWCLOSED, this );
        CWindowManager::GetInstance()->DispatchEventToRecipient( this , &event );
    }
}

bool CWindow::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_vec2CloseButtonSize == pVariableAddr )
        {
            DeserializeVariable( m_vec2CloseButtonSize, pSerializer );
            if ( m_pCloseButton )
            {
                m_pCloseButton->SetSize( m_vec2CloseButtonSize );
            }
            bRet = true;
        }
        else if( &m_vec2CloseButtonPosition == pVariableAddr )
        {
            DeserializeVariable( m_vec2CloseButtonPosition, pSerializer );
            if ( m_pCloseButton )
            {
                m_pCloseButton->SetPercentPosition( m_vec2CloseButtonPosition - m_vec2Anchor );
                UpdateCloseButton();
            }
            bRet = true;
        }
        else if( &m_fragCloseButton == pVariableAddr )
        {
            DeserializeVariable( m_fragCloseButton, pSerializer );
            if ( m_pCloseButton )
            {
                m_pCloseButton->SetTextrueFrag( m_fragCloseButton );
            }
            bRet = true;
        }
    }
    return bRet;
}

void CWindow::UpdateCloseButton()
{
    BEATS_ASSERT( m_pCloseButton );
    float x = 0.5f, y = 0.5f;
    x = m_vec2CloseButtonPosition.x > 0.5f ? 1.0f : 0.0f;
    y = m_vec2CloseButtonPosition.y > 0.5f ? 1.0f : 0.0f;
    m_pCloseButton->SetAnchor( CVec2( x, y ) );
}
