#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTREFERENCE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTREFERENCE_H__INCLUDE

#include "ComponentProxy.h"

class CComponentReference : public CComponentProxy
{
    typedef CComponentProxy super;
public:
    CComponentReference(size_t uProxyId, size_t uProxyGuid, CComponentGraphic* pGraphics);
    CComponentReference(CComponentProxy* pComponentHost);
    virtual ~CComponentReference();

    CComponentProxy* GetHostProxy();
    virtual size_t GetProxyId();
    virtual size_t GetGuid() const override;
    virtual size_t GetParentGuid() const override;
    virtual const TCHAR* GetClassStr() const override;

    virtual void SaveToXML(TiXmlElement* pNode, bool bSaveOnlyNoneNativePart = false) override;
    virtual void LoadFromXML(TiXmlElement* pNode) override;

    virtual void Initialize() override;
    virtual void Uninitialize() override;

private:
    size_t m_uReferenceId;
    size_t m_uReferenceGuid;
    CComponentProxy* m_pHostProxy;
};

#endif