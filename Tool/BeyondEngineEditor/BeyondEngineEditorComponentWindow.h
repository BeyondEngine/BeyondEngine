#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_BEYONDENGINEEDITORCOMPONENTWINDOW_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_BEYONDENGINEEDITORCOMPONENTWINDOW_H__INCLUDE

#include "wx/glcanvas.h"
#include "BeyondEngineEditorGLWindow.h"

class CCamera;
class CShaderProgram;
class CComponentProxy;
class CDependencyDescription;
class CDependencyDescriptionLine;
class CEditorMainFrame;
class CBeyondEngineEditorGLWindow;
class CScene;
class CComponentRenderObject;
class CNode;

class CBeyondEngineEditorComponentWindow : public CBeyondEngineEditorGLWindow
{
public:
    CBeyondEngineEditorComponentWindow(wxWindow *parent,
        wxGLContext* pShareContext = NULL,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("BeyondEngineEditorGLWindow"));

    virtual ~CBeyondEngineEditorComponentWindow();

    void UpdateAllDependencyLine();
    void DeleteSelectedComponent();
    void DeleteSelectedDependencyLine();
    virtual void Update() override;
    void SetDraggingComponent(CComponentProxy* pDraggingComponent);
    CComponentProxy* GetDraggingComponent();
    CComponentProxy* GetConnectComponent();
    CDependencyDescription* GetDraggingDependency();
    void ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY);
    void ResetProjectionMode();
    virtual CCamera* GetCamera() override;

protected:
    void OnCaptureLost(wxMouseCaptureLostEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseMidScroll(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseRightUp(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnComponentMenuClicked(wxMenuEvent& event);
    void OnDependencyMenuClicked(wxMenuEvent& event);
    void OnKeyDown(wxKeyEvent& aEvent);

private:
    void ConvertWorldPosToGridPos(const kmVec2* pVectorPos, int* pOutGridX, int* pOutGridY);
    void ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY);
    CComponentProxy* HitTestForComponent(const wxPoint& pos, enum EComponentAeraRectType* pOutAreaType = NULL, void** pReturnData = NULL);

private:
    int             m_iWidth;
    int             m_iHeight;
    int             m_pOutX;
    int             m_pOutY;
    kmVec2          m_startDragPos;
    float           m_fCellSize;
    bool            m_bShowMenu;

    wxMenu*         m_pComponentMenu; 
    wxMenu*         m_pAddDependencyMenu;
    CShaderProgram* m_pLineProgram;
    CEditorMainFrame*       m_pMainFrame;
    CDependencyDescription* m_pAutoSetDependency;
    CComponentProxy*  m_pDraggingComponent;
    CComponentProxy*  m_pConnectComponent;
    CComponentProxy*  m_pCopyComponent;
    CComponentProxy*  m_pClickedComponent;
    CDependencyDescription* m_pDraggingDependency;
    CDependencyDescriptionLine* m_pSelectedDependencyLine;

    CCamera* m_pCamera;
    CComponentRenderObject *m_pNode;

    std::map<wxMenuItem*, size_t> m_menuItemAndGuidMap;
    wxDECLARE_NO_COPY_CLASS(CBeyondEngineEditorComponentWindow);
    DECLARE_EVENT_TABLE()
};
#endif