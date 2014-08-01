#include "stdafx.h"
#include "Label.h"
#include "GUI/Font/FontManager.h"
#include "GUI/WindowManager.h"
#include "GUI/Font/StyledText.h"
#include "GUI/Font/FontFace.h"
#include "Render/RenderManager.h"
#include "Render/RenderWindow.h"

int CLabel::nNum = 0;

CLabel::CLabel()
    : m_fPadding( 0.0f )
    , m_uTextSize( 1 )
    , m_textColor( 0xFFFFFFFF )
    , m_borderColor( 0x000000FF )
    , m_hAlign( eHA_LEFT )
    , m_vAlign( eVA_MIDDLE )
    , m_strFontName( _T("ipagp") )
    , m_pFontFace( nullptr )
{
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("Label"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CLabel::~CLabel()
{

}

void CLabel::Initialize()
{
    super::Initialize();
    if( !m_strFontName.empty() )
    {
        m_pFontFace = CFontManager::GetInstance()->GetFace( m_strFontName );
        BEATS_ASSERT( m_pFontFace, _T("can't get the font face named %s"), m_strFontName.c_str() );
    }
    if (m_pFontFace)
    {
        ParseText();
    }
}

void CLabel::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_strFontName, true, 0xFFFFFFFF, _T("字体名字"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_strText, true, 0xFFFFFFFF, _T("内容"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_uTextSize, true, 0xFFFFFFFF, _T("字号"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_textColor, true, 0xFFFFFFFF, _T("文本颜色"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_borderColor, true, 0xFFFFFFFF, _T("边框颜色"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_hAlign, true, 0xFFFFFFFF, _T("水平对齐"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_vAlign, true, 0xFFFFFFFF, _T("垂直对齐"), NULL, NULL, NULL);
}

bool CLabel::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_strFontName == pVariableAddr )
        {
            TString fontName;
            DeserializeVariable( fontName, pSerializer );
            if ( fontName != m_strFontName )
            {
                m_strFontName = fontName;
                m_pFontFace = CFontManager::GetInstance()->GetFace( m_strFontName );
                BEATS_ASSERT( m_pFontFace, _T("can't get the font face named %s"), m_strFontName.c_str() );
                ParseText();
            }
            bRet = true;
        }
        else if ( &m_strText == pVariableAddr )
        {
            TString steText;
            DeserializeVariable( steText, pSerializer );
            SetText( steText );
            bRet = true;
        }
        else if ( &m_uTextSize == pVariableAddr )
        {
            size_t uSize;
            DeserializeVariable( uSize, pSerializer );
            SetTextSize( uSize );
            bRet = true;
        }
        else if ( &m_hAlign == pVariableAddr )
        {
            EHoriAlign eHAlign;
            DeserializeVariable( eHAlign, pSerializer );
            SetTextHoriAlign( eHAlign );
            bRet = true;
        }
        else if ( &m_vAlign == pVariableAddr )
        {
            EVertAlign eVAlign;
            DeserializeVariable( eVAlign, pSerializer );
            SetTextVertAlign( eVAlign );
            bRet = true;
        }
    }
    return bRet;
}

void CLabel::DoRender()
{
    if ( !m_strText.empty() )
    {
        float x = 0;
        float y = 0;
        GetAnchorPosition( x, y );
        for( const SStyledTextSegment &segment : m_textSegments )
        {
            CColor color = segment.color;
            color.a = (unsigned char)((float)GetColor( true ).a * segment.color.a / 0xFF);
            segment.pFontFace->RenderText(segment.text, segment.pos.x - x, segment.pos.y - y, (float)m_uTextSize,
                color, m_borderColor, &GetWorldTM(), true, IsClip() ? &m_rectClip : nullptr );
        }
    }
    super::DoRender();
}

void CLabel::ParseText()
{
    CStyledTextParser parser( m_strText.c_str(),
        CVec2(m_fPadding, m_fPadding), m_vecRealSize - m_fPadding,
        m_pFontFace, m_textColor, m_borderColor, m_hAlign, m_vAlign);
    m_textSegments = parser.Parse();
}

void CLabel::SetText( const TString& text )
{
    if ( m_strText != text )
    {
        m_strText = text;
        ParseText();
    }
}

void CLabel::SetTextSize( size_t size )
{
    if ( m_uTextSize != size )
    {
        m_uTextSize = size;
    }
}

void CLabel::SetTextHoriAlign( EHoriAlign hAlign )
{
    if ( hAlign != m_hAlign )
    {
        m_hAlign = hAlign;
        ParseText();
    }
}

void CLabel::SetTextVertAlign( EVertAlign vAlign )
{
    if ( vAlign != m_vAlign )
    {
        m_vAlign = vAlign;
        ParseText();
    }
}
