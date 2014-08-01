#ifndef BEYONDENGINEEDITOR_MAINAPP_H__INCLUDE
#define BEYONDENGINEEDITOR_MAINAPP_H__INCLUDE
class wxPGEditor;
class CEditorMainFrame;
class CEngineEditor : public wxApp
{
public:
    virtual bool OnInit();
    void CreatePtrEditor();
    wxPGEditor* GetPtrEditor() const;
    CEditorMainFrame* GetMainFrame() const;

private:
    CEditorMainFrame* m_pMainFrame;
    wxPGEditor* m_pPtrButtonEditor;
};


#endif