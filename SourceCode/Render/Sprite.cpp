#include "stdafx.h"
#include "Sprite.h"
#include "Texture.h"
#include "RenderManager.h"
#include "Resource/ResourceManager.h"
#include "TextureFrag.h"
#include "Material.h"
#include "RenderState.h"
#include "ShaderProgram.h"
#include "RenderGroup.h"
#include "RenderGroupManager.h"
#include "Shader.h"
#include "Resource/ResourceManager.h"
#include "RenderBatch.h"
#include "RenderWindow.h"
#include "TextureAtlas.h"
#include "ReflectTextureInfo.h"
#include "Utility/BeatsUtility/FilePathTool.h"

CSprite::CSprite()
    : m_bUseInheritColorScale(true)
{
    kmVec2Fill(&m_anchor, 0.f, 0.f);
    kmVec2Fill(&m_size, 0.f, 0.f);
}

CSprite::~CSprite()
{
}

void CSprite::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_fragInfo, true, 0xFFFFFFFF, _T("纹理"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_anchor, true, 0xFFFFFFFF, _T("锚点"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_size, true, 0xFFFFFFFF, _T("尺寸"), NULL, NULL, NULL);
}

bool CSprite::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bHandled = super::OnPropertyChange(pVariableAddr, pSerializer);
    if(!bHandled)
    {
        if(pVariableAddr == &m_fragInfo)
        {
            DeserializeVariable(m_fragInfo, pSerializer);
            bHandled = true;
            OnTextureFragChanged();
        }
        else if (pVariableAddr == &m_size)
        {
            CVec2 size;
            DeserializeVariable(size, pSerializer);
            bHandled = true;
            SetSize(size.x, size.y);
        }
        else if (pVariableAddr == &m_anchor)
        {
            CVec2 anchor;
            DeserializeVariable(anchor, pSerializer);
            bHandled = true;
            SetAnchor(anchor.x, anchor.y);
        }
    }
    return bHandled;
}

void CSprite::Initialize()
{
    super::Initialize();
    SetRealSize(m_size.x, m_size.y);
    if(GetTextureFrag())
    {
        OnTextureFragChanged();
    }
}

const CQuadP &CSprite::QuadP() const
{
    return m_quad;
}

const CQuadT &CSprite::QuadT() const
{
    BEATS_ASSERT(m_fragInfo.GetTextureFrag());
    return m_fragInfo.GetTextureFrag()->Quad();
}

CTextureFrag* CSprite::GetTextureFrag() const
{
    return m_fragInfo.GetTextureFrag();
}

SharePtr<CMaterial> CSprite::Material()
{
    if(!m_pMaterial)
    {
        m_pMaterial = new CMaterial();
        m_pMaterial->SetSharders( _T("SpriteShader.vs"), _T("SpriteShader.ps") );
        m_pMaterial->SetCullFaceEnable(false);
        m_pMaterial->SetDepthTest( false );
        m_pMaterial->SetBlendEnable( true );
        m_pMaterial->SetBlendSource(GL_SRC_ALPHA);
        m_pMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    }
    m_pMaterial->SetTexture(0, GetTextureFrag()->Texture());
    return m_pMaterial;
}

void CSprite::SetUseInheritColorScale(bool bUse)
{
    m_bUseInheritColorScale = bUse;
}

void CSprite::PreRender()
{
    // Do nothing
}

void CSprite::DoRender()
{
    CTextureFrag* pFrag = GetTextureFrag();
    if (pFrag && pFrag->Texture())
    {
        CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_3D_ALPHA);
        BEATS_ASSERT(renderGroup);
        CRenderBatch *batch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPTC), Material(), GL_TRIANGLES, true);
        batch->AddQuad(&m_quad, &QuadT(), GetColorScale(m_bUseInheritColorScale), &GetWorldTM());
    }
}

bool CSprite::HitTest(float x, float y) const
{
    bool bHitted = false;
    if(!bHitted)
    {
        float fRealAnchorX = m_anchor.x * m_size.x;
        float fRealAnchorY = m_anchor.y * m_size.y;
        bHitted = (-fRealAnchorX <= x && x <= m_size.x - fRealAnchorX) &&
            (-fRealAnchorY <= y && y <= m_size.y - fRealAnchorY);
    }
    BEATS_PRINT(_T("Hitted:%s\n"), bHitted ? _T("true") : _T("false"));
    return bHitted;
}

void CSprite::SetAnchor(float anchorX, float anchorY)
{
    if (!BEATS_FLOAT_EQUAL(anchorX, m_anchor.x) ||
        !BEATS_FLOAT_EQUAL(anchorY, m_anchor.y))
    {
        m_anchor.x = anchorX;
        m_anchor.y = anchorY;
        UPDATE_PROPERTY_PROXY(m_anchor);
        UpdateVertices();
    }
}

const kmVec2 &CSprite::Anchor() const
{
    return m_anchor;
}

void CSprite::SetSize(float width, float height)
{
    if (!BEATS_FLOAT_EQUAL(width, m_size.x) ||
        !BEATS_FLOAT_EQUAL(height, m_size.y))
    {
        m_size.x = width;
        m_size.y = height;
        UPDATE_PROPERTY_PROXY(m_size);
        float fRealWidth = width;
        float fRealHeight = height;
        if (BEATS_FLOAT_EQUAL(width , 0) && 
            BEATS_FLOAT_EQUAL(height , 0))
        {
            CTextureFrag* pTextureFrag = GetTextureFrag();
            if (pTextureFrag && pTextureFrag->Texture())
            {
                const CVec2& textureSize = pTextureFrag->Size();
                fRealWidth = textureSize.x;
                fRealHeight = textureSize.y;
            }
        }
        SetRealSize(fRealWidth, fRealHeight);
    }
}

const kmVec2 &CSprite::Size() const
{
    return m_size;
}

void CSprite::UpdateVertices()
{
    CVec2 realAnchor = m_anchor * m_realSize;

    // Set vertex, lay on X-Z plane.
    m_quad.tl = CVec3(-realAnchor.x, 0, -realAnchor.y);
    CVec2 brPos = m_realSize - realAnchor;
    m_quad.br = CVec3(brPos.x, 0, brPos.y);
    m_quad.tr.x = m_quad.br.x;
    m_quad.tr.y = 0;
    m_quad.tr.z = m_quad.tl.z;
    m_quad.bl.x = m_quad.tl.x;
    m_quad.bl.y = 0;
    m_quad.bl.z = m_quad.br.z;
}

void CSprite::OnTextureFragChanged()
{
    CTextureFrag* pFrag = GetTextureFrag();
    if(pFrag && pFrag->Texture())
    {
        if (BEATS_FLOAT_EQUAL(m_size.x, 0) &&
            BEATS_FLOAT_EQUAL(m_size.y, 0))
        {
            const CVec2& size = pFrag->Size();
            SetRealSize(size.x, size.y);
        }
    }
    if(m_pMaterial)
        m_pMaterial->SetTexture(0, pFrag == NULL ? NULL : pFrag->Texture());
}

void CSprite::SetRealSize(float fWidth, float fHeight)
{
    if (!BEATS_FLOAT_EQUAL(m_realSize.x, fWidth) ||
        !BEATS_FLOAT_EQUAL(m_realSize.y, fHeight))
    {
        m_realSize.x = fWidth;
        m_realSize.y = fHeight;
        UpdateVertices();
    }
}