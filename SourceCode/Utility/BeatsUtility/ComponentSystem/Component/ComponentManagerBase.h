#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGERBASE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGERBASE_H__INCLUDE

class CComponentBase;
class CIdManager;
class CSerializer;
class CDependencyDescription;
class CComponentProject;

typedef void (*TAddDependencyFunc)(void* pContainer, void* pDependency);
void DefaultAddDependencyFunc(void* pContainer, void* pDependency);

struct SDependencyResolver
{
    SDependencyResolver()
        : uIndex(0)
        , uGuid(0)
        , uInstanceId(0)
        , bIsList(false)
        , pVariableAddress(NULL)
        , pDescription(NULL)
        , pAddFunc(NULL)
    {

    }
    ~SDependencyResolver()
    {

    }
    size_t uIndex;
    size_t uGuid;
    size_t uInstanceId;
    bool bIsList;
    void* pVariableAddress;
    CDependencyDescription* pDescription;
    TAddDependencyFunc pAddFunc;
};

class CComponentManagerBase
{
public:
    CComponentManagerBase();
    virtual ~CComponentManagerBase();

    //Call it in derived class destructor instead of ~CComponentManagerBase(), because we still need the singleton(derived class)
    //In the Release, If We do it in ~CComponentManagerBase(), we can no longer visit the singleton.
    void Release();
    void DeleteAllInstance();
    void InitializeAllInstance();
    void InitializeAllTemplate();
    void UninitializeAllInstance();
    void UninitializeAllTemplate();

    bool RegisterTemplate(CComponentBase* pComponent);
    bool UnregisterTemplate(CComponentBase* pComponent);
    bool RegisterInstance(CComponentBase* pComponent);
    bool UnregisterInstance(CComponentBase* pComponent);

    /* 
    1. param bCloneFromTemplate means, if you wanna create a brand new component or make a copy of template component. When we create component from data, we usually create
    a brand new data, if we create components in editor, we always copy the template.
    2. param bManualManage means, if you wanna manage this new create component by yourself or component manager, if you manage it yourself, you have to remember to recycle
    the memory, and it can't be found by the manager as if component manager never knows your component.
    3. param specifiedInstanceId means, you can specify id by yourself instead of manager, if you request an id which is taken by manager, there will be an assert.
    */
    CComponentBase* GetComponentTemplate(size_t guid) const;
    CComponentBase* CreateComponent(size_t guid, bool bCloneFromTemplate, bool bManualManage = false, size_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL, bool bCallInitFunc = true);
    CComponentBase* CreateComponentByRef(CComponentBase* pComponentRef, bool bCloneValue, bool bManualManage = false, size_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL, bool bCallInitFunc = true);

    const std::map<size_t, CComponentBase*>* GetComponentTemplateMap();
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* GetComponentInstanceMap();
    CComponentBase* GetComponentInstance(size_t uId, size_t uGuid = 0xFFFFFFFF);

    CIdManager* GetIdManager() const;
    CComponentProject* GetProject() const;

    void SetForbidDependencyResolve(bool bEnable);
    bool GetForbidDependencyResolve() const;
    void AddDependencyResolver( CDependencyDescription* pDescription, size_t uIndex, size_t uGuid, size_t uInstanceId, void* pVariableAddress, bool bIsList, TAddDependencyFunc pFunc = NULL);
    virtual void ResolveDependency() = 0;

protected:
    bool m_bForbidDependencyResolve;
    CIdManager* m_pIdManager;
    CComponentProject* m_pProject;
    std::map<size_t, CComponentBase*>* m_pComponentTemplateMap;
    std::map<size_t, std::map<size_t, CComponentBase*>*>* m_pComponentInstanceMap;
    std::vector<SDependencyResolver*>* m_pDependencyResolver;
    std::vector<CComponentBase*>* m_pUninitializedComponents;
};

#endif