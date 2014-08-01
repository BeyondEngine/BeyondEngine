#include "stdafx.h"
#include "CommandManager.h"
#include "CommandBase.h"

CEditorCommandManager *CEditorCommandManager::m_pInstance = NULL;
CEditorCommandManager::CEditorCommandManager()
{
}

CEditorCommandManager::~CEditorCommandManager()
{
}

void CEditorCommandManager::DoCommand(CEditorCommandBase* pCommand)
{
    pCommand->Do();
    m_redoVector.clear();
    m_undoVector.push_back(pCommand);
}

void CEditorCommandManager::UnDo()
{
    if (m_undoVector.size() > 0)
    {
        CEditorCommandBase* pCommand = m_undoVector.back();
        pCommand->UnDo();
        m_undoVector.pop_back();
        m_redoVector.push_back(pCommand);
    }
}

void CEditorCommandManager::ReDo()
{
    if (m_redoVector.size() > 0)
    {
        CEditorCommandBase* pCommand = m_redoVector.back();
        pCommand->Do();
        m_redoVector.pop_back();
        m_undoVector.push_back(pCommand);
    }
}
