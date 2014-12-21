#ifndef BEYOND_ENGINE_SCENE_NODE3D_H__INCLUDE
#define BEYOND_ENGINE_SCENE_NODE3D_H__INCLUDE
#include "Node.h"

class CNode3D : public CNode
{
    DECLARE_REFLECT_GUID( CNode3D, 0x1458a660, CNode )
public:
    CNode3D();
    virtual ~CNode3D();
    virtual void ReflectData(CSerializer& serializer) override;
    virtual CNode3D* GetUserDefinePos(const TString& strPointName, bool bWorldOrLocal, CVec3& outPos) const;
    virtual ENodeType GetType() const override;
protected:
    std::map<TString, CVec3> m_userDefinePos3D;
};

#endif//NODE3D_H_INCLUDE

