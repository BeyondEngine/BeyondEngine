#include "stdafx.h"
#include "ProgressBar.h"
#include "Render/TextureFrag.h"
#include "GUI/WindowManager.h"
#include "Render/Texture.h"

int CProgressBar::nNum = 0;

CProgressBar::CProgressBar()
    : m_uCurrentProgress( 0 )
{
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("ProgressBar"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CProgressBar::~CProgressBar()
{

}

void CProgressBar::Initialize()
{
    super::Initialize();
    UpdateForegroundQuadP();
    UpdateForegroundTexture();
}

void CProgressBar::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_uCurrentProgress, true, 0xFFFFFFFF, _T("当前进度"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fragForeGround, true, 0xFFFFFFFF, _T("前景图片"), NULL, NULL, NULL );
}

void CProgressBar::SetCurrentProgress( size_t progress )
{
    m_uCurrentProgress = progress;
    UpdateForegroundQuadP();
}

void CProgressBar::DoRender()
{
    super::DoRender();
    CTextureFrag* pFrag = m_fragForeGround.GetTextureFrag();
    if (pFrag && pFrag->Texture())
    {
        CWindowManager::GetInstance()->RenderTextureControl( pFrag, IsGrid9Scale(), GetGrid9ScaleOffset(), &m_foreQuadp,
            &m_foreQuadt, GetColor( m_bUseInheritColorScale ), IsClip(), !IsEnable(), GetClipRect(), &GetWorldTM() );
    }
}

void CProgressBar::UpdateQuadP()
{
    super::UpdateQuadP();
    UpdateForegroundQuadP();
}

bool CProgressBar::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if( !bRet )
    {
        if ( &m_uCurrentProgress == pVariableAddr )
        {
            DeserializeVariable( m_uCurrentProgress, pSerializer );
            SetCurrentProgress( m_uCurrentProgress );
            bRet = true;
        }
        else if( &m_fragForeGround == pVariableAddr )
        {
            DeserializeVariable( m_fragForeGround, pSerializer );
            UpdateForegroundTexture();
            bRet = true;
        }
    }
    return bRet;
}

void CProgressBar::UpdateTextrue()
{
    super::UpdateTextrue();
    UpdateForegroundTexture();
}

void CProgressBar::UpdateForegroundQuadP()
{
    m_foreQuadp.tl = m_quadp.tl;
    m_foreQuadp.bl = m_quadp.bl;

    float x = m_uCurrentProgress * 0.01f * m_vecRealSize.x;
    m_foreQuadp.tr = m_foreQuadp.tl;
    m_foreQuadp.tr.x += x;

    m_foreQuadp.br = m_foreQuadp.bl;
    m_foreQuadp.br.x += x;
}

void CProgressBar::UpdateForegroundTexture()
{
    CTextureFrag* frag = m_fragForeGround.GetTextureFrag();
    if ( frag )
    {
        m_foreQuadt = frag->Quad();
    }
}
