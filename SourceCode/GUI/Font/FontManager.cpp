#include "stdafx.h"
#include "FontManager.h"
#include "FontFace.h"
#include "Render/Texture.h"
#include "Render/RenderManager.h"
#include "Render/Material.h"
#include "Render/RenderGroup.h"
#include "Render/Shader.h"
#include "Render/RenderState.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderTarget.h"
#include "Render/TextureFrag.h"
#include "Resource/ResourceManager.h"
#include "FreetypeFontFace.h"
#include "BitmapFontFace.h"

CFontManager *CFontManager::m_pInstance = nullptr;

CFontManager::CFontManager()
{

}

CFontManager::~CFontManager()
{
    for(auto face : m_faceMap)
    {
        BEATS_SAFE_DELETE(face.second);
    }
}
CFontFace *CFontManager::CreateFreetypeFontFace( const TString &name, const TString &file, int sizeInPt, int dpi, bool bRegister )
{
    CFontFace *face = NULL;
    auto it = m_faceMap.find(name);
    if (it == m_faceMap.end())
    {
        face = new CFreetypeFontFace(name, file, sizeInPt, dpi);
        if (bRegister)
        {
            m_faceMap[name] = face;
        }
    }
    else
    {
        face = it->second;
    }
    face->SetFontType(e_free_type_font);
    return face;
}

CFontFace *CFontManager::CreateBitmapFontFace(const TString &filename)
{
    CBitmapFontFace *face = new CBitmapFontFace(filename);
    BEATS_ASSERT(m_faceMap.find(face->GetName()) == m_faceMap.end(),
        _T("Font face:%s already existed"), face->GetName().c_str());
    BEATS_ASSERT(!face->GetName().empty());
    m_faceMap[face->GetName()] = face;
    face->SetFontType(e_bitmap_font);
    return face;
}

CFontFace *CFontManager::GetFace( const TString &name )
{
    auto itr = m_faceMap.find(name);
    BEATS_ASSERT(itr != m_faceMap.end(), "Get face %s failed!", name.c_str());
    return itr != m_faceMap.end() ? itr->second : nullptr;
}

void CFontManager::Clear()
{
    for(auto facePair : m_faceMap)
    {
        facePair.second->Clear();
    }
}

const std::map<TString, CFontFace *>& CFontManager::GetFontFaceMap() const
{
    return m_faceMap;
}