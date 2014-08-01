#ifndef BEYOND_ENGINE_AI_AISCHEME_H__INCLUDE
#define BEYOND_ENGINE_AI_AISCHEME_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

class CAIState;
class CAIScheme : public CComponentInstance
{
    DECLARE_REFLECT_GUID( CAIScheme, 0x9100B71A, CComponentInstance )

public:
    CAIScheme();
    virtual ~CAIScheme();

    virtual void Initialize() override;
    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet) override;
    const TString& GetName() const;
    CAIState* GetInitState() const;
    bool HasState(CAIState* pState) const;

private:
    TString m_strName;
    CAIState* m_pInitState;
    std::vector<CAIState*> m_states;
};

#endif