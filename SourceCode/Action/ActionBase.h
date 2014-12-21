#ifndef BEYOND_ENGINE_ACTION_ACTIONBASE_H__INCLUDE
#define BEYOND_ENGINE_ACTION_ACTIONBASE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
//#define ENABLE_ACTION_PRINT_INFO
#ifdef ENABLE_ACTION_PRINT_INFO
#define PRINT_ACTION_INFO(...) BEATS_PRINT(__VA_ARGS__)
#else
#define PRINT_ACTION_INFO(...)
#endif

class CNode;
struct SActionContext
{
    SActionContext()
        : m_bOperatorIncludeSelf(false)
        , m_pSerializer(NULL)
        , m_pUserData(NULL)
        , m_pRootNode(NULL)
        , m_fDeltaTime(0)
    {

    }
    ~SActionContext()
    {

    }
    bool m_bOperatorIncludeSelf;
    CSerializer* m_pSerializer;
    void* m_pUserData;
    float m_fDeltaTime;
    CNode* m_pRootNode;
    std::vector<CNode*> m_operator;
};

class CActionBase : public CComponentInstance
{
    DECLARE_REFLECT_GUID_ABSTRACT( CActionBase, 0x741BC05A, CComponentInstance )
public:
    CActionBase();
    virtual ~CActionBase();
    bool Execute(SActionContext* pContext);
    virtual void ReflectData(CSerializer& serializer) override;
    const TString& GetName() const;
    CActionBase* CloneWholeActionTree(bool bCallInit) const;
    CActionBase* GetNextAction() const;
private:
    virtual bool ExecuteImp(SActionContext* pContext) = 0;

protected:
    bool m_bOperateOnOwn;
    CActionBase* m_pNextAction;
    TString m_strName;
};
#endif