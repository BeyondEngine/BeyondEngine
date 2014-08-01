#ifndef BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONDATA_H__INCLUDE
#define BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONDATA_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

class CNodeAnimationElement;
class CNode;
class CNodeAnimationData : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CNodeAnimationData, 0x01B3472D, CComponentInstance)

public:
    CNodeAnimationData();
    virtual ~CNodeAnimationData();

    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet) override;

    void AddElements(CNodeAnimationElement* pElement);
    size_t GetFrameCount() const;
    void SetFrameCount(size_t uFrameCount);
    void Apply(CNode* pNode, size_t uCurrFrame);
    const std::vector<CNodeAnimationElement*>& GetElements() const;
    void SetName(const TString& strName);
    const TString& GetName() const;

private:
    size_t m_uFrameCount;
    TString m_strName;
    std::vector<CNodeAnimationElement*> m_elements;
};

#endif
