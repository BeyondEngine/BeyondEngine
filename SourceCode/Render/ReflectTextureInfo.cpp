#include "stdafx.h"
#include "ReflectTextureInfo.h"
#include "TextureAtlas.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

SReflectTextureInfo::SReflectTextureInfo()
    : m_pFrag(nullptr)
{
}

SReflectTextureInfo::~SReflectTextureInfo()
{
}

SReflectTextureInfo& SReflectTextureInfo::operator = (const SReflectTextureInfo& rhs)
{
#ifdef DELAY_LOAD_TEXTURE
    m_strAtlasName = rhs.m_strAtlasName;
    m_strFragName = rhs.m_strFragName;
#endif
    m_pFrag = rhs.GetTextureFrag();
    return *this;
}

bool SReflectTextureInfo::SetTextureFrag(const TString &strAtlasName, const TString &strFragName)
{
#ifdef DELAY_LOAD_TEXTURE
    m_strAtlasName = strAtlasName;
    m_strFragName = strFragName;
#ifdef DISABLE_RENDER
    m_strAtlasName = "name.xml";
    m_strFragName = "name_confirm";
#endif
    return !m_strAtlasName.empty() && !m_strFragName.empty();
#else
    SharePtr<CTextureAtlas> pAtlas = CResourceManager::GetInstance()->GetResource<CTextureAtlas>(strAtlasName);
    bool bRet = pAtlas && !strFragName.empty();
    if (bRet)
    {
        m_pFrag = pAtlas->GetTextureFrag(strFragName);
    }
    else
    {
        m_pFrag = nullptr;
    }
    return bRet;
#endif
}

void SReflectTextureInfo::SetTextureFrag(SharePtr<CTextureFrag> pFrag)
{
    m_pFrag = pFrag;
}

SharePtr<CTextureFrag> SReflectTextureInfo::GetTextureFrag() const
{
#ifdef DELAY_LOAD_TEXTURE
    if (m_pFrag == nullptr &&
        !m_strAtlasName.empty() &&
        !m_strFragName.empty())
    {
        SharePtr<CTextureAtlas> pAtlas = CResourceManager::GetInstance()->GetResource<CTextureAtlas>(m_strAtlasName);
        bool bRet = pAtlas && !m_strFragName.empty();
        if (bRet)
        {
            m_pFrag = pAtlas->GetTextureFrag(m_strFragName);
            BEATS_ASSERT(m_pFrag != nullptr);
        }
        else
        {
            m_pFrag = nullptr;
        }
    }
#endif
    return m_pFrag;
}

void SReflectTextureInfo::Deserialize(CSerializer *pSerializer, CComponentInstance* pOwner)
{
    TString strValue;
    *pSerializer >> strValue;
#ifdef _DEBUG
    m_strValue = strValue;
#endif
    std::vector<TString> buf;
    BEATS_ASSERT(CStringHelper::GetInstance()->FindFirstString(strValue.c_str(), " ", false) == TString::npos, "texture name should not contain space in %s", strValue.c_str());
    CStringHelper::GetInstance()->SplitString(strValue.c_str(), _T("@"), buf, false);
    if(buf.size() == 2)
    {
        bool bRet = SetTextureFrag(buf[0], buf[1]);
        BEYONDENGINE_UNUSED_PARAM(bRet);
        BEYONDENGINE_UNUSED_PARAM(pOwner);
#ifndef DELAY_LOAD_TEXTURE
        uint32_t uId = pOwner->GetId();
        if (pOwner->GetReflectOwner())
        {
            uId = pOwner->GetReflectOwner()->GetId();
        }
        BEATS_ASSERT(!bRet || m_pFrag != NULL, _T("Get texutre %s from atals %s failed!\nat component %s id:%d "), buf[0].c_str(), buf[1].c_str(), pOwner->GetClassStr(), uId);
#endif
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
        TString strAtlasName = CFilePathTool::GetInstance()->FileName(m_pFrag->GetAtlas()->GetFilePath().c_str());
        BEATS_ASSERT(!strAtlasName.empty());
        _stprintf(szBuffer, _T("%s@%s"), strAtlasName.c_str(), m_pFrag->GetName().c_str());
    }
    *pSerializer << szBuffer;
}