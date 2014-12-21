#ifndef BEYOND_ENGINE_RESOURCE_RESOURCE_H__INCLUDE
#define BEYOND_ENGINE_RESOURCE_RESOURCE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "Component/ComponentPublic.h"

#define DECLARE_RESOURCE_TYPE(type)\
    public:\
    virtual EResourceType GetType() const override { return type; }\
    static const EResourceType RESOURCE_TYPE = type;

class CResource : public CComponentInstance
{
    DECLARE_REFLECT_GUID_ABSTRACT(CResource, 0x480AD1CB, CComponentInstance)
public:
    CResource();
    virtual ~CResource();

    virtual void ReflectData(CSerializer& serializer) override;
    const TString& GetFilePath() const;
    void SetFilePath(const TString& str);
    
    virtual bool ShouldClean() const;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    virtual EResourceType GetType() const = 0;
#ifdef EDITOR_MODE
    virtual void Reload();
    virtual bool NeedReload() const;
    uint32_t m_uLastModifyTimeLow = 0;
    uint32_t m_uLastModifyTimeHigh = 0;
#endif
#ifdef DEVELOP_VERSION
    virtual TString GetDescription() const;
    bool m_bDelayInitialize = false;
    uint32_t m_uLoadTimeMS = 0;
    uint32_t m_uInitializeTimeMS = 0;
    uint32_t m_uFileSize = 0;
#endif
protected:
    SReflectFilePath m_strPath;
    CSerializer* m_pData;
};

#endif