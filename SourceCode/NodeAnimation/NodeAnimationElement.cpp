#include "stdafx.h"
#include "NodeAnimationElement.h"
#include "Scene/Node.h"
#include "Render/Sprite.h"
#include "Render/RenderTarget.h"
#include "Render/RenderManager.h"

CNodeAnimationElement::CNodeAnimationElement()
: m_type(eNAET_Count)
{
}

CNodeAnimationElement::~CNodeAnimationElement()
{

}

void CNodeAnimationElement::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_type, true, 0xffffffff, _T("类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_keyFrames, true, 0xFFFFFFFF, _T("关键帧"), NULL, NULL, NULL);
}

ENodeAnimationElementType CNodeAnimationElement::GetType() const
{
    return m_type;
}

void CNodeAnimationElement::SetType(ENodeAnimationElementType type)
{
    m_type = type;
}

bool CNodeAnimationElement::GetValue(uint32_t uFrame, CVec3& outValue)
{
    bool bRet = false;
    BEATS_ASSERT(m_keyFrames.size() > 0);
    auto preIter = m_keyFrames.end();
    if (uFrame >= m_keyFrames.begin()->first &&
        uFrame <= m_keyFrames.rbegin()->first)
    {
        if (m_keyFrames.find(uFrame) != m_keyFrames.end())
        {
            outValue = m_keyFrames[uFrame];
            bRet = true;
        }
        else
        {
            for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
            {
                if (iter->first > uFrame)
                {
                    if (preIter != m_keyFrames.end())
                    {
                        BEATS_ASSERT(preIter->first < iter->first && uFrame > preIter->first);
                        uint32_t uDelta = iter->first - preIter->first;
                        CVec3 deltaValue = iter->second - preIter->second;
                        outValue = deltaValue * ((float)(uFrame - preIter->first) / uDelta);
                        outValue = preIter->second + outValue;
                        bRet = true;
                        break;
                    }
                }
                preIter = iter;
            }
        }
    }
    else if (uFrame < m_keyFrames.begin()->first)
    {
        outValue = m_keyFrames.begin()->second;
        bRet = true;
    }
    else
    {
        BEATS_ASSERT(uFrame > m_keyFrames.rbegin()->first);
        outValue = m_keyFrames.rbegin()->second;
        bRet = true;
    }
    return bRet;
}

void CNodeAnimationElement::Apply(CNode* pNode, uint32_t uCurrFrame)
{
    BEATS_ASSERT(pNode != NULL);
    CVec3 currValue;
    GetValue(uCurrFrame, currValue);
    pNode->NodeAnimationUpdate(m_type, uCurrFrame, currValue);
#ifdef EDITOR_MODE
    if (pNode->GetProxyComponent())
    {
        const std::vector<CComponentInstance*>& syncNodes = pNode->GetProxyComponent()->GetSyncComponents();
        for (uint32_t i = 0; i < syncNodes.size(); ++i)
        {
            Apply(down_cast<CNode*>(syncNodes[i]), uCurrFrame);
        }
    }
#endif
}

void CNodeAnimationElement::AddKeyFrame(uint32_t uFramePos, const CVec3& data)
{
    BEATS_ASSERT(m_keyFrames.find(uFramePos) == m_keyFrames.end(), _T("Can't add key frame twice at pos %d"), uFramePos);
    m_keyFrames[uFramePos] = data;
}

void CNodeAnimationElement::RemoveKeyFrame(uint32_t uFramePos)
{
    BEATS_ASSERT(m_keyFrames.find(uFramePos) != m_keyFrames.end(), _T("Can't remove key frame at pos %d"), uFramePos);
    m_keyFrames.erase(uFramePos);
}

const std::map<uint32_t, CVec3>& CNodeAnimationElement::GetKeyFrames() const
{
    return m_keyFrames;
}

bool CNodeAnimationElement::SetValue(uint32_t uFrame, CVec3& value)
{
    bool bRet = false;
    BEATS_ASSERT(m_keyFrames.size() > 0);
    BEATS_ASSERT(m_keyFrames.find(uFrame) != m_keyFrames.end());
    if (m_keyFrames.find(uFrame) != m_keyFrames.end())
    {
        m_keyFrames[uFrame] = value;
        bRet = true;
    }
    return bRet;
}
