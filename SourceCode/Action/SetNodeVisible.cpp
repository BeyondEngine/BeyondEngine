#include "stdafx.h"
#include "SetNodeVisible.h"
#include "Scene/Node.h"

CSetNodeVisible::CSetNodeVisible()
    : m_bVisible(true)
{

}

CSetNodeVisible::~CSetNodeVisible()
{

}

void CSetNodeVisible::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bVisible, true, 0xFFFFFFFF, _T("是否可见"), NULL, NULL, NULL);
}

bool CSetNodeVisible::ExecuteImp(SActionContext* pContext)
{
    for (size_t i = 0; i < pContext->m_operator.size(); ++i)
    {
        pContext->m_operator[i]->SetVisible(m_bVisible);
    }
    return true;
}
