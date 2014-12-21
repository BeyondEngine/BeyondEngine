#include "stdafx.h"
#include "EnginePropertyGrid.h"
#include "EngineProperGridManager.h"
BEGIN_EVENT_TABLE(CEnginePropertyGrid, wxPropertyGrid)
    EVT_SCROLLWIN(CEnginePropertyGrid::OnScrollEvent)
END_EVENT_TABLE()

CEnginePropertyGrid::CEnginePropertyGrid()
    : m_pManager(NULL)
{
    
}

CEnginePropertyGrid::~CEnginePropertyGrid()
{
} 

void CEnginePropertyGrid::FreeEditorCtrl()
{
    wxPropertyGrid::FreeEditors();
}

void CEnginePropertyGrid::SetManager(CEnginePropertyGridManager* pManager)
{
    m_pManager = pManager;
}

void CEnginePropertyGrid::OnScrollEvent(wxScrollWinEvent &event)
{
    if (m_pManager)
    {
        m_pManager->OnScrollChanged(event.GetPosition());
    }
}

CEnginePropertyGridManager* CEnginePropertyGrid::GetManager() const
{
    return m_pManager;
}
