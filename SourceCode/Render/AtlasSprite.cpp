#include "stdafx.h"
#include "AtlasSprite.h"
#include "RenderManager.h"
#ifdef EDITOR_MODE
#include "BeyondEngineEditor/MapPropertyDescription.h"
#endif

CAtlasSprite::CAtlasSprite()
: m_nCurrRenderFrame(0)
{

}

CAtlasSprite::~CAtlasSprite()
{
}

bool CAtlasSprite::Load()
{
    for (size_t i = 0; i < m_childList.size(); ++i)
    {
        ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_childList[i] && m_childList[i]->GetParentSprite() == nullptr);
        AddChildSprite(m_childList[i]);
        ASSUME_VARIABLE_IN_EDITOR_END
    }
    for (auto iter = m_spriteFramesMap.begin(); iter != m_spriteFramesMap.end(); ++iter)
    {
        ASSUME_VARIABLE_IN_EDITOR_BEGIN(iter->second && iter->second->GetParentSprite() == nullptr);
        AddChildSprite(iter->second);
        ASSUME_VARIABLE_IN_EDITOR_END
    }
    return super::Load();
}

void CAtlasSprite::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_spriteFramesMap, true, 0xFFFFFFFF, _T("精灵帧"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_childList, true, 0xFFFFFFFF, _T("子序列表"), NULL, NULL, NULL);
    HIDE_PROPERTY(m_frames);
}

void CAtlasSprite::Update(float dtt)
{
    super::Update(dtt);
    int uCurrFrame = GetCurrFrame();
    if (m_spriteFramesMap.find(uCurrFrame) != m_spriteFramesMap.end())
    {
        m_nCurrRenderFrame = uCurrFrame;
    }
    CSprite* pSprite = GetCurrentSprite();
    if (pSprite != NULL)
    {
        BEATS_ASSERT(pSprite->GetParentSprite() == this);
        pSprite->Update(dtt);
    }
    for (auto iter : m_childList)
    {
        ASSUME_VARIABLE_IN_EDITOR_BEGIN(iter != NULL)
            BEATS_ASSERT(iter->GetParentSprite() == this);
            iter->SetCurrFrame(uCurrFrame);
            iter->Update(dtt);
        ASSUME_VARIABLE_IN_EDITOR_END
    }
}

void CAtlasSprite::DoRender()
{
    if (IsVisible(true))
    {
        CSprite* pSprite = GetCurrentSprite();
        if (pSprite != NULL)
        {
            pSprite->DoRender();
        }

        for (auto iter : m_childList)
        {
            if (iter != NULL)
            {
                iter->DoRender();
            }
        }
#ifdef DEVELOP_VERSION
        if (CRenderManager::GetInstance()->m_bRenderSpriteUserDefinePos)
        {
            RenderUserDefinePos();
        }
#endif
    }
}

CSprite* CAtlasSprite::GetCurrentSprite() const
{
    CSprite* pSprite = NULL;
    if (m_spriteFramesMap.size() > 0
        && m_nCurrRenderFrame >= m_spriteFramesMap.begin()->first
        && m_nCurrRenderFrame <= m_spriteFramesMap.rbegin()->first)
    {
        uint32_t uCurrFrame = GetCurrFrame();
        auto currFrameIter = m_spriteFramesMap.find(uCurrFrame);
        if (currFrameIter == m_spriteFramesMap.end())
        {
            auto currRenderFrameIter = m_spriteFramesMap.find(m_nCurrRenderFrame);
            pSprite = currRenderFrameIter == m_spriteFramesMap.end() ? NULL : currRenderFrameIter->second;
        }
        else
        {
            pSprite = currFrameIter->second;
            BEATS_ASSERT(pSprite != NULL);
        }
    }
    return pSprite;
}

const std::map<int, CSprite*>& CAtlasSprite::GetSpriteFrameMap() const
{
    return m_spriteFramesMap;
}

std::vector<CAtlasSprite*>& CAtlasSprite::GetChildAtlasSprite()
{
    return m_childList;
}

uint32_t CAtlasSprite::GetRandomPos() const
{
    uint32_t uRet = 0;
    if (m_spriteFramesMap.size() > 0)
    {
        uint32_t uIndex = rand() % m_spriteFramesMap.size();
        uint32_t i = 0;
        for (auto iter : m_spriteFramesMap)
        {
            if (i == uIndex)
            {
                uRet = iter.first;
                break;
            }
            i++;
        }
    }
    return uRet;
}

CSprite* CAtlasSprite::GetSpriteFrame(int nFrameIndex)
{
    CSprite* pRet = NULL;
    if (m_spriteFramesMap.find(nFrameIndex) != m_spriteFramesMap.end())
    {
        pRet = m_spriteFramesMap[nFrameIndex];
    }
    return pRet;
}

void CAtlasSprite::SyncCurFrameTexture()
{
    uint32_t uCurrFrame = GetCurrFrame();
    if (m_spriteFramesMap.find(uCurrFrame) != m_spriteFramesMap.end())
    {
        m_nCurrRenderFrame = (int)uCurrFrame;
    }
    for (auto iter : m_childList)
    {
        if (iter != NULL)
        {
            iter->SetCurrFrame(uCurrFrame);
            iter->SyncCurFrameTexture();
        }
    }
}

SharePtr<CTextureFrag> CAtlasSprite::GetTextureFrag() const
{
    SharePtr<CTextureFrag> pRet = super::GetTextureFrag();
    if (GetCurrentSprite() != nullptr)
    {
        pRet = GetCurrentSprite()->GetTextureFrag();
    }
    return pRet;
}

CRect CAtlasSprite::GetSelfRect() const
{
    CRect rect;
    CSprite* pSprite = GetCurrentSprite();
    if( pSprite )
    {
        rect = pSprite->GetSelfRect();
    }
    return rect;
}

void CAtlasSprite::GetCurrentRenderSpriteList(std::vector<CSprite*>& spriteList) const
{
    CSprite* pSprite = GetCurrentSprite();
    if (pSprite)
    {
        spriteList.push_back(pSprite);
    }
    for (auto child : m_childList)
    {
        if( child->IsVisible() )
        {
            spriteList.push_back(child);
        }
    }
}

void CAtlasSprite::UpdateFrameCount()
{
    m_uFrameCount = m_spriteFramesMap.size() > 0 ? m_spriteFramesMap.rbegin()->first + 1 : 0;
    for (auto iter : m_childList)
    {
        ASSUME_VARIABLE_IN_EDITOR_BEGIN(iter != NULL)
        uint32_t uCount = iter->GetFrameCount();
        if (m_uFrameCount < uCount)
        {
            m_uFrameCount = uCount;
        }
        ASSUME_VARIABLE_IN_EDITOR_END
    }
}
#ifdef EDITOR_MODE
bool CAtlasSprite::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bHandled = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bHandled)
    {
        if (pVariableAddr == &m_spriteFramesMap)
        {
            DeserializeVariable(m_spriteFramesMap, pSerializer, this);
            UpdateFrameCount();
            for (auto iter : m_spriteFramesMap)
            {
                if (iter.second != NULL && iter.second->GetParentSprite() == NULL)
                {
                    AddChildSprite(iter.second);
                }
            }
            bHandled = true;
        }
        if (pVariableAddr == &m_childList)
        {
            DeserializeVariable(m_childList, pSerializer, this);
            UpdateFrameCount();
            for (auto iter : m_childList)
            {
                if (iter != NULL && iter->GetParentSprite() == NULL)
                {
                    AddChildSprite(iter);
                }
            }
            bHandled = true;
        }
    }
    return bHandled;
}
#endif
bool CAtlasSprite::HitTest(const CVec2& pt)
{
    bool bRet = super::HitTest(pt);
    if (!bRet)
    {
        auto iter =m_spriteFramesMap.find(m_nCurrRenderFrame);
        if (iter != m_spriteFramesMap.end())
        {
            bRet = iter->second->HitTest(pt);
        }
    }
    if (!bRet)
    {
        for (size_t i = 0; i < m_childList.size(); ++i)
        {
            bRet = m_childList[i]->HitTest(pt);
            if (bRet)
            {
                break;
            }
        }
    }
    return bRet;
}

#ifdef EDITOR_MODE
void CAtlasSprite::SetSyncProxyComponent(CComponentProxy* pProxy)
{
    super::SetSyncProxyComponent(pProxy);
    if (pProxy)
    {
        CMapPropertyDescription* pMapProperty = down_cast<CMapPropertyDescription*>(pProxy->GetProperty("m_spriteFramesMap"));
        BEATS_ASSERT(pMapProperty->GetChildren().size() == m_spriteFramesMap.size());
        for (auto iter = m_spriteFramesMap.begin(); iter != m_spriteFramesMap.end(); ++iter)
        {
            for (size_t i = 0; i < pMapProperty->GetChildren().size(); ++i)
            {
                int* pCurrValue = (int*)pMapProperty->GetChildren()[i]->GetChildren()[0]->GetValue(eVT_CurrentValue);
                if (*pCurrValue == iter->first)
                {
                    CComponentProxy* pMapValueProxy = pMapProperty->GetChildren()[i]->GetChildren()[1]->GetInstanceComponent();
                    iter->second->SetSyncProxyComponent(pMapValueProxy);
                    break;
                }
            }
        }
        CListPropertyDescription* pListProperty = down_cast<CListPropertyDescription*>(pProxy->GetProperty("m_childList"));
        BEATS_ASSERT(pListProperty->GetChildren().size() == m_childList.size());
        for (size_t i = 0; i < m_childList.size(); ++i)
        {
            m_childList[i]->SetSyncProxyComponent(pListProperty->GetChildren()[i]->GetInstanceComponent());
        }
    }
}
#endif

void CAtlasSprite::SetCurrFrame(uint32_t currFrame)
{
    super::SetCurrFrame(currFrame);
    SyncCurFrameTexture();
}

