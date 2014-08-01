#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_ANIMATIONGLWINDOW_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_ANIMATIONGLWINDOW_H__INCLUDE

#include "BeyondEngineEditorGLWindow.h"

class CGridRenderObject;
class CModel;
class CAnimationGLWindow : public CBeyondEngineEditorGLWindow
{
    typedef CBeyondEngineEditorGLWindow super;
public:
    CAnimationGLWindow(wxWindow *parent,
                        wxGLContext* pShareContext = NULL,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize, long style = 0,
                        const wxString& name = wxT("AnimationGLWindow"));
    virtual ~CAnimationGLWindow();

    void SetModel(CModel* pModel);
    CModel* GetModel();
    virtual CCamera* GetCamera() override;

protected:
    virtual void Update() override;

private:
    CModel* m_pModel;
    CCamera* m_pCamera;
    CGridRenderObject* m_pGrid;
};
#endif