#include "stdafx.h"
#include "EnginePropertyGrid.h"
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

void CEnginePropertyGrid::SetManager( wxPropertyGridManager* pManager )
{
    m_pManager = pManager;
}

wxPropertyGridManager* CEnginePropertyGrid::GetManager() const
{
    return m_pManager;
}
