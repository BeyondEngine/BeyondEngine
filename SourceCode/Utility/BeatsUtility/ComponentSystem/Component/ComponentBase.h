#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescription.h"

#define DECLARE_REFLECT_GUID(className, guid, parentClassName)\
    DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
    public:\
    virtual CComponentBase* Clone(bool /*bCloneFromTemplate*/, CSerializer* pSerializer, size_t id, bool bCallInitFunc = true){BEATS_ASSERT(typeid(className) == typeid(*this), _T("Define wrong class type: define %s"), _T(#className));CComponentBase* pNewInstance = new className; pNewInstance->SetId(id); if (pSerializer != NULL){pNewInstance->ReflectData(*pSerializer);if(bCallInitFunc){pNewInstance->Initialize();}} return pNewInstance;}\
    private:

#define DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
        public:\
        static const size_t REFLECT_GUID = guid;\
        static const size_t PARENT_REFLECT_GUID = parentClassName::REFLECT_GUID;\
        virtual size_t GetGuid() const {return REFLECT_GUID;}\
        virtual size_t GetParentGuid() const {return PARENT_REFLECT_GUID;}\
        virtual const TCHAR* GetClassStr() const {return _T(#className);}\
        private:\
        typedef parentClassName super;

class CSerializer;
class CDependencyDescription;
class CComponentBase
{
public:
    CComponentBase();
    virtual ~CComponentBase();
    DECLARE_REFLECT_GUID(CComponentBase, 0xFBA14097, CComponentBase)

public:
    size_t GetId() const;
    void SetId(size_t id);

    virtual bool Load(); // this method do some load stuff, which can be done in other thread.
    virtual bool Unload();
    bool IsLoaded() const;
    void SetLoadFlag(bool bFlag);

    virtual void Initialize();// this method can only be called in main thread, including render operation.
    virtual void Uninitialize();
    bool IsInitialized();
    void SetInitializeFlag(bool bFlag);

    virtual void ReflectData(CSerializer& serializer);
    // Editor mode only!
    // The return value means if user has sync the variable manually, or system will do this assign.
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet);
    virtual bool OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent);
    virtual bool OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent);
private:
    bool m_bLoaded;
    bool m_bInitialize;
    size_t m_id;
};
#endif