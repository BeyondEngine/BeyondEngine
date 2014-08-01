#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE

#include "ComponentBase.h"


class CComponentInstance : public CComponentBase
{
    DECLARE_REFLECT_GUID(CComponentInstance, 0x04314AEF, CComponentBase)
public:
    CComponentInstance();
    virtual ~CComponentInstance();
    virtual void Uninitialize() override;

    void SetDataPos(size_t uDataPos);
    size_t GetDataPos() const;

    void SetDataSize(size_t uDataSize);
    size_t GetDataSize() const;

public:
    void SetProxyComponent(CComponentProxy* pProxy);
    CComponentProxy* GetProxyComponent() const;

    void SetSyncProxyComponent(CComponentProxy* pProxy);
    CComponentProxy* GetSyncProxyComponent() const;

    void Serialize(CSerializer& serializer);
    CComponentBase* CloneInstance();

private:
    size_t m_uDataPos;
    size_t m_uDataSize;
    CComponentProxy* m_pProxyComponent;
    CComponentProxy* m_pSyncProxyComponent;
};

#endif