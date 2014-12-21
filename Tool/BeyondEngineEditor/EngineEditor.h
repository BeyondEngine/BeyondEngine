#ifndef BEYOND_ENGINE_EDITOR_ENGINEEDITOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_ENGINEEDITOR_H__INCLUDE

class wxGLContext;
class wxGLCanvas;
class wxPGEditor;
class CEditorMainFrame;
class CGradientDialog;
class CApplication;
class CWaitingForProcessDialog;
class CTexturePreviewDialog;
class wxPGEditorBase;

class CEngineEditor : public wxApp
{
public:
    CEngineEditor();
    virtual ~CEngineEditor();

    virtual bool OnInit();
    int OnExitImpl();

    wxPGEditor* GetPtrEditor();
    wxPGEditor* GetTextureEditor();
    wxPGEditor* GetGradientColorEditor();
    wxPGEditor* GetGridEditor();
    wxPGEditor* GetSoundFileEditor();
    wxPGEditor* GetRandomValueEditor();
    wxPGEditor* GetMenuEditor();
    CEditorMainFrame* GetMainFrame() const;
    wxGLContext* CreateGLContext(wxGLCanvas* pCanvas);
    wxGLContext* GetGLContext() const;
    const TString& GetWorkingPath();
    CGradientDialog* GetGradientDialog();
    CTexturePreviewDialog* GetTexturePreviewDialog();
    CWaitingForProcessDialog* GetWaitingDialog();
    void RegisterPGEditor(wxPGEditorBase* pPGEditor);
    void UnRegisterPGEditor(wxPGEditorBase* pPGEditor);
    const std::set<const wxPGEditor*>& GetEditorRegisterSet();

private:
    wxGLContext*        m_glRC;
    CEditorMainFrame*   m_pMainFrame;
    wxPGEditor*         m_pPtrButtonEditor;
    wxPGEditor*         m_pTextureEditor;
    wxPGEditor*         m_pGradientColorEditor;
    wxPGEditor*         m_pGridEditor;
    wxPGEditor*         m_pSoundFileEditor;
    wxPGEditor*         m_pRandomValueEditor;
    wxPGEditor*         m_pMenuEditor = nullptr;
    CGradientDialog*    m_pGradientDialog;
    CTexturePreviewDialog*    m_pTexturePreviewDialog;
    CApplication*       m_pApplication;
    CWaitingForProcessDialog* m_pWaitingDialog;
    TString             m_strWorkingPath;
    std::set<const wxPGEditor*> m_editorRegisterSet;
};


#endif