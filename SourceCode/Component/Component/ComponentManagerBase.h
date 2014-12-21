#ifndef BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTMANAGERBASE_H__INCLUDE
#define BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTMANAGERBASE_H__INCLUDE

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
    uint32_t uIndex;
    uint32_t uGuid;
    uint32_t uInstanceId;
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
    CComponentBase* GetComponentTemplate(uint32_t guid) const;
    CComponentBase* CreateComponent(uint32_t guid, bool bCloneFromTemplate, bool bManualManage = false, uint32_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL, bool bCallInitFunc = true);
    CComponentBase* CreateComponentByRef(CComponentBase* pComponentRef, bool bCloneValue, bool bManualManage = false, uint32_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL, bool bCallInitFunc = true);

    const std::map<uint32_t, CComponentBase*>* GetComponentTemplateMap();
    const std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>* GetComponentInstanceMap();
    CComponentBase* GetComponentInstance(uint32_t uId, uint32_t uGuid = 0xFFFFFFFF);
    const std::vector<uint32_t>& GetLoadedFiles() const;

    CIdManager* GetIdManager() const;
    CComponentProject* GetProject() const;

    virtual void LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pLoadComponents) = 0;
    virtual void UnloadFile(uint32_t uFileId, std::vector<CComponentBase*>* pUnloadComponents) = 0;
    virtual void CloseFile(uint32_t uFileId) = 0;
    void Reset(bool bDeleteTemplate = false);

    uint32_t GetCurrLoadFileId();
    void SetCurrLoadFileId(uint32_t uId);

    void CalcSwitchFile(uint32_t uFileId, std::vector<uint32_t>& loadFiles, std::vector<uint32_t>& unloadFiles, bool& bNewAddFile);
    void AddDependencyResolver( CDependencyDescription* pDescription, uint32_t uIndex, uint32_t uGuid, uint32_t uInstanceId, void* pVariableAddress, bool bIsList, TAddDependencyFunc pFunc = NULL);
    virtual void ResolveDependency() = 0;

private:
    void DeleteAllInstance();

protected:
    uint32_t m_uCurrLoadFileId;
    CIdManager* m_pIdManager;
    CComponentProject* m_pProject;
    std::map<uint32_t, CComponentBase*>* m_pComponentTemplateMap;
    std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>* m_pComponentInstanceMap;
    std::vector<SDependencyResolver*>* m_pDependencyResolver;
    std::vector<uint32_t> m_loadedFiles;
};

#endif