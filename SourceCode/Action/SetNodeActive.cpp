#include "stdafx.h"
#include "SetNodeActive.h"
#include "Scene/Node.h"

CSetNodeActive::CSetNodeActive()
: m_bActive(true)
{

}

CSetNodeActive::~CSetNodeActive()
{

}

void CSetNodeActive::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bActive, true, 0xFFFFFFFF, _T("ÊÇ·ñ¼¤»î"), NULL, NULL, NULL);
}

bool CSetNodeActive::ExecuteImp(SActionContext* pContext)
{
    for (size_t i = 0; i < pContext->m_operator.size(); ++i)
    {
        m_bActive ? pContext->m_operator[i]->Activate() : pContext->m_operator[i]->Deactivate();
    }
    return true;
}
