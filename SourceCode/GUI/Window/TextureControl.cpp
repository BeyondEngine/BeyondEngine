#include "stdafx.h"
#include "TextureControl.h"
#include "Render/TextureFrag.h"
#include "GUI/WindowManager.h"
#include "Render/Texture.h"

CTextureControl::CTextureControl()
    : m_eFixMode( eRFM_NO_FIX )
    , m_bGrid9Enabled( false )
    , m_vec2Grid9Offset( CVec2(0.1f,0.1f))
{

}

CTextureControl::~CTextureControl()
{

}

void CTextureControl::Initialize()
{
    UpdateTextrue();
    super::Initialize();
}

void CTextureControl::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_fragImage, true, 0xFFFFFFFF, _T("背景图片"), NULL, NULL, NULL );
    DECLARE_PROPERTY(serializer, m_eFixMode, true, 0xFFFFFFFF, _T("图片大小固定模式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bGrid9Enabled, true, 0xFFFFFFFF, _T("使用九宫格"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_vec2Grid9Offset, true, 0xFFFFFFFF, _T("九宫格文理偏移"), NULL,
    NULL, _T("VisibleWhen:m_bGrid9Enabled == true, SpinStep:1.f, MinValue:0.f, MaxValue:0.5f"));
}

bool CTextureControl::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !bRet )
    {
        if ( &m_fragImage == pVariableAddr )
        {
            DeserializeVariable( m_fragImage, pSerializer );
            UpdateTextrue();
            bRet = true;
        }
        else if( &m_eFixMode == pVariableAddr )
        {
            ERatioFixMode fixMode;
            DeserializeVariable( fixMode, pSerializer );
            SetRatioFixMode(fixMode);
            bRet = true;
        }
    }
    return bRet;
}

void CTextureControl::DoRender()
{
    CTextureFrag* pFrag = m_fragImage.GetTextureFrag();
    if ( pFrag && pFrag->Texture() )
    {
        CWindowManager::GetInstance()->RenderTextureControl( pFrag, IsGrid9Scale(), GetGrid9ScaleOffset(),
            &GetQuadP(), &GetQuadT(), GetColor( m_bUseInheritColorScale ), IsClip(), !IsEnable(), GetClipRect(), &GetWorldTM() );
    }
    super::DoRender();
}

void CTextureControl::SetTextrueFrag( const SReflectTextureInfo& frag )
{
    m_fragImage = frag;
    UpdateTextrue();
}

void CTextureControl::SetRatioFixMode(ERatioFixMode mode)
{
    if ( mode != m_eFixMode )
    {
        m_eFixMode = mode;
        CalcRealSize();
        UpdateQuadP();
        for ( auto childWnd : GetChildren() )
        {
            if ( childWnd->GetType() == eNT_NodeGUI )
            {
                CControl* child = down_cast<CControl*>(childWnd);
                child->OnParentSizeChange( m_vec2Size.x , m_vec2Size.y );
            }
        }
    }
}

const CQuadT& CTextureControl::GetQuadT() const
{
    return m_quadt;
}

SharePtr<CTexture> CTextureControl::GetTextrue() const
{
    SharePtr<CTexture> pRetTexture = nullptr;
    CTextureFrag* pFrag = m_fragImage.GetTextureFrag();
    if ( pFrag && pFrag->Texture() )
    {
        pRetTexture = pFrag->Texture();
    }
    return pRetTexture;
}

bool CTextureControl::IsGrid9Scale() const
{
    return m_bGrid9Enabled;
}

const CVec2& CTextureControl::GetGrid9ScaleOffset() const
{
    return m_vec2Grid9Offset;
}

void CTextureControl::UpdateTextrue()
{
    CTextureFrag* pFrag = m_fragImage.GetTextureFrag();
    if ( pFrag && pFrag->Texture() )
    {
        m_quadt = pFrag->Quad();
    }
}

void CTextureControl::CalcRealSize()
{
    super::CalcRealSize();
    CTextureFrag* frag = m_fragImage.GetTextureFrag();
    if ( frag )
    {
        switch (m_eFixMode)
        {
        case eRFM_NO_FIX:
            break;
        case eRFM_FOLLOW_CONTENT:
            {
                m_vecRealSize = frag->Size();
            }
            break;
        case eRFM_HEIGHT_FOLLOW_WIDTH:
            {
                BEATS_ASSERT( frag->Size().x != 0 && frag->Size().y != 0 );
                m_vecRealSize.y = m_vecRealSize.x / (frag->Size().x / frag->Size().y);
            }
            break;
        case eRFM_WIDTH_FOLLOW_HEIGHT:
            {
                BEATS_ASSERT( frag->Size().x != 0 && frag->Size().y != 0 );
                m_vecRealSize.x = m_vecRealSize.y * (frag->Size().x / frag->Size().y);
            }
            break;
        default:
            BEATS_ASSERT( false, _T("error type") );
            break;
        }
    }
}
