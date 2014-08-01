#ifndef FCENGINEEDITOR_COMMANDMANAGER_H__INCLUDE
#define FCENGINEEDITOR_COMMANDMANAGER_H__INCLUDE

class CEditorCommandBase;
class CEditorCommandManager
{
    BEATS_DECLARE_SINGLETON(CEditorCommandManager);
    public:
    void DoCommand(CEditorCommandBase* pCommand);
    void UnDo();
    void ReDo();

private:
    std::vector<CEditorCommandBase*> m_undoVector;
    std::vector<CEditorCommandBase*> m_redoVector;
};



#endif