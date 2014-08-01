#include "stdafx.h"
#include "ReflectTextureInfo.h"
#include "TextureAtlas.h"
#include "Resource/ResourceManager.h"
#include "TextureFrag.h"

SReflectTextureInfo::SReflectTextureInfo()
    : m_pFrag(nullptr)
{
}

SReflectTextureInfo::SReflectTextureInfo( const SReflectTextureInfo& other )
{
    m_pFrag = other.m_pFrag;
}

SReflectTextureInfo::~SReflectTextureInfo()
{
}

void SReflectTextureInfo::SetTextureFrag(const TString &strAtlasName, const TString &strFragName)
{
    SharePtr<CTextureAtlas> pAtlas = CResourceManager::GetInstance()->GetResource<CTextureAtlas>(strAtlasName);
    if (pAtlas && !strFragName.empty())
    {
        m_pFrag = pAtlas->GetTextureFrag(strFragName);
        BEATS_ASSERT(m_pFrag != NULL, _T("Get texutre %s from atals %s failed!"), strFragName.c_str(), strAtlasName.c_str());
    }
    else
    {
        m_pFrag = nullptr;
    }
}

CTextureFrag *SReflectTextureInfo::GetTextureFrag() const
{
    return m_pFrag;
}

SharePtr<CTextureAtlas> SReflectTextureInfo::GetAtlas() const
{
    BEATS_ASSERT(m_pFrag != NULL);
    return m_pFrag->Atlas();
}

void SReflectTextureInfo::Deserialize(CSerializer *pSerializer)
{
    TString strValue;
    *pSerializer >> strValue;
#ifdef _DEBUG
    m_strValue = strValue;
#endif

    std::vector<TString> buf;
    CStringHelper::GetInstance()->SplitString(strValue.c_str(), _T("@"), buf);
    if(buf.size() == 2)
    {
        SetTextureFrag(buf[0], buf[1]);
    }
    else
    {
        m_pFrag = nullptr;
    }
}

void SReflectTextureInfo::Serialize(CSerializer *pSerializer) const
{
    TCHAR szBuffer[MAX_PATH] = {0};
    if (m_pFrag != NULL)
    {
        _stprintf(szBuffer, _T("%s@%s"), m_pFrag->Atlas()->Name().c_str(), m_pFrag->Name().c_str());
    }
    *pSerializer << szBuffer;
}
