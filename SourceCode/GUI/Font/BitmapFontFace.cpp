#include "stdafx.h"
#include "BitmapFontFace.h"
#include "Resource/ResourcePathManager.h"
#include "BeatsUtility/FilePathTool.h"
#include "TinyXML/tinyxml.h"
#include "Utf8String.h"
#include "Resource/ResourceManager.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderManager.h"
#include "Render/VertexFormat.h"
#include "Render/RenderBatch.h"
#include "Render/TextureFrag.h"
#include "Render/Texture.h"
#include "Render/RenderGroup.h"
#include "Render/Material.h"
#include "Render/RenderState.h"
#include "FontManager.h"

CBitmapFontFace::CBitmapFontFace(const TString &filename)
    : CFontFace(filename)
    , m_fDefaultWidth(0.f)
    , m_fDefaultHeight(0.f)
{
    LoadFontFile(filename);
}

CBitmapFontFace::~CBitmapFontFace()
{

}

void CBitmapFontFace::LoadFontFile(const TString &filename)
{
    // Load From File
    TString strResourcePath = CResourcePathManager::GetInstance()->GetResourcePath
        (CResourcePathManager::eRPT_Font);
    strResourcePath.append(_T("/"));
    strResourcePath += filename;
    TCHAR szBuffer[MAX_PATH];
    CFilePathTool::GetInstance()->Canonical(szBuffer, strResourcePath.c_str());

    TiXmlDocument doc;
    CSerializer serializer;
    CFilePathTool::GetInstance()->LoadFile(&serializer, szBuffer, _T("rb"));
    if (serializer.GetWritePos() != serializer.GetReadPos())
    {
        doc.Parse((char*)serializer.GetReadPtr());
    }
    TiXmlElement *root = doc.RootElement();
    BEATS_ASSERT(root && strcmp(root->Value(), "Font") == 0, 
        _T("Font file %s not found or incorrect!"), szBuffer);

    const char *name = root->Attribute("Name");
    BEATS_ASSERT(name);
    m_strName = name;
    root->QueryFloatAttribute("DefaultWidth", &m_fDefaultWidth);
    root->QueryFloatAttribute("DefaultHeight", &m_fDefaultHeight);

    for(TiXmlElement *elemImage = root->FirstChildElement("Glyph");
        elemImage != nullptr; elemImage = elemImage->NextSiblingElement("Glyph"))
    {
        const char *strChar = elemImage->Attribute("Char");
        BEATS_ASSERT(strChar);
        std::wstring strUniChar = Utf8ToWString(strChar);
        BEATS_ASSERT(strUniChar.size() == 1);
        unsigned long code = static_cast<unsigned long>(strUniChar[0]);
        const char *strAtlas = elemImage->Attribute("Atlas");
        BEATS_ASSERT(strAtlas);
        const char *strFrag = elemImage->Attribute("SubTexture");
        BEATS_ASSERT(strFrag);
        SharePtr<CTextureAtlas> pAtlas = CResourceManager::GetInstance()->
            GetResource<CTextureAtlas>(strAtlas);
        BEATS_ASSERT(pAtlas);
        CTextureFrag *pFrag = pAtlas->GetTextureFrag(strFrag);
        BEATS_ASSERT(pFrag);

        CBitmapFontGlyph *pGlyph = new CBitmapFontGlyph;
        pGlyph->SetWidth(m_fDefaultWidth, false);
        pGlyph->SetHeight(m_fDefaultHeight, false);
        pGlyph->pFrag = pFrag;
        m_glyphMap.insert(std::make_pair(code, pGlyph));

        auto itr = m_materialMap.find(pAtlas->Texture()->ID());
        if(itr == m_materialMap.end())
        {
            SharePtr<CMaterial> material = CFontManager::GetInstance()->CreateDefaultMaterial(true);
            material->SetTexture(0, pAtlas->Texture());
            m_materialMap[pAtlas->Texture()->ID()] = material;
        }
    }
}

const CFontGlyph *CBitmapFontFace::PrepareChar(wchar_t character)
{
    auto itr = m_glyphMap.find(character);
    return itr != m_glyphMap.end() ? itr->second : nullptr;
}

void CBitmapFontFace::DrawGlyph(const CFontGlyph *glyph, float x, float y, float fFontSize, CColor color, CColor borderColor,
                                const kmMat4 *transform, bool bGUI, const CRect *pRect ) const
{
    const CBitmapFontGlyph *pGlyph = down_cast<const CBitmapFontGlyph *>(glyph);
    CQuadP quad;
    quad.tl.x = x;
    quad.tl.y = y;
    quad.tr.x = x + pGlyph->GetWidth(false) * fFontSize;
    quad.tr.y = quad.tl.y;
    quad.bl.x = quad.tl.x;
    quad.bl.y = y + pGlyph->GetHeight(false) * fFontSize;
    quad.br.x = quad.tr.x;
    quad.br.y = quad.bl.y;

    SharePtr<CMaterial> pMaterial;
    if(!pRect)
    {
        auto itr = m_materialMap.find(pGlyph->pFrag->Texture()->ID());
        BEATS_ASSERT(itr != m_materialMap.end());
        pMaterial = itr->second;
    }
    else
    {
        pMaterial = CFontManager::GetInstance()->CreateDefaultMaterial(false);
        pMaterial->SetTexture(0, pGlyph->pFrag->Texture());
        pMaterial->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_ScissorTest, true);
        pMaterial->GetRenderState()->SetScissorRect(pRect->position.x, pRect->position.y, pRect->size.x, pRect->size.y);
    }

    CRenderBatch *batch = (bGUI ? CFontManager::GetInstance()->GetRenderGroupGUI()
                                : CFontManager::GetInstance()->GetRenderGroup2D() )
                                ->GetRenderBatch(VERTEX_FORMAT(CVertexPTC), pMaterial, GL_TRIANGLES, true );

    batch->AddQuad(&quad, &pGlyph->pFrag->Quad(), color, transform);
}
