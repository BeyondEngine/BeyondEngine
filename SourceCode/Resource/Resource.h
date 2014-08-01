#ifndef BEYOND_ENGINE_RESOURCE_RESOURCE_H__INCLUDE
#define BEYOND_ENGINE_RESOURCE_RESOURCE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"

#include <condition_variable>

#define DECLARE_RESOURCE_TYPE(type)\
    public:\
    virtual EResourceType GetType(){return type;}\
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
    
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
    virtual EResourceType GetType() = 0;

protected:
    SReflectFilePath m_strPath;
    CSerializer* m_pData;
};

#endif