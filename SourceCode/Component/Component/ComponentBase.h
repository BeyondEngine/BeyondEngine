#ifndef BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTBASE_H__INCLUDE
#define BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTBASE_H__INCLUDE

#include "DependencyDescription.h"
#include "ComponentInstanceManager.h"

#define DECLARE_REFLECT_GUID(className, guid, parentClassName)\
    DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
    public:\
    virtual CComponentBase* Clone(bool /*bCloneFromTemplate*/, CSerializer* pSerializer, uint32_t id, bool bCallInitFunc = true) const override \
    {\
        bool bOriginClonePhase = CComponentInstanceManager::GetInstance()->IsInClonePhase(); \
        CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(true); \
        BEATS_ASSERT(typeid(className) == typeid(*this), _T("Define wrong class type: define %s"), _T(#className)); \
        CComponentBase* pNewInstance = BEATS_NEW(className, "Component", #className); \
        pNewInstance->SetId(id); \
        if (pSerializer != NULL)\
        {\
            pNewInstance->ReflectData(*pSerializer); \
            if (bCallInitFunc)\
            {\
                pNewInstance->Initialize(); \
            }\
        }\
        CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(bOriginClonePhase); \
        return pNewInstance; \
    }\
    private:

#define DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
        public:\
        static const uint32_t REFLECT_GUID = guid;\
        static const uint32_t PARENT_REFLECT_GUID = parentClassName::REFLECT_GUID;\
        virtual uint32_t GetGuid() const override {return REFLECT_GUID;}\
        virtual uint32_t GetParentGuid() const override { return PARENT_REFLECT_GUID; }\
        virtual const TCHAR* GetClassStr() const override { return _T(#className); }\
        private:\
        typedef parentClassName super;


class CSerializer;
class CDependencyDescription;
class CComponentBase
{
public:
    CComponentBase();
    virtual ~CComponentBase();
#define override
    DECLARE_REFLECT_GUID(CComponentBase, 0xFBA14097, CComponentBase)
#undef override

public:
    uint32_t GetId() const;
    void SetId(uint32_t id);

    virtual bool Load(); // This method do some load stuff, which can be done in other thread.
    virtual bool Unload();
    bool IsLoaded() const;
    void SetLoadFlag(bool bFlag);

    virtual void Initialize();// This method can only be called in main thread, including render operation.
    virtual void Uninitialize();
    bool IsInitialized() const;
    void SetInitializeFlag(bool bFlag);

    // Game mode only!
    virtual void ReflectData(CSerializer& serializer);

private:
    bool m_bLoaded;
    bool m_bInitialize;
    uint32_t m_id;
};
#endif