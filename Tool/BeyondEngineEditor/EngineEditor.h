#ifndef BEYOND_ENGINE_EDITOR_ENGINEEDITOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_ENGINEEDITOR_H__INCLUDE

class wxGLContext;
class wxGLCanvas;
class wxPGEditor;
class CEditorMainFrame;
class CEngineEditor : public wxApp
{
public:
    CEngineEditor();
    virtual ~CEngineEditor();

    virtual bool OnInit();
    virtual int OnExit();

    wxPGEditor* GetPtrEditor();
    wxPGEditor* GetTextureEditor();
    wxPGEditor* GetGradientColorEditor();
    wxPGEditor* GetGridEditor();
    CEditorMainFrame* GetMainFrame() const;
    wxGLContext* CreateGLContext(wxGLCanvas* pCanvas);
    wxGLContext* GetGLContext() const;
    const TString& GetWorkingPath();

private:
    wxGLContext*        m_glRC;
    CEditorMainFrame*   m_pMainFrame;
    wxPGEditor*         m_pPtrButtonEditor;
    wxPGEditor*         m_pTextureEditor;
    wxPGEditor*         m_pGradientColorEditor;
    wxPGEditor*         m_pGridEditor;
    TString             m_strWorkingPath;
};


#endif