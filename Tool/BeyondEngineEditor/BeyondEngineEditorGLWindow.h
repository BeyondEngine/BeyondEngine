#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_BEYONDENGINEEDITORGLWINDOW_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_BEYONDENGINEEDITORGLWINDOW_H__INCLUDE

#include "wx/glcanvas.h"

class CCamera;
class CScene;
class wxGLRenderWindow;
class CGridRenderObject;
class CNode3D;
class CEditorMainFrame;
class CBeyondEngineEditorGLWindow : public wxGLCanvas
{
public:
    CBeyondEngineEditorGLWindow(wxWindow *parent,
                    wxGLContext* pShareContext = NULL,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxString& name = wxT("BeyondEngineEditorGLWindow"));

    virtual ~CBeyondEngineEditorGLWindow();
    wxGLContext* GetGLContext() const;
    void SetGLContext(wxGLContext* pContext);
    bool SafeGetKeyStage(int nVirtKey);
    wxGLRenderWindow *GetRenderWindow() const;

    virtual void Update() = 0;
    virtual void SetContextToCurrent();
    virtual CCamera* GetCamera() = 0;

protected:
    void OnCaptureLost(wxMouseCaptureLostEvent& event);
    void OnSetCursor(wxSetCursorEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnMouse(wxMouseEvent& event);
    void ShowCursor();
    void HideCursor();

protected:
    CEditorMainFrame*   m_pMainFrame;
    wxGLRenderWindow*   m_pRenderWindow;

private:
    float               m_fSpeedUpScale;
    wxPoint             m_startPosition;
    static const int    m_attribList[5];

    wxDECLARE_NO_COPY_CLASS(CBeyondEngineEditorGLWindow);
    DECLARE_EVENT_TABLE()
};
#endif