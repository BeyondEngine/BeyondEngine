#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_SPLINEGLWINDOW_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_SPLINEGLWINDOW_H__INCLUDE

#include "BeyondEngineEditorGLWindow.h"

struct SSpline;
class CSplineGLWindow : public CBeyondEngineEditorGLWindow
{
    typedef CBeyondEngineEditorGLWindow super;
public:
    CSplineGLWindow(wxWindow *parent,
        wxGLContext* pShareContext = NULL,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("BeyondEngineEditorGLWindow"));
    virtual ~CSplineGLWindow();

    virtual CCamera* GetCamera() override;
    virtual void Update() override;

protected:
    void OnMouse(wxMouseEvent& event);

private:
    SharePtr<SSpline>   m_spline;
    int                 m_selectedIndex;
    wxGLContext*        m_glRC;
    DECLARE_EVENT_TABLE()
};
#endif