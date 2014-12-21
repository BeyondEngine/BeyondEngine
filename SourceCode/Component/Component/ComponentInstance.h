#ifndef BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTINSTANCE_H__INCLUDE
#define BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTINSTANCE_H__INCLUDE

#include "ComponentBase.h"
class CComponentProxy;

class  CComponentInstance : public CComponentBase
{
    DECLARE_REFLECT_GUID(CComponentInstance, 0x4314AEF, CComponentBase)
public:
    CComponentInstance();
    virtual ~CComponentInstance();
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual bool Load() override;
    virtual bool Unload() override;

    void SetDataPos(uint32_t uDataPos);
    uint32_t GetDataPos() const;

    void SetDataSize(uint32_t uDataSize);
    uint32_t GetDataSize() const;

    void SetProxyComponent(CComponentProxy* pProxy);
    class CComponentProxy* GetProxyComponent() const;

    virtual void SetSyncProxyComponent(CComponentProxy* pProxy);

    virtual CComponentInstance* CloneInstance() const;

    void SetReflectOwner(CComponentInstance* pReflectOwner);
    CComponentInstance* GetReflectOwner() const;
    const std::set<CComponentInstance*>& GetReflectComponents() const;
    void ClearReflectComponents();
    void RegisterReflectComponent(CComponentInstance* pComponent);
    void UnregisterReflectComponent(CComponentInstance* pComponent);
#ifdef EDITOR_MODE
    // Editor mode only!
    // The return value means if user has sync the variable manually, or system will do this assign.
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet);
    virtual bool OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent);
    virtual bool OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent);
    virtual void OnSave();
    virtual bool OnPropertyImport(const TString& strPropertyName, const TString& strFile);
    virtual bool OnExport();
#endif
#ifdef _DEBUG
    bool m_bReflectPropertyCheck = false;
    bool m_bReflectPropertyCritical = false;
#endif

private:
    uint32_t m_uDataPos;
    uint32_t m_uDataSize;
    CComponentInstance* m_pReflectComponentOwner; // TODO: this is only for avoid delete reflect components manually. maybe it is not necessary.
    std::set<CComponentInstance*> m_reflectComponents;
    CComponentProxy* m_pProxyComponent;
    // This member only for: when this instance is deleted, it must tell the sync proxy to unregister itself.
    CComponentProxy* m_pSyncProxyComponent;
};

#endif