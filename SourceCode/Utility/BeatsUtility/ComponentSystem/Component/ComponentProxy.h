#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROXY_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROXY_H__INCLUDE

#include "ComponentBase.h"
#include "Utility/BeatsUtility/ComponentSystem/Property/PropertyPublic.h"
class CComponentGraphic;
class TiXmlElement;
class CDependencyDescription;
class CDependencyDescriptionLine;
class CPropertyDescriptionBase;
class CComponentGraphic;
class CComponentInstance;

class CComponentProxy : public CComponentBase
{
    typedef CComponentBase super;
public:
    CComponentProxy();//Only ComponentReference should use this constructor.
    CComponentProxy(CComponentGraphic* pGraphics);
    CComponentProxy(CComponentGraphic* pGraphics, size_t uGuid, size_t uParentGuid, const TCHAR* pszClassName);
    virtual ~CComponentProxy();

    const TString& GetDisplayName() const;
    void SetDisplayName(const TCHAR* pDisplayName);
    const TString& GetCatalogName() const;
    void SetCatalogName(const TCHAR* pCatalogName);
    const TString& GetUserDefineDisplayName() const;
    void SetUserDefineDisplayName(const TCHAR* pszUserDefineDisplayName);
    CComponentGraphic* GetGraphics();
    void SetGraphics(CComponentGraphic* pGraphics);

    void SetHostComponent(CComponentInstance* pComponent);
    CComponentInstance* GetHostComponent() const;
    void UpdateHostComponent();

    void SetTemplateFlag(bool bFlag);
    bool GetTemplateFlag() const;

    void AddProperty(CPropertyDescriptionBase* pProperty);
    void ClearProperty();
    const std::vector<CPropertyDescriptionBase*>* GetPropertyPool() const;
    CPropertyDescriptionBase* GetPropertyDescription(const TCHAR* pszVariableName) const;

    CDependencyDescription* GetDependency(size_t uIndex);
    const std::vector<CDependencyDescription*>* GetDependencies();
    void AddDependencyDescription(CDependencyDescription* pDependencyDesc);
    void AddBeConnectedDependencyDescriptionLine(CDependencyDescriptionLine* pDependencyDescLine);
    void RemoveBeConnectedDependencyDescriptionLine(CDependencyDescriptionLine* pDependencyDescLine);
    const std::vector<CDependencyDescriptionLine*>* GetBeConnectedDependencyLines();

    //Simulate the real component.
    static const size_t REFLECT_GUID = 1;
    static const size_t PARENT_REFLECT_GUID = super::REFLECT_GUID;
    virtual size_t GetGuid() const;
    virtual size_t GetParentGuid() const override;
    virtual const TCHAR* GetClassStr() const;
    void Save();
    virtual size_t GetProxyId();
    virtual CComponentBase* Clone(bool bCloneValue, CSerializer* pSerializer, size_t id, bool bCallInitFunc = true) override;

    ///*
    //    We have two ways for interaction:
    //    1. Serialize && Deserialize, this is for binarize all data which comes from editor, for program running.
    //    2. SaveToXML && LoadFromXML, this is for save and load data for editing.
    //*/

    virtual void SaveToXML(TiXmlElement* pNode, bool bSaveOnlyNoneNativePart = false);
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual void Serialize(CSerializer& serializer, EValueType eValueType);
    virtual void Deserialize(CSerializer& serializer);

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    void AddSyncComponent(CComponentInstance* pInstance);
    void RemoveSyncComponent(CComponentInstance* pInstance);
    const std::vector<CComponentInstance*>& GetSyncComponents() const;

protected:
    std::vector<CPropertyDescriptionBase*>* m_pProperties;
    std::vector<CDependencyDescription*>* m_pDependenciesDescription;
    std::vector<CDependencyDescriptionLine*>* m_pBeConnectedDependencyLines;
    CComponentInstance* m_pHostComponent;
    std::vector<CComponentInstance*> m_syncComponents; // these instance will be synced as m_pHostComponent, usually for prototype stuff.

private:
    bool m_bIsTemplate;
    size_t m_uGuid;
    size_t m_uParentGuid;
    CComponentGraphic* m_pGraphics;
    std::vector<char>* m_pSerializeOrder; // this variable save the order of property/dependency data comes in when deserialize. we have to keep the order when serialize.
    TString m_strClassName;
    TString m_strDisplayName;
    TString m_strUserDefineDisplayName;
    TString m_strCatalogName;
};

#endif