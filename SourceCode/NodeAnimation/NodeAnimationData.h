#ifndef BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONDATA_H__INCLUDE
#define BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONDATA_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "NodeAnimationElement.h"

class CNode;
class CNodeAnimationData : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CNodeAnimationData, 0x01B3472D, CComponentInstance)

public:
    CNodeAnimationData();
    virtual ~CNodeAnimationData();

    virtual bool Load() override;
    virtual bool Unload() override;
    virtual void ReflectData(CSerializer& serializer) override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet) override;
#endif
    CNodeAnimationElement* GetAnimationElementByType(ENodeAnimationElementType eElementType);
    void AddElements(CNodeAnimationElement* pElement);
    uint32_t GetFrameCount() const;
    void SetFrameCount(uint32_t uFrameCount);
    void Apply(CNode* pNode, uint32_t uCurrFrame);
    const std::vector<CNodeAnimationElement*>& GetElements() const;
    void SetName(const TString& strName);
    const TString& GetName() const;
    void SetTempFlag(bool bFlag);
    bool GetTempFlag() const;

    static CNodeAnimationData* CreateSingleData(ENodeAnimationElementType type, const CVec3& startValue, const CVec3& endValue, float fTimeInSecond, uint32_t startPos = 0);
    static CNodeAnimationElement* CreateElement(ENodeAnimationElementType type, const CVec3& startValue, const CVec3& endValue, float fTimeInSecond, uint32_t startPos = 0);

private:
    bool m_bTempDataFlag = false;
    uint32_t m_uFrameCount;
    uint32_t m_uTotalTimeMS;
    TString m_strName;
    std::vector<CNodeAnimationElement*> m_elements;
};

#endif
