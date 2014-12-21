#include "stdafx.h"
#include "Node3D.h"

CNode3D::CNode3D()
{
    m_defaultGroupID = LAYER_3D;
}

CNode3D::~CNode3D()
{

}

ENodeType CNode3D::GetType() const
{
    return eNT_Node3D;
}

void CNode3D::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_userDefinePos3D, true, 0xFFFFFFFF, _T("用户定义点"), NULL, NULL, NULL);
}

CNode3D* CNode3D::GetUserDefinePos(const TString& strPointName, bool bWorldOrLocal, CVec3& outPos) const
{
    auto iter = m_userDefinePos3D.find(strPointName);
    CNode3D* pRet = nullptr;
    if (iter != m_userDefinePos3D.end())
    {
        outPos = iter->second;
        if (bWorldOrLocal)
        {
            const CMat4& worldTM = GetWorldTM();
            outPos *= worldTM;
        }
        pRet = const_cast<CNode3D*>(this);
    }
    return pRet;
}

