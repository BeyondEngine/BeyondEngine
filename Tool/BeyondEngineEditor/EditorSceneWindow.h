#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_EDITORSCENEWINDOW_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_EDITORSCENEWINDOW_H__INCLUDE

#include "BeyondEngineEditorGLWindow.h"

class CViewAgentBase;
class CEditorSceneWindow : public CBeyondEngineEditorGLWindow
{
    typedef CBeyondEngineEditorGLWindow super;
public:
    CEditorSceneWindow(wxWindow *parent,
        wxGLContext* pShareContext = NULL,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("BeyondEngineEditorGLWindow"));
    virtual ~CEditorSceneWindow();

    CGridRenderObject* GetGrid() const;

    virtual void Update() override;
    virtual CCamera* GetCamera() override;
    virtual void SetContextToCurrent() override;

    void SetViewAgent(CViewAgentBase* pWxView);

private:
    void OnSize(wxSizeEvent& event);

private:
    CGridRenderObject*  m_pNodeGrid;
    CCamera* m_pDefault3DCamera;
    CCamera* m_pDefault2DCamera;
    CViewAgentBase* m_pWxView;

    wxDECLARE_NO_COPY_CLASS(CEditorSceneWindow);
    DECLARE_EVENT_TABLE()
};
#endif