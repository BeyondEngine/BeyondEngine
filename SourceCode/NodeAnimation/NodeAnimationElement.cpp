#include "stdafx.h"
#include "NodeAnimationElement.h"
#include "Scene\Node.h"
#include "GUI\Window\Control.h"
#include "Render\Sprite.h"

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
    DECLARE_PROPERTY(serializer, m_type, true, 0xffffffff, _T("ÀàÐÍ"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_keyFrames, true, 0xFFFFFFFF, _T("¹Ø¼üÖ¡"), NULL, NULL, NULL);
}

ENodeAnimationElementType CNodeAnimationElement::GetType() const
{
    return m_type;
}

bool CNodeAnimationElement::GetValue(size_t uFrame, CVec3& outValue)
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
                        size_t uDelta = iter->first - preIter->first;
                        CVec3 deltaValue = iter->second - preIter->second;
                        kmVec3Scale(&outValue, &deltaValue, (float)(uFrame - preIter->first) / uDelta);
                        outValue = preIter->second + outValue;
                        bRet = true;
                        break;
                    }
                }
                preIter = iter;
            }
        }
    }
    return bRet;
}

void CNodeAnimationElement::Apply(CNode* pNode, size_t uCurrFrame)
{
    BEATS_ASSERT(pNode != NULL);
    CVec3 currValue;
    if(GetValue(uCurrFrame, currValue))
    {
        SAnimationProperty* pProperty = pNode->GetAnimationProperty();
        if (pProperty == NULL)
        {
            pProperty = new SAnimationProperty;
            pNode->SetAnimationProperty(pProperty);
        }
        switch (m_type)
        {
        case eNAET_Scale:
            pProperty->m_scaleForAnimation = currValue;
            break;
        case eNAET_Pos:
            pProperty->m_posForAnimation = currValue;
            break;
        case eNAET_Rotation:
            pProperty->m_rotationForAnimation = currValue;
            break;
        case eNAET_UIColor:
            {
#ifdef EDITOR_MODE
                CControl* pControl = dynamic_cast<CControl*>(pNode);
                BEATS_ASSERT( pControl , _T("the CControl can't be null"));
                if (pControl == NULL)
                {
                    break;
                }
#else
                BEATS_ASSERT(dynamic_cast<CControl*>(pNode) != NULL);
                CControl* pControl = static_cast<CControl*>(pNode);
#endif
                CColor color = pControl->GetColor();
                color.r = (unsigned char)currValue.x;
                color.g = (unsigned char)currValue.y;
                color.b = (unsigned char)currValue.z;
                pControl->SetColor(color);
            }
            break;
        case eNAET_UIAlpha:
            {
                BEATS_ASSERT(dynamic_cast<CControl*>(pNode) != NULL);
#ifdef EDITOR_MODE
                CControl* pControl = dynamic_cast<CControl*>(pNode);
                BEATS_ASSERT( pControl , _T("the CControl can't be null"));
                if (pControl == NULL)
                {
                    break;
                }
#else
                CControl* pControl = static_cast<CControl*>(pNode);
#endif
                CColor color = pControl->GetColor();
                color.a = (unsigned char)currValue.x;
                pControl->SetColor(color);
            }
            break;
        case eNAET_ColorScale:
            {
                CColor color;
                color.r = (unsigned char)currValue.x;
                color.g = (unsigned char)currValue.y;
                color.b = (unsigned char)currValue.z;
                color.a = 100;//means no scale in alpha for now.
                pNode->SetColorScale(color);
            }
            break;
        default:
            BEATS_ASSERT(false);
            break;
        }
    }
#ifdef EDITOR_MODE
    if (pNode->GetProxyComponent())
    {
        const std::vector<CComponentInstance*>& syncNodes = pNode->GetProxyComponent()->GetSyncComponents();
        for (size_t i = 0; i < syncNodes.size(); ++i)
        {
            Apply((CNode*)syncNodes[i], uCurrFrame);
        }
    }
#endif
}

void CNodeAnimationElement::AddKeyFrame(size_t uFramePos, const CVec3& data)
{
    BEATS_ASSERT(m_keyFrames.find(uFramePos) == m_keyFrames.end(), _T("Can't add key frame twice at pos %d"), uFramePos);
    m_keyFrames[uFramePos] = data;
}

void CNodeAnimationElement::RemoveKeyFrame(size_t uFramePos)
{
    BEATS_ASSERT(m_keyFrames.find(uFramePos) != m_keyFrames.end(), _T("Can't remove key frame at pos %d"), uFramePos);
    m_keyFrames.erase(uFramePos);
}

const std::map<size_t, CVec3>& CNodeAnimationElement::GetKeyFrames() const
{
    return m_keyFrames;
}
