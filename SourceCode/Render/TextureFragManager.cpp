#include "stdafx.h"
#include "TextureFragManager.h"
#include "TextureFrag.h"
#include "Texture.h"
#include "TextureAtlas.h"
#include "Resource/ResourcePathManager.h"

CTextureFragManager *CTextureFragManager::m_pInstance = nullptr;

CTextureFragManager::CTextureFragManager()
{

}

CTextureFragManager::~CTextureFragManager()
{
    for(auto atlas : m_managedAtlases)
    {
        atlas->Unload();
        BEATS_SAFE_DELETE(atlas);
    }
}

CTextureFrag *CTextureFragManager::GetTextureFrag(const TString &atlasName, const TString &fragName)
{
    auto itr = m_textureAtlases.find(atlasName);
    return itr != m_textureAtlases.end() ? itr->second->GetTextureFrag(fragName) : nullptr;
}

CTextureAtlas *CTextureFragManager::CreateTextureAtlas(const TString &textureFileName)
{
    CTextureAtlas *atlas = new CTextureAtlas;
    TString strPath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Texture);
    strPath.append(_T("/")).append(textureFileName.c_str());
    atlas->SetFilePath( strPath );
    atlas->Load();
    m_managedAtlases.insert(atlas);
    return atlas;
}

void CTextureFragManager::AddTextureAtlas(CTextureAtlas *atlas)
{
    BEATS_ASSERT(m_textureAtlases.find(atlas->Name()) == m_textureAtlases.end());
    m_textureAtlases[atlas->Name()] = atlas;
}

void CTextureFragManager::RemoveTextureAtlas(const TString &atlasName)
{
    m_textureAtlases.erase(atlasName);
}
