#include "stdafx.h"
#include "TabWindow.h"
#include "Render/TextureFrag.h"
#include "GUI/Event/WindowEvent.h"
#include "Button.h"
#include "Label.h"
#include "GUI/WindowManager.h"
#include "Render/RenderManager.h"
#include "Render/Texture.h"

int CTabWindow::nNum = 0;

CTabWindow::CTabWindow()
    : m_vec2TabButtonSize( 200.0f, 100.0f )
    , m_eTabButtonSideType( eTS_Top )
    , m_eTabButtonPositionType( eTP_Center )
    , m_uCurrentIndex( 0 )
    , m_bShowText( true )
    , m_uTextSize( 2 )
{
    m_vec2Size = CVec2( 800.0f, 600.0f );
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("TabWindow"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CTabWindow::~CTabWindow()
{

}

void CTabWindow::Initialize()
{
    super::Initialize();
    
    for ( auto control : m_windowVector )
    {
        AddChild( control );
        control->SetSize( CVec2( 0.0f, 0.0f) );
        control->SetPercentPosition( CVec2( 0.0f, -GetAnchor().y ) );
        control->SetPercentSize( CVec2( 1.0f, 1.0f) );
        control->SetVisible( false );
        CreateTabButton( );
    }
    UpdteButtonImage();
    UpdateTabButtonStyle();
    UpdateTabButtonData();
    UpdateTabWindow();
}

void CTabWindow::Uninitialize()
{
    for ( auto iter : m_buttonLabelMap )
    {
        CWindowManager::GetInstance()->LogoutEventRecipient( iter.first, this );
        CWindowManager::GetInstance()->DestoryControl( iter.first );
        CWindowManager::GetInstance()->DestoryControl( iter.second );
    }
    super::Uninitialize();
}

void CTabWindow::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_fragButtonImage, true, 0xFFFFFFFF, _T("按钮图像"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fragButtonToggedImage, true, 0xFFFFFFFF, _T("按钮锁住图像"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_vec2TabButtonSize, true, 0xFFFFFFFF, _T("按钮大小"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_eTabButtonSideType, true, 0xFFFFFFFF,_T("按钮靠边"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_eTabButtonPositionType, true, 0xFFFFFFF, _T("按钮位置"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_bShowText, true, 0xFFFFFFFF, _T("显示窗口名字"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_uTextSize, true, 0xFFFFFFFF, _T("窗口字体大小"), NULL, NULL, NULL );
    DECLARE_DEPENDENCY_LIST( serializer, m_windowVector,_T("Tab子控件"), eDT_Weak );
}

bool CTabWindow::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_vec2TabButtonSize == pVariableAddr )
        {
            DeserializeVariable( m_vec2TabButtonSize, pSerializer );
            UpdateTabButtonStyle();
            UpdateTabWindow();
            bRet = true;
        }
        else if( &m_eTabButtonSideType == pVariableAddr )
        {
            DeserializeVariable( m_eTabButtonSideType, pSerializer );
            UpdateTabButtonStyle();
            UpdateTabWindow();
            bRet = true;
        }
        else if( &m_eTabButtonPositionType == pVariableAddr )
        {
            DeserializeVariable( m_eTabButtonPositionType, pSerializer );
            UpdateTabButtonStyle();
            bRet = true;
        }
        else if( &m_uTextSize == pVariableAddr )
        {
            DeserializeVariable( m_uTextSize, pSerializer );
            UpdateTabButtonData();
            bRet = true;
        }
        else if( &m_bShowText == pVariableAddr )
        {
            DeserializeVariable( m_bShowText, pSerializer );
            UpdateTabButtonData();
            bRet = true;
        }
        else if( &m_fragButtonImage == pVariableAddr )
        {
            DeserializeVariable( m_fragButtonImage, pSerializer );
            UpdteButtonImage();
            bRet = true;
        }
        else if( &m_fragButtonToggedImage == pVariableAddr )
        {
            DeserializeVariable( m_fragButtonToggedImage, pSerializer );
            UpdteButtonImage();
            bRet = true;
        }
    }
    return bRet;
}

bool CTabWindow::OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent)
{
    bool bRet = super::OnDependencyChange( pComponentAddr, pComponent );
    if ( !bRet )
    {
        if ( &m_windowVector == pComponentAddr )
        {
            CControl* pControl = (CControl*)pComponent;
            BEATS_ASSERT(dynamic_cast<CControl*>(pComponent) != NULL || action == eDCA_Delete);
            if (action == eDCA_Add)
            {
                AddTabPage( pControl );
            }
            else if ( action == eDCA_Change || action == eDCA_Delete )
            {
                RemovePage( pControl );
            }
            bRet = true;
        }
    }
    return bRet;
}

void CTabWindow::UpdateTabButtonStyle()
{
    float yPos = m_eTabButtonSideType == eTS_Bottom ? m_vecRealSize.y * (1.0f - GetAnchor().y) - m_vec2TabButtonSize.y : 0 - GetAnchor().y * m_vecRealSize.y;
    for ( auto buttonPair : m_buttonLabelMap )
    {
        buttonPair.first->SetAbsolutePosition( CVec2(0.0f, yPos) );
        buttonPair.first->SetSize( m_vec2TabButtonSize );
    }

    size_t count = m_buttonLabelMap.size();
    float width = count * m_vec2TabButtonSize.x;
    float xPos = 0.0f;
    switch ( m_eTabButtonPositionType )
    {
    case eTP_Left:
        {
            xPos = m_vecRealSize.x * GetAnchor().x * -1.0f;
        }
        break;
    case eTP_Center:
        {
            xPos = m_vecRealSize.x * ( 0.5f - GetAnchor().x ) - 0.5f * width;
        }
        
        break;
    case eTP_Right:
        {
            xPos = m_vecRealSize.x * ( 1.0f - GetAnchor().x ) - width;
        }
        break;
    default:
        BEATS_ASSERT( false, _T("bad type"))
        break;
    }
    int i = 0;
    for ( auto buttonPair : m_buttonLabelMap )
    {
        CVec2 position = buttonPair.first->GetAbsolutePosition();
        buttonPair.first->SetAbsolutePosition( CVec2( xPos + m_vec2TabButtonSize.x * i , position.y));
        i++;
    }
}

void CTabWindow::UpdteButtonImage()
{
    size_t i = 0;
    for ( auto buttonPair : m_buttonLabelMap )
    {
        SReflectTextureInfo& operateTexture = i == m_uCurrentIndex ? m_fragButtonImage : m_fragButtonToggedImage;
        CTextureFrag* pFrag = operateTexture.GetTextureFrag();
        if ( pFrag && pFrag->Texture() )
        {
            buttonPair.first->SetTextrueFrag( operateTexture );
        }
        i++;
    }
}

void CTabWindow::UpdateTabWindow()
{
    for ( size_t i = 0; i < m_windowVector.size(); ++i )
    {
        bool bVisible = i == m_uCurrentIndex;
        if ( bVisible )
        {
            float y = m_eTabButtonSideType == eTS_Top ? ( 1.0f - GetAnchor().y ) * m_vec2TabButtonSize.y: GetAnchor().y * -m_vec2TabButtonSize.y;
            m_windowVector[ i ]->SetAbsolutePosition( CVec2( 0.0f, y ) );
        }
        m_windowVector[ i ]->SetVisible( bVisible );
        m_windowVector[ i ]->OnParentSizeChange( m_vecRealSize.x, m_vecRealSize.y - m_vec2TabButtonSize.y );
    }
}

void CTabWindow::CreateTabButton( )
{
    BEATS_ASSERT( m_buttonLabelMap.size() <= m_windowVector.size(),
        _T("can't create button, when the button count bigger than control count"));
    CButton* pButton = CWindowManager::GetInstance()->CreateControl<CButton>( DEFAULT_CONTROL_NAME, this );
    CWindowManager::GetInstance()->RegisterEventRecipient( pButton, &CTabWindow::OnTabButtonClicked, this );
    pButton->OnParentSizeChange( m_vecRealSize.x, m_vecRealSize.y );
    CLabel* pLabel = CWindowManager::GetInstance()->CreateControl<CLabel>( DEFAULT_CONTROL_NAME, pButton );
    pLabel->SetEventDispatchToParent( true );
    pLabel->SetSize( CVec2(0.0f, 0.0f) );
    pLabel->SetPercentSize( CVec2( 1.0f, 1.0f ));
    pLabel->SetTextHoriAlign( eHA_CENTER );
    pLabel->SetTextVertAlign( eVA_TOP );
    m_buttonLabelMap[ pButton ] = pLabel;
}

void CTabWindow::DeleteTabButton( )
{
    BEATS_ASSERT( m_buttonLabelMap.size() > m_windowVector.size(),
        _T("can't create button, when the button count bigger than control count"));
    auto iter = m_buttonLabelMap.begin();
    CWindowManager::GetInstance()->LogoutEventRecipient( iter->first, this );
    CWindowManager::GetInstance()->DestoryControl( iter->first );
    CWindowManager::GetInstance()->DestoryControl( iter->second );
    m_buttonLabelMap.erase( iter );
}

void CTabWindow::OnTabButtonClicked( CBaseEvent* event )
{
    if ( event->GetType() == eET_EVENT_MOUSE_CLICKED )
    {
        CWindowEvent* pEvent = down_cast<CWindowEvent*>(event);
        CButton* button = down_cast<CButton*>(pEvent->OperandWindow());
        m_uCurrentIndex = (size_t)button->GetUserData();
        UpdteButtonImage();
        UpdateTabWindow();
    }
}

void CTabWindow::UpdateTabButtonData()
{
    BEATS_ASSERT( m_windowVector.size() == m_buttonLabelMap.size(), _T("can't update the button data") );
    size_t i = 0;
    auto iter = m_buttonLabelMap.begin();
    for ( ; iter != m_buttonLabelMap.end(); ++iter )
    {
        iter->first->SetUserData( (void*)i);
        iter->second->SetVisible( m_bShowText );
        iter->second->SetTextSize( m_uTextSize );
        iter->second->SetText( m_windowVector[i]->GetName() );
        i++;
    }
}

void CTabWindow::OnParentSizeChange( float width, float height )
{
    super::OnParentSizeChange( width, height );
    UpdateTabWindow();
    UpdateTabButtonStyle();
}

void CTabWindow::SetSize( const CVec2& size )
{
    super::SetSize( size );
    UpdateTabWindow();
    UpdateTabButtonStyle();
}

void CTabWindow::AddTabPage( CControl* pControl )
{
    AddChild( pControl );
    pControl->SetSize( CVec2( 0.0f, 0.0f) );
    pControl->SetPercentPosition( CVec2( 0.0f, -GetAnchor().y ) );
    pControl->SetPercentSize( CVec2( 1.0f, 1.0f) );
    m_windowVector.push_back( pControl );
    pControl->SetVisible( false );
    if ( m_windowVector.size() > m_buttonLabelMap.size() )
    {
        for (size_t i = 0; i < m_windowVector.size() - m_buttonLabelMap.size(); i++)
        {
            CreateTabButton();
        }
    }
    UpdteButtonImage();
    UpdateTabButtonStyle();
    UpdateTabButtonData();
    UpdateTabWindow();
}

void CTabWindow::RemovePage( CControl* pControl )
{
    RemoveChild( pControl );
    pControl->SetVisible( true );
    auto iter = std::find( m_windowVector.begin(), m_windowVector.end(), pControl );
    BEATS_ASSERT( iter != m_windowVector.end(), _T("can't remove this control"));
    m_windowVector.erase( iter );
    if ( m_windowVector.size() < m_buttonLabelMap.size() )
    {
        for (size_t i = 0; i < m_buttonLabelMap.size() - m_windowVector.size(); i++)
        {
            DeleteTabButton();
        }
    }
    UpdteButtonImage();
    UpdateTabButtonStyle();
    UpdateTabButtonData();
}

size_t CTabWindow::GetPageCount() const
{
    return m_windowVector.size();
}

CControl* CTabWindow::GetPage( size_t index ) const
{
    BEATS_ASSERT( index < m_windowVector.size() );
    return m_windowVector[ index ];
}

