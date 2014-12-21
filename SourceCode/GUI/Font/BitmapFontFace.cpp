#include "stdafx.h"
#include "BitmapFontFace.h"
#include "BeatsUtility/FilePathTool.h"

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
    m_strFontName = filename;
    LoadFontFile(m_strFontName);
}

CBitmapFontFace::~CBitmapFontFace()
{
    CleanFontData();
}

void CBitmapFontFace::LoadFontFile(const TString &filename)
{
    // Load From File
    TString strResourcePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Font);
    strResourcePath.append(_T("/"));
    strResourcePath += filename;
    TCHAR szBuffer[MAX_PATH];
    CFilePathTool::GetInstance()->Canonical(szBuffer, strResourcePath.c_str());
    CSerializer serialier;
    CFilePathTool::GetInstance()->LoadFile(&serialier, szBuffer, "rb");
    char manualFileEndDataForString = 0;
    serialier << manualFileEndDataForString;
    rapidxml::xml_document<> doc;
   try
   {
       doc.parse<rapidxml::parse_default>((char*)serialier.GetBuffer());
       rapidxml::xml_node<> *root = doc.first_node("Font");
       BEATS_ASSERT(root != nullptr,
           _T("Font file %s not found or incorrect!"), szBuffer);

       const char *name = root->first_attribute("Name")->value();
       BEATS_ASSERT(name);
       m_strName = name;
       m_fDefaultWidth = (float)_ttof(root->first_attribute("DefaultWidth")->value());
       m_fDefaultHeight = (float)_ttof(root->first_attribute("DefaultHeight")->value());

       for (rapidxml::xml_node<> *pElemImage = root->first_node("Glyph");
           pElemImage != nullptr; pElemImage = pElemImage->next_sibling("Glyph"))
       {
           const char *strChar = pElemImage->first_attribute("Char")->value();
           BEATS_ASSERT(strChar);
           std::wstring strUniChar = CStringHelper::GetInstance()->Utf8ToWString(strChar);
           BEATS_ASSERT(strUniChar.size() == 1);
           uint32_t code = static_cast<uint32_t>(strUniChar[0]);
           const char *strAtlas = pElemImage->first_attribute("Atlas")->value();
           BEATS_ASSERT(strAtlas);
           const char *strFrag = pElemImage->first_attribute("SubTexture")->value();
           BEATS_ASSERT(strFrag);
#ifdef DISABLE_RENDER
           strAtlas = "name.xml";
           strFrag = "name_confirm";
#endif
           SharePtr<CTextureAtlas> pAtlas = CResourceManager::GetInstance()->GetResource<CTextureAtlas>(strAtlas);
           BEATS_ASSERT(pAtlas);

           CBitmapFontGlyph *pGlyph = new CBitmapFontGlyph;
           pGlyph->pFrag = pAtlas->GetTextureFrag(strFrag);
           BEATS_ASSERT(pGlyph->pFrag, "bitmap font frag not found");
           pGlyph->SetWidth(pGlyph->pFrag->GetSize().X(), false);
           pGlyph->SetHeight(pGlyph->pFrag->GetSize().Y(), false);
           pGlyph->SetWidth(pGlyph->pFrag->GetSize().X(), true);
           pGlyph->SetHeight(pGlyph->pFrag->GetSize().Y(), true);
           m_glyphMapLocker.lock();
           m_glyphMap.insert(std::make_pair(code, pGlyph));
           m_glyphMapLocker.unlock();
           m_pTexture = pAtlas->Texture();
       }
   }
   catch (rapidxml::parse_error &e)
   {
       BEATS_ASSERT(false, _T("Load config file %s faled!/n%s/n"), szBuffer, e.what());
       BEYONDENGINE_UNUSED_PARAM(e);
   }
}

const CFontGlyph *CBitmapFontFace::PrepareChar(wchar_t character, bool& bGlyphReset)
{
    bGlyphReset = false;
    const CFontGlyph* pRet = nullptr;
    m_glyphMapLocker.lock();
    auto itr = m_glyphMap.find(character);
    if (itr != m_glyphMap.end())
    {
        pRet = itr->second;
    }
#ifdef EDITOR_MODE
    else
    {
        itr = m_glyphMap.begin();
        pRet = itr->second;
    }
#endif
    m_glyphMapLocker.unlock();
    return pRet;
}

void CBitmapFontFace::DrawGlyph(CRenderBatch* pBatch, const CFontGlyph *glyph, float x, float y, float fFontSize, const CColor& color, const CColor& borderColor,
    const CMat4 *transform, const CRect *pRect, float fAlphaScale ) const
{
    BEATS_ASSERT(pBatch);
    const CBitmapFontGlyph *pGlyph = down_cast<const CBitmapFontGlyph *>(glyph);
    CQuadP quad;
    quad.tl.X() = x;
    quad.tl.Y() = y;
    quad.tr.X() = x + pGlyph->GetWidth(false) * fFontSize;
    quad.tr.Y() = quad.tl.Y();
    quad.bl.X() = quad.tl.X();
    quad.bl.Y() = y + pGlyph->GetHeight(false) * fFontSize;
    quad.br.X() = quad.tr.X();
    quad.br.Y() = quad.bl.Y();
    CColor realColor = color;
    realColor.a = (unsigned char)((float)color.a * fAlphaScale);
    BEATS_ASSERT(!pRect);
    pBatch->AddQuad(&quad, &pGlyph->pFrag->GetQuadT(), realColor, borderColor, transform);
}

CRenderBatch* CBitmapFontFace::GetRenderBatch(ERenderGroupID renderGroupId) const
{
    CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(renderGroupId);
    SharePtr<CMaterial> pMaterial = CRenderManager::GetInstance()->GetImageMaterial();
    std::map<unsigned char, SharePtr<CTexture> > textureMap;
    textureMap[0] = m_pTexture;
    CRenderBatch* pRet = pRenderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPTCC), pMaterial, GL_TRIANGLES, true, true, &textureMap);
    BEATS_ASSERT(pRet != nullptr);
    return pRet;
}

void CBitmapFontFace::Reload()
{
    CleanFontData();
    LoadFontFile(m_strFontName);
}

void CBitmapFontFace::CleanFontData()
{
    m_glyphMapLocker.lock();
    BEATS_SAFE_DELETE_MAP(m_glyphMap);
    m_glyphMapLocker.unlock();
    m_pTexture = nullptr;
}
