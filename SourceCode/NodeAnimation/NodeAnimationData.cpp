#include "stdafx.h"
#include "NodeAnimationData.h"
#include "NodeAnimationElement.h"
#include "NodeAnimationManager.h"
#include "Scene/Node.h"
#ifdef EDITOR_MODE
#include "BeyondEngineEditor/MapPropertyDescription.h"
#include "BeyondEngineEditor/Vec3fPropertyDescription.h"
#endif
CNodeAnimationData::CNodeAnimationData()
    : m_uFrameCount(0)
    , m_uTotalTimeMS(0)
{

}

CNodeAnimationData::~CNodeAnimationData()
{
    if (GetId() == 0xFFFFFFFF)
    {
        BEATS_SAFE_DELETE_VECTOR(m_elements);
    }
}

bool CNodeAnimationData::Load()
{
    bool bRet = super::Load();
#ifndef DISABLE_NODE_ANIMATION
    if (!m_strName.empty())
    {
        BEATS_ASSERT(!m_bTempDataFlag);
        CNodeAnimationManager::GetInstance()->RegisterNodeAnimationData(this);
    }
#endif
    return bRet;
}

bool CNodeAnimationData::Unload()
{
    bool bRet = super::Unload();
#ifndef DISABLE_NODE_ANIMATION
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(!GetName().empty())
        CNodeAnimationManager::GetInstance()->UnregisterNodeAnimationData(this);
    ASSUME_VARIABLE_IN_EDITOR_END
#endif
    return bRet;
}

void CNodeAnimationData::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strName, true, 0xFFFFFFFF, _T("名称"), NULL, _T("名称必须是唯一的"), NULL);
    DECLARE_PROPERTY(serializer, m_uFrameCount, true, 0xFFFFFFFF, _T("总帧数"), NULL, NULL, _T("MinValue:0"));  //TODO:: Remove
    DECLARE_PROPERTY(serializer, m_uTotalTimeMS, true, 0xFFFFFFFF, _T("总时间(毫秒)"), NULL, NULL, _T("MinValue:0"));
    DECLARE_PROPERTY(serializer, m_elements, true, 0xFFFFFFFF, _T("动画元素"), NULL, NULL, NULL);
}
#ifdef EDITOR_MODE
bool CNodeAnimationData::OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pNewValueToBeSet);
    if (!bRet)
    {
        if (pVariableAddr == &m_strName)
        {
            TString strName;
            DeserializeVariable(strName, pNewValueToBeSet, this);
            if (!m_strName.empty())
            {
                CNodeAnimationManager::GetInstance()->UnregisterNodeAnimationData(this);
            }
            SetName(strName);
            if (!strName.empty())
            {
                CNodeAnimationManager::GetInstance()->RegisterNodeAnimationData(this);
            }
            bRet = true;
        }
#ifdef EDITOR_MODE
        if (pVariableAddr == &m_uTotalTimeMS)
        {
            DeserializeVariable(m_uTotalTimeMS, pNewValueToBeSet, this);
            uint32_t uOldFrameCount = m_uFrameCount;
            m_uFrameCount = (uint32_t)(60 * (float)m_uTotalTimeMS * 0.001f);  // HACK: assume FPS is 60
            UPDATE_PROXY_PROPERTY_BY_NAME(this, m_uFrameCount, "m_uFrameCount");
            if (uOldFrameCount != 0)
            {
                float fRatio = (float)m_uFrameCount / uOldFrameCount;
                for (uint32_t i = 0; i < m_elements.size(); i++)
                {
                    std::map<uint32_t, CVec3> keyFrame = m_elements[i]->GetKeyFrames();
                    // Update the proxy.
                    CNodeAnimationElement* pElement = m_elements[i];
                    CComponentProxy* pProxy = pElement->GetProxyComponent();
                    CMapPropertyDescription* pMapProperty = dynamic_cast<CMapPropertyDescription*>(pProxy->GetProperty(_T("m_keyFrames")));
                    pMapProperty->RemoveAllChild();
                    for (auto iter = keyFrame.begin(); iter != keyFrame.end(); ++iter)
                    {
                        CPropertyDescriptionBase* pNewProperty = pMapProperty->InsertChild(NULL);
                        CPropertyDescriptionBase* pKeyProperty = pNewProperty->GetChildren()[0];
                        BEATS_ASSERT(pKeyProperty != NULL);
                        uint32_t uNewKey = (uint32_t)(iter->first * fRatio);
                        pKeyProperty->CopyValue((void*)&uNewKey, pKeyProperty->GetValue(eVT_CurrentValue));
                        CVec3PropertyDescription* pValueProperty = dynamic_cast<CVec3PropertyDescription*>(pNewProperty->GetChildren()[1]);
                        BEATS_ASSERT(pValueProperty != NULL);
                        CSerializer serializer;
                        serializer << iter->second.X() << iter->second.Y() << iter->second.Z();
                        pValueProperty->Deserialize(serializer);
                    }
                }
            }
            bRet = true;
        }
#endif
    }
    return bRet;
}
#endif

void CNodeAnimationData::AddElements(CNodeAnimationElement* pElement)
{
    BEATS_ASSERT(pElement != NULL);
    m_elements.push_back(pElement);
}

uint32_t CNodeAnimationData::GetFrameCount() const
{
    uint32_t uCount = 0;
    for (auto iter : m_elements)
    {
        uint32_t uMaxFrameIndex = iter->GetKeyFrames().rbegin()->first;
        BEATS_ASSERT(uMaxFrameIndex != 0xFFFFFFFF);
        if (uMaxFrameIndex > uCount)
        {
            uCount = uMaxFrameIndex + 1;
        }
    }
    return uCount;
}

void CNodeAnimationData::SetFrameCount(uint32_t uFrameCount)
{
    m_uFrameCount = uFrameCount;
    UPDATE_PROXY_PROPERTY_BY_NAME(this, m_uFrameCount, "m_uFrameCount");
    m_uTotalTimeMS = (uint32_t)(m_uFrameCount / 60.0f * 1000);
    UPDATE_PROXY_PROPERTY_BY_NAME(this, m_uTotalTimeMS, "m_uTotalTimeMS");
}

void CNodeAnimationData::Apply(CNode* pNode, uint32_t uCurrFrame)
{
    BEATS_ASSERT(uCurrFrame < GetFrameCount());
    for (uint32_t i = 0; i < m_elements.size(); ++i)
    {
        m_elements[i]->Apply(pNode, uCurrFrame);
    }
}

const std::vector<CNodeAnimationElement*>& CNodeAnimationData::GetElements() const
{
    return m_elements;
}

void CNodeAnimationData::SetName(const TString& strName)
{
    m_strName = strName;
}

const TString& CNodeAnimationData::GetName() const
{
    return m_strName;
}

void CNodeAnimationData::SetTempFlag(bool bFlag)
{
    BEATS_ASSERT(bFlag == false || (!IsLoaded() && GetName().empty()));
    m_bTempDataFlag = bFlag;
}

bool CNodeAnimationData::GetTempFlag() const
{
    return m_bTempDataFlag;
}

CNodeAnimationData* CNodeAnimationData::CreateSingleData(ENodeAnimationElementType type, const CVec3& startValue, const CVec3& endValue, float fTimeInSecond, uint32_t startPos)
{
    CNodeAnimationData* pData = new CNodeAnimationData;
    CNodeAnimationElement* pElement = new CNodeAnimationElement;
    pElement->SetType(type);
    pElement->AddKeyFrame(startPos, startValue);
    pElement->AddKeyFrame((uint32_t)(startPos + fTimeInSecond * 60), endValue);
    pData->AddElements(pElement);
    pData->SetTempFlag(true);
    return pData;
}

CNodeAnimationElement* CNodeAnimationData::CreateElement(ENodeAnimationElementType type, const CVec3& startValue, const CVec3& endValue, float fTimeInSecond, uint32_t startPos)
{
    CNodeAnimationElement* pElement = new CNodeAnimationElement;
    pElement->SetType(type);
    pElement->AddKeyFrame(startPos, startValue);
    pElement->AddKeyFrame((uint32_t)(startPos + fTimeInSecond * 60), endValue);
    return pElement;
}

CNodeAnimationElement* CNodeAnimationData::GetAnimationElementByType(ENodeAnimationElementType eElementType)
{
    CNodeAnimationElement* pElement = NULL;
    for (auto iter : m_elements)
    {
        if (iter->GetType() == eElementType)
        {
            pElement = iter;
            break;
        }
    }
    return pElement;
}