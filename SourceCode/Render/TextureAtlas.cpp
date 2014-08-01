#include "stdafx.h"
#include "TextureAtlas.h"
#include "Utility/TinyXML/tinyxml.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Resource/Resource.h"
#include "Resource/ResourceManager.h"
#include "TextureFrag.h"
#include "Texture.h"
#include "TextureFragManager.h"

CTextureAtlas::CTextureAtlas()
{

}

CTextureAtlas::~CTextureAtlas()
{

}

bool CTextureAtlas::Load()
{
    BEATS_ASSERT(!IsLoaded());

    TString fileext = CFilePathTool::GetInstance()->Extension(GetFilePath().c_str());
    if(fileext == _T(".xml"))
    {
        TiXmlDocument doc;
        CSerializer serializer;
        CFilePathTool::GetInstance()->LoadFile(&serializer, GetFilePath().c_str(), _T("rb"));
        if (serializer.GetWritePos() != serializer.GetReadPos())
        {
            doc.Parse((char*)serializer.GetReadPtr());
        }
        TiXmlElement *root = doc.RootElement();
        BEATS_ASSERT(root && strcmp(root->Value(), "Imageset") == 0, 
            _T("TextureAtlas file %s not found or incorrect!"), GetFilePath().c_str());

        const char *textureFile = root->Attribute("Imagefile");
        BEATS_ASSERT(textureFile);
        TCHAR szNameBuffer[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToTCHAR(textureFile, szNameBuffer, MAX_PATH);
        m_texture = CResourceManager::GetInstance()->GetResource<CTexture>(szNameBuffer);
        kmVec2 size;
        kmVec2 point;
        TString strName;
        for(TiXmlElement *elemImage = root->FirstChildElement("Image");
            elemImage != nullptr; elemImage = elemImage->NextSiblingElement("Image"))
        {
            const char *name = elemImage->Attribute("Name");
            BEATS_ASSERT(name);
            kmVec2Fill(&point, 0.f, 0.f);
            kmVec2Fill(&size, 0.f, 0.f);
            elemImage->QueryFloatAttribute("XPos", &point.x);
            elemImage->QueryFloatAttribute("YPos", &point.y);
            elemImage->QueryFloatAttribute("Width", &size.x);
            elemImage->QueryFloatAttribute("Height", &size.y);

            CStringHelper::GetInstance()->ConvertToTCHAR(name, szNameBuffer, MAX_PATH);
            strName.assign(szNameBuffer);
            CreateTextureFrag(strName, point, size);
        }
    }
    else
    {
        m_texture = CResourceManager::GetInstance()->GetResource<CTexture>(GetFilePath());
    }

    m_name = CFilePathTool::GetInstance()->FileName(GetFilePath().c_str());
    super::Load();
    return true;
}

void CTextureAtlas::Initialize()
{
    super::Initialize();
    if (!m_texture->IsInitialized())
    {
        m_texture->Initialize();
    }
    for (auto iter = m_textureFrags.begin(); iter != m_textureFrags.end(); ++iter)
    {
        iter->second->Initialize();
    }
    CTextureFragManager::GetInstance()->AddTextureAtlas(this);
}

bool CTextureAtlas::Unload()
{
    if (IsLoaded())
    {
        for(auto frag : m_textureFrags)
        {
            BEATS_SAFE_DELETE(frag.second);
        }
        CTextureFragManager::GetInstance()->RemoveTextureAtlas(m_name);
    }
    super::Unload();
    return true;
}

const TString &CTextureAtlas::Name() const
{
    return m_name;
}

SharePtr<CTexture> CTextureAtlas::Texture() const
{
    return m_texture;
}

CTextureFrag *CTextureAtlas::GetTextureFrag(const TString &name) const
{
    auto itr = m_textureFrags.find(name);
    return itr != m_textureFrags.end() ? itr->second : nullptr;
}

CTextureFrag *CTextureAtlas::CreateTextureFrag(const TString &name, kmVec2 point, kmVec2 size)
{
    BEATS_ASSERT(m_textureFrags.find(name) == m_textureFrags.end());
    SharePtr<CResource> pSelf;
    CResourceManager::GetInstance()->QueryResource(GetFilePath(), pSelf);
    BEATS_ASSERT(pSelf.Get() == this);
    CTextureFrag *frag = new CTextureFrag(name, pSelf, point, size);
    m_textureFrags[name] = frag;
    return frag;
}

