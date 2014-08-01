#include "stdafx.h"
#include "TextBox.h"
#include "Render/TextureFrag.h"
#include "GUI/WindowManager.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"
#include "Render/RenderManager.h"
#include "Render/RenderWindow.h"
#include "GUI/Font/StyledText.h"

int CTextBox::nNum = 0;

CTextBox::CTextBox()
    : m_fXBorderWidth( 5.0f )
    , m_fYBorderWidth( 1.0f )
    , m_uTextSize( 1 )
    , m_fCurXoffset( 1.0f )
    , m_bCurVIsible( true )
    , m_fCurTime( 0.0f )
    , m_strFontName( _T("ipagp") )
    , m_pFontFace( nullptr )
{
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("TextBox"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CTextBox::~CTextBox()
{

}

void CTextBox::Initialize()
{
    super::Initialize();
    m_pFontFace = CFontManager::GetInstance()->GetFace( m_strFontName );
    BEATS_ASSERT( m_pFontFace, _T("can't get the font face named %s"), m_strFontName.c_str() );
}

void CTextBox::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_fXBorderWidth, true, 0xFFFFFFFF, _T("前后端边框宽度"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fYBorderWidth, true, 0xFFFFFFFF, _T("上下端边框宽度"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_strFontName, true, 0xFFFFFFFF, _T("字体名字"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_uTextSize, true, 0xFFFFFFFF, _T("字体大小"), NULL, NULL, NULL );
}

bool CTextBox::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
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
        else if ( &m_fXBorderWidth == pVariableAddr )
        {
            DeserializeVariable( m_fXBorderWidth, pSerializer );
            UpdateQuadP();
            bRet = true;
        }
        else if ( &m_fYBorderWidth == pVariableAddr )
        {
            DeserializeVariable( m_fYBorderWidth, pSerializer );
            UpdateQuadP();
            bRet = true;
        }
        else if ( &m_uTextSize == pVariableAddr )
        {
            DeserializeVariable( m_uTextSize, pSerializer );
            UpdateQuadP();
            bRet = true;
        }
    }
    return bRet;
}

void CTextBox::UpdateQuadP()
{
    super::UpdateQuadP();
    UpdateCur();
}

void CTextBox::DoRender()
{
    super::DoRender();
    for( const SStyledTextSegment &segment : m_textSegmentsTransform )
    {
        CColor color = segment.color;
        color.a = (unsigned char)((float)GetColor( true ).a * segment.color.a / 0xFF);
        segment.pFontFace->RenderText(segment.text, segment.pos.x, segment.pos.y, m_fScaleSize,
            color, 0x000000FF, nullptr, true, &m_rectInnerRect);
    }
    if ( IsFoucus() && m_bCurVIsible )
    {
        CColor color(0xFFFFFFFF);
        CWindowManager::GetInstance()->RenderBaseControl( &m_quadCur, color, IsClip(), m_rectClip, &GetWorldTM() );
    }
}

void CTextBox::ParseText()
{
    CStyledTextParser parser( m_strText.c_str(),
        CVec2(0.0f, 0.0f), m_vecRealSize,
        m_pFontFace, 0xFFFFFFFF, 0x000000FF, EHoriAlign::eHA_LEFT, EVertAlign::eVA_TOP );
    m_textSegments = parser.Parse();
}

const TString& CTextBox::GetText() const
{
    return m_strText;
}

void CTextBox::Update( float dtt )
{
    super::Update( dtt );
    if( m_fCurTime < 1.0f )
    {
        m_fCurTime += dtt;
    }
    else
    {
        m_bCurVIsible = !m_bCurVIsible;
        m_fCurTime = 0.0f;
    }
}

void CTextBox::UpdateCur()
{
    m_quadCur.tl = m_quadp.tl + CVec3( m_fXBorderWidth + m_fCurXoffset, m_fYBorderWidth, 0.0f);
    m_quadCur.bl = m_quadp.bl + CVec3( m_fXBorderWidth + m_fCurXoffset, m_vecRealSize.y - m_fYBorderWidth, 0.0f);
    m_quadCur.tr = m_quadCur.tl;
    m_quadCur.tr.x += 1.0f;
    m_quadCur.br = m_quadCur.bl;
    m_quadCur.br.x += 1.0f;
}

void CTextBox::OnBackSpaceWord()
{
    if ( IsFoucus() )
    {
        m_fCurXoffset -= m_fScaleSize;
        DeleteAChar();
        ParseText();
        UpdateCur();
    }
}

void CTextBox::OnInsertText( const TCHAR* data, int num )
{
    if ( IsFoucus() )
    {
        TString insertString( data, nNum );
        m_strText += insertString;
        auto glyphs = m_pFontFace->GetGlyphs( m_strText );
        m_fCurXoffset = 0;
        CRenderTarget* pCurTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
        BEATS_ASSERT(pCurTarget != NULL);
        for ( auto glyph : glyphs )
        {
            m_fCurXoffset +=  glyph->GetWidth() * pCurTarget->GetScaleFactor();
        }
        ParseText();
        UpdateCur();
    }
}

void CTextBox::DeleteAChar()
{
    auto end = m_strText.end() - 1;
    m_strText.erase( end );
}
