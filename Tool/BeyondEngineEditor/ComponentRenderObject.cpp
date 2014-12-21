#include "stdafx.h"
#include "ComponentRenderObject.h"
#include "EditorMainFrame.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "PerformDetector/PerformDetector.h"
#include "Render/RenderManager.h"
#include "Render/Camera.h"
#include "Component/Component/ComponentInstance.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProxy.h"
#include "Component/Component/DependencyDescription.h"
#include "Component/Component/DependencyDescriptionLine.h"
#include "Component/Component/ComponentGraphic.h"
#include "Component/Component/ComponentProject.h"
#include "ComponentGraphics_GL.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderGroup.h"
#include "Render/RenderBatch.h"

static const float MAX_Z_VALUE = -100.f;
static const float MIN_CELL_SIZE = 5.f;
static const int MIN_SKEW_DEPENDENCY = 4;
static const int MIN_SKEW_DEPENDENCY_HIGHT = 3;

CComponentRenderObject::CComponentRenderObject()
    : m_pCamera(nullptr)
    , m_pMainFrame(nullptr)
    , m_pComponentWindow(nullptr)
    , m_fCellSize(0.f)
{
}

CComponentRenderObject::~CComponentRenderObject()
{
}

void CComponentRenderObject::SetComponentWindow(CBeyondEngineEditorComponentWindow *pComponentWindow)
{
    m_pComponentWindow = pComponentWindow;
}

void CComponentRenderObject::SetMainFrame(CEditorMainFrame *pMainFrame)
{
    m_pMainFrame = pMainFrame;
}

void CComponentRenderObject::SetCamera(CCamera *pCamera)
{
    m_pCamera = pCamera;
}

void CComponentRenderObject::SetCellSize(float fCellSize)
{
    m_fCellSize = fCellSize;
}

void CComponentRenderObject::DoRender()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_Grid)
    RenderGridLine();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_Grid)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_DraggingLine)
    RenderDraggingDependencyLine();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_DraggingLine)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_Components)
    RenderComponents();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_Components)
}

void CComponentRenderObject::RenderGridLine()
{
/*   _________________________
    |                         |
    |    ________________     |
    |    |               |    |
    |    |       c       |    |
    |    |_______________|    |
    |                         |
    s_________________________|

    s : position that start to draw line
    c : current position, also means center of the screen
    inner rect: our rendering area, also means screen
    outside rect: the area to draw line
    the size of outside rect is just twice as the screen size.
*/
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    const wxSize currentSize = m_pComponentWindow->GetSize();
    uint32_t clientAreaGridCountX = currentSize.x / m_fCellSize;
    uint32_t clientAreaGridCountY = currentSize.y / m_fCellSize;

    const CVec3& cameraPos = m_pCamera->GetViewPos();
    CVec2 startPos;
    startPos.X() = cameraPos.X() - clientAreaGridCountX * m_fCellSize;
    startPos.Y() = cameraPos.Y() - clientAreaGridCountY * m_fCellSize;

    // Clip the pos to grid.
    startPos.X() = (int)(startPos.X() / m_fCellSize) * m_fCellSize;
    startPos.Y() = (int)(startPos.Y() / m_fCellSize) * m_fCellSize;

    static const CColor lineColor = 0x60606060;
    for (uint32_t j = 0; j < clientAreaGridCountY * 2; ++j)
    {
        CVertexPC startPoint;
        startPoint.color = lineColor;
        startPoint.position.X() = startPos.X();
        startPoint.position.Y() = startPos.Y() + j * m_fCellSize;
        startPoint.position.Z() = MAX_Z_VALUE;

        CVertexPC endPoint;
        endPoint.color = lineColor;
        endPoint.position.X() = startPos.X() + clientAreaGridCountX * 2 * m_fCellSize;
        endPoint.position.Y() = startPos.Y() + j * m_fCellSize;
        endPoint.position.Z() = MAX_Z_VALUE;

        pRenderMgr->Render2DLine(startPoint, endPoint);
    }
    for (uint32_t i = 0; i < clientAreaGridCountX * 2; ++i)
    {
        CVertexPC startPoint;
        startPoint.color = lineColor;
        startPoint.position.X() = startPos.X() + i * m_fCellSize;
        startPoint.position.Y() = startPos.Y();
        startPoint.position.Z() = MAX_Z_VALUE;

        CVertexPC endPoint;
        endPoint.color = lineColor;
        endPoint.position.X() = startPos.X() + i * m_fCellSize;
        endPoint.position.Y() = startPos.Y() + clientAreaGridCountY * 2 * m_fCellSize;
        endPoint.position.Z() = MAX_Z_VALUE;

        pRenderMgr->Render2DLine(startPoint, endPoint);
    }

    //Draw Center Pos
    CVertexPC tmp[4];
    tmp[0].position.X() = 0;
    tmp[0].position.Y() = m_fCellSize;
    tmp[0].position.Z() = MAX_Z_VALUE + 1;
    tmp[0].color = 0xffffffff;

    tmp[1].position.X() = 0;
    tmp[1].position.Y() = -m_fCellSize;
    tmp[1].position.Z() = MAX_Z_VALUE + 1;
    tmp[1].color = 0xffffffff;


    tmp[2].position.X() = -m_fCellSize;
    tmp[2].position.Y() = 0;
    tmp[2].position.Z() = MAX_Z_VALUE + 1;
    tmp[2].color = 0xffffffff;

    tmp[3].position.X() = m_fCellSize;
    tmp[3].position.Y() = 0;
    tmp[3].position.Z() = MAX_Z_VALUE + 1;
    tmp[3].color = 0xffffffff;
    pRenderMgr->Render2DLine(tmp[0], tmp[1]);
    pRenderMgr->Render2DLine(tmp[2], tmp[3]);
}

void CComponentRenderObject::RenderDraggingDependencyLine()
{
    CComponentProxy* pSelectedComponent = m_pMainFrame->GetSelectedComponent();
    if (wxGetMouseState().LeftIsDown() && pSelectedComponent)
    {
        if (m_pComponentWindow->GetDraggingDependency() != NULL)
        {
            int iDependencyPosX = 0;
            int iDependencyPosY = 0;
            uint32_t uIndex = m_pComponentWindow->GetDraggingDependency()->GetIndex();
            pSelectedComponent->GetGraphics()->GetDependencyPosition(uIndex, &iDependencyPosX, &iDependencyPosY);
            CVertex2DPC line[2];
            ConvertGridPosToWorldPos(iDependencyPosX, iDependencyPosY, &line[0].position.X(), &line[0].position.Y());
            line[0].position.X() += m_fCellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyWidth();
            line[0].position.Y() -= m_fCellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyHeight();
            wxPoint pos = wxGetMouseState().GetPosition();
            // Convert screen mouse position to rendering area position
            wxPoint clientPos = m_pComponentWindow->ScreenToClient(wxGetMousePosition());
            ConvertWindowPosToWorldPos(clientPos, &line[1].position.X(), &line[1].position.Y());
            line[0].color = (m_pComponentWindow->GetConnectComponent() != NULL) ? 0x00ff00ff : 0xff0000ff;
            line[1].color = (m_pComponentWindow->GetConnectComponent() != NULL) ? 0x00ff00ff : 0xff0000ff;
            CRenderManager::GetInstance()->Render2DLine(line[0], line[1]);
        }
    }
}

void CComponentRenderObject::RenderComponents()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_ComponnetInstance)
    CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_2D);
    if (CComponentGraphic_GL::m_pRenderBatch != NULL)
    {
        renderGroup->AddRenderBatch(CComponentGraphic_GL::m_pRenderBatch);
    }
    //1. Render all component instance.
    uint32_t uFileId = CComponentProxyManager::GetInstance()->GetCurrLoadFileId();
    if (uFileId != 0xFFFFFFFF)
    {
        const std::map<uint32_t, CComponentProxy*>& componentsInCurScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
        for (auto iter = componentsInCurScene.begin(); iter != componentsInCurScene.end(); ++iter)
        {
            CComponentGraphic* pGraphic = iter->second->GetGraphics();
            if (pGraphic)
            {
                pGraphic->OnRender(m_fCellSize, iter->second == m_pMainFrame->GetSelectedComponent());
            }
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_ComponnetInstance)

    //2. Render dragging component
    if (m_pComponentWindow->GetDraggingComponent())
    {
        // Convert screen mouse position to rendering area position
        wxPoint clientPos = m_pComponentWindow->ScreenToClient(wxGetMousePosition());
        float fMouseWorldPosX = 0;
        float fMouseWorldPosY = 0;
        ConvertWindowPosToWorldPos(clientPos, &fMouseWorldPosX, &fMouseWorldPosY);
        CComponentGraphic* pDraggingGraphics = m_pComponentWindow->GetDraggingComponent()->GetGraphics();
        int x = 0;
        int y = 0;
        CVec2 worldPos;
        worldPos.Fill(fMouseWorldPosX, fMouseWorldPosY);
        ConvertWorldPosToGridPos(&worldPos, &x, &y);
        pDraggingGraphics->SetPosition(x, y);
        pDraggingGraphics->OnRender(m_fCellSize, true);

        //BEATS_PRINT(_T("You must be dragging at world Pos%f, %f gridPos %d %d\n"), fMouseWorldPosX, fMouseWorldPosY, x, y);
    }
}

void CComponentRenderObject::ConvertWorldPosToGridPos(const CVec2* pVectorPos, int* pOutGridX, int* pOutGridY)
{
    *pOutGridX = floor(pVectorPos->X() / m_fCellSize);
    *pOutGridY = ceil(pVectorPos->Y() / m_fCellSize);
}

void CComponentRenderObject::ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY)
{
    *pOutWorldPosX = gridX * m_fCellSize;
    *pOutWorldPosY = gridY * m_fCellSize;
}

void CComponentRenderObject::ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY)
{
    wxSize clientSize = m_pComponentWindow->GetClientSize();
    const CVec3& cameraPos = m_pCamera->GetViewPos();
    *pOutWorldPosX = (windowPos.x - clientSize.x * 0.5f) + cameraPos.X();
    *pOutWorldPosY = (windowPos.y - clientSize.y * 0.5f) + cameraPos.Y();
}