#include "stdafx.h"
#include "SceneViewAgent.h"
#include "EditorMainFrame.h"


CSceneViewAgent* CSceneViewAgent::m_pInstance = nullptr;
CSceneViewAgent::CSceneViewAgent()
{

}

CSceneViewAgent::~CSceneViewAgent()
{

}

void CSceneViewAgent::ProcessMouseEvent( wxMouseEvent& event )
{
    event.Skip();
}

void CSceneViewAgent::InView()
{
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pResourcePanel).Show();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Hide();
    m_pMainFrame->m_Manager.Update();
}

void CSceneViewAgent::OutView()
{
    if(m_pMainFrame->m_pSelectedComponentProxy)
    {
        CNode3D* pNode = dynamic_cast<CNode3D*>(m_pMainFrame->m_pSelectedComponentProxy->GetHostComponent());
        if(pNode)
        {
            pNode->SetRenderWorldTM(false);
        }
    }
}

void CSceneViewAgent::SelectComponent(CComponentProxy* pComponentInstance)
{
    if (m_pMainFrame->m_pSelectedComponentProxy)
    {
        CNode3D* pNode3D = dynamic_cast<CNode3D*>(m_pMainFrame->m_pSelectedComponentProxy->GetHostComponent());
        if (pNode3D != NULL)
        {
            pNode3D->SetRenderWorldTM(false);
        }
        const std::vector<CComponentInstance*>& syncComponents = m_pMainFrame->m_pSelectedComponentProxy->GetSyncComponents();
        for (size_t i = 0; i < syncComponents.size(); ++i)
        {
            CNode3D* pSyncNode3D = dynamic_cast<CNode3D*>(syncComponents[i]);
            if (pSyncNode3D != NULL)
            {
                pSyncNode3D->SetRenderWorldTM(false);
            }
        }
    }
    if (pComponentInstance != NULL)
    {
        CNode3D* pNode3D = dynamic_cast<CNode3D*>(pComponentInstance->GetHostComponent());
        if (pNode3D != NULL)
        {
            pNode3D->SetRenderWorldTM(true);
        }
        const std::vector<CComponentInstance*>& syncComponents = pComponentInstance->GetSyncComponents();
        for (size_t i = 0; i < syncComponents.size(); ++i)
        {
            CNode3D* pSyncNode3D = dynamic_cast<CNode3D*>(syncComponents[i]);
            if (pSyncNode3D != NULL)
            {
                pSyncNode3D->SetRenderWorldTM(true);
            }
        }
    }
}