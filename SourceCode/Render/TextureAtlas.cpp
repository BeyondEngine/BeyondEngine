#include "stdafx.h"
#include "TextureAtlas.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Resource/Resource.h"
#include "Resource/ResourceManager.h"
#include "TextureFrag.h"
#include "Texture.h"
#include "Framework/Application.h"
#ifdef EDITOR_MODE
std::map<TString, std::map<TString, char>> CTextureAtlas::m_fragCheckList;
std::map<TString, std::set<TString>> CTextureAtlas::m_fragMissingInfo;
#endif
CTextureAtlas::CTextureAtlas()
{

}

CTextureAtlas::~CTextureAtlas()
{
    if (IsInitialized())
    {
        Uninitialize();
    }
}

bool CTextureAtlas::Load()
{
    BEATS_ASSERT(!IsLoaded());
    const TCHAR* pszFilePath = GetFilePath().c_str();
    TString fileext = CFilePathTool::GetInstance()->Extension(pszFilePath);
    if(fileext == _T(".xml"))
    {
        rapidxml::xml_document<> doc;
        CSerializer serializer;
        bool bLoadSuccess = CFilePathTool::GetInstance()->LoadFile(&serializer, pszFilePath, _T("rb"));
        BEATS_ASSERT(bLoadSuccess, _T("Load file %s failed!"), pszFilePath);
        BEYONDENGINE_UNUSED_PARAM(bLoadSuccess);
        if (bLoadSuccess)
        {
            CResourceManager::GetInstance()->DecodeResourceData(serializer);
            if (serializer.GetWritePos() != serializer.GetReadPos())
            {
                serializer << (char)0; //append 0 for the string.
                doc.parse<rapidxml::parse_default>((char*)serializer.GetReadPtr());
            }
            rapidxml::xml_node<> *root = doc.first_node("Imageset");
            BEATS_ASSERT(root != nullptr,
                _T("TextureAtlas file %s not found or incorrect!"), pszFilePath);

            const char *textureFile = root->first_attribute("Imagefile")->value();
            BEATS_ASSERT(textureFile, _T("Texture atlas attribute Imagefile is null in atlas %s"), pszFilePath);
            m_pTexture = CResourceManager::GetInstance()->GetResource<CTexture>(textureFile);
            BEATS_ASSERT(m_pTexture != NULL, _T("Get Texture %s failed in texture atlas file %s"), textureFile, pszFilePath);
            CVec2 size;
            CVec2 point;
            for (rapidxml::xml_node<> *elemImage = root->first_node("Image");
                elemImage != nullptr; elemImage = elemImage->next_sibling("Image"))
            {
                const char *name = elemImage->first_attribute("Name")->value();
                BEATS_ASSERT(name);
                point.Fill(0.f, 0.f);
                size.Fill(0.f, 0.f);
                point.X() = (float)_ttof(elemImage->first_attribute("XPos")->value());
                point.Y() = (float)_ttof(elemImage->first_attribute("YPos")->value());
                size.X() = (float)_ttof(elemImage->first_attribute("Width")->value());
                size.Y() = (float)_ttof(elemImage->first_attribute("Height")->value());
                SharePtr<CTextureFrag> pTextureFrag = CreateTextureFrag(name, point, size);
#ifdef EDITOR_MODE
                if (elemImage->first_attribute("Trimmed") != nullptr)
                {
                    pTextureFrag->m_bTrimmed = TString(elemImage->first_attribute("Trimmed")->value()) == "true";
                    if (pTextureFrag->m_bTrimmed)
                    {
                        pTextureFrag->m_originSize.X() = (float)_ttof(elemImage->first_attribute("SrcWidth")->value());
                        pTextureFrag->m_originSize.Y() = (float)_ttof(elemImage->first_attribute("SrcHeight")->value());
                        pTextureFrag->m_originOffset.X() = (float)_ttof(elemImage->first_attribute("SrcX")->value());
                        pTextureFrag->m_originOffset.Y() = (float)_ttof(elemImage->first_attribute("SrcY")->value());
                    }
                }
#endif
            }
        }
    }
    else
    {
        m_pTexture = CResourceManager::GetInstance()->GetResource<CTexture>(pszFilePath);
        BEATS_ASSERT(m_pTexture != NULL, _T("Get Texture failed in texture atlas file %s"), pszFilePath)
    }

    super::Load();
    return true;
}

void CTextureAtlas::Initialize()
{
    super::Initialize();
    BEATS_ASSERT(m_pTexture != NULL, _T("Texture can't be null for texture atlas: %s"), GetFilePath().c_str());
    if (m_pTexture != NULL)
    {
        if (!m_pTexture->IsInitialized())
        {
            m_pTexture->Initialize();
        }
    }
}

bool CTextureAtlas::Unload()
{
    if (IsLoaded())
    {
        m_textureFrags.clear();
        if (m_pTexture)
        {
            if (m_pTexture->IsInitialized())
            {
                m_pTexture->Uninitialize();
            }
            BEATS_ASSERT(m_pTexture.RefCount() == CResourceManager::MIN_RESOURCE_REF_COUNT);
            CResourceManager::GetInstance()->UnregisterResource(m_pTexture);
        }
        TString strAtlasName = CFilePathTool::GetInstance()->FileName(GetFilePath().c_str());
        BEATS_ASSERT(!strAtlasName.empty());
    }
    super::Unload();
    return true;
}

bool CTextureAtlas::ShouldClean() const
{
    bool bRet = true;
    for (auto iter = m_textureFrags.begin(); iter != m_textureFrags.end(); ++iter)
    {
        if (iter->second.RefCount() > 1)
        {
            bRet = false;
            break;
        }
    }
    return bRet;
}

SharePtr<CTexture> CTextureAtlas::Texture() const
{
    return m_pTexture;
}

SharePtr<CTextureFrag> CTextureAtlas::GetTextureFrag(const TString &name) const
{
    auto itr = m_textureFrags.find(name);
    return itr != m_textureFrags.end() ? itr->second : nullptr;
}

#ifdef EDITOR_MODE
bool CTextureAtlas::NeedReload() const
{
    return super::NeedReload() || m_pTexture->NeedReload();
}

void CTextureAtlas::Reload()
{
    super::Reload();
    Unload();
    Load();
    m_pTexture->Reload();
}

void CTextureAtlas::LoadCheckList()
{
    TString strAtlasFileName = CFilePathTool::GetInstance()->FileName(GetFilePath().c_str());
    if (m_fragCheckList.find(strAtlasFileName) == m_fragCheckList.end())
    {
        rapidxml::xml_document<> doc;
        CSerializer serializer;
        bool bLoadSuccess = CFilePathTool::GetInstance()->LoadFile(&serializer, GetFilePath().c_str(), _T("rb"));
        BEATS_ASSERT(bLoadSuccess, _T("Load file %s failed!"), GetFilePath().c_str());
        BEYONDENGINE_UNUSED_PARAM(bLoadSuccess);
        if (bLoadSuccess)
        {
            CResourceManager::GetInstance()->DecodeResourceData(serializer);
            if (serializer.GetWritePos() != serializer.GetReadPos())
            {
                serializer << (char)0; //append 0 for the string.
                doc.parse<rapidxml::parse_default>((char*)serializer.GetReadPtr());
            }
            rapidxml::xml_node<> *root = doc.first_node("Imageset");
            BEATS_ASSERT(root != nullptr,
                _T("TextureAtlas file %s not found or incorrect!"), GetFilePath().c_str());

            const char *textureFile = root->first_attribute("Imagefile")->value();
            BEATS_ASSERT(textureFile, _T("Texture atlas attribute Imagefile is null in atlas %s"), GetFilePath().c_str());
            m_pTexture = CResourceManager::GetInstance()->GetResource<CTexture>(textureFile);
            BEATS_ASSERT(m_pTexture != NULL, _T("Get Texture %s failed in texture atlas file %s"), textureFile, GetFilePath().c_str());
            CVec2 size;
            CVec2 point;
            
            for (rapidxml::xml_node<> *elemImage = root->first_node("Image");
                elemImage != nullptr; elemImage = elemImage->next_sibling("Image"))
            {
                const char *name = elemImage->first_attribute("Name")->value();
                BEATS_ASSERT(m_fragCheckList.find(name) == m_fragCheckList.end(), "Frag name %s can't repeat in atlas %s", name, GetFilePath().c_str());
                m_fragCheckList[strAtlasFileName][CStringHelper::GetInstance()->ToLower(name)] = 0;
            }
        }
    }
}

#endif


SharePtr<CTextureFrag> CTextureAtlas::CreateTextureFrag(const TString &name, CVec2 point, CVec2 size)
{
    TString lowerName = CStringHelper::GetInstance()->ToLower(name);
    BEATS_ASSERT(m_textureFrags.find(lowerName) == m_textureFrags.end());
    SharePtr<CResource> pSelf;
    CResourceManager::GetInstance()->QueryResource(GetFilePath(), pSelf);
    BEATS_ASSERT(pSelf.Get() == this);
    SharePtr<CTextureFrag> pRet = new CTextureFrag(lowerName, pSelf, point, size);
    m_textureFrags[lowerName] = pRet;
    return pRet;
}

