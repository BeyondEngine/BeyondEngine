#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_COMPONENTRENDEROBJECT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_COMPONENTRENDEROBJECT_H__INCLUDE

#include "Scene/Node3D.h"

class CCamera;
class CBeyondEngineEditorComponentWindow;
class CEditorMainFrame;
class CDependencyDescription;
class CDependencyDescriptionLine;
class CComponentProxy;

class CComponentRenderObject : public CNode3D
{
public:
    CComponentRenderObject();
    virtual ~CComponentRenderObject();

    virtual void DoRender() override;

    void SetComponentWindow(CBeyondEngineEditorComponentWindow *pComponentWindow);
    void SetMainFrame(CEditorMainFrame *pMainFrame);
    void SetCamera(CCamera *pCamera);
    void SetCellSize(float fCellSize);

private:
    void RenderGridLine();
    void RenderDraggingDependencyLine();
    void RenderComponents();
    void ConvertWorldPosToGridPos(const kmVec2* pVectorPos, int* pOutGridX, int* pOutGridY);
    void ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY);
    void ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY);

private:
    CCamera *m_pCamera;
    CEditorMainFrame *m_pMainFrame;
    CBeyondEngineEditorComponentWindow *m_pComponentWindow;

    float m_fCellSize;
};

#endif