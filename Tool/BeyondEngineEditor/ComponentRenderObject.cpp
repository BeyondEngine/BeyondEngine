#include "stdafx.h"
#include "ComponentRenderObject.h"
#include "EditorMainFrame.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "PerformDetector/PerformDetector.h"
#include "Render/RenderManager.h"
#include "Render/Camera.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescriptionLine.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "ComponentGraphics_GL.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderGroup.h"

static const float MAX_Z_VALUE = -1000.f;
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
    size_t clientAreaGridCountX = currentSize.x / m_fCellSize;
    size_t clientAreaGridCountY = currentSize.y / m_fCellSize;

    const kmVec3& cameraPos = m_pCamera->GetViewPos();
    kmVec2 startPos;
    startPos.x = -cameraPos.x - clientAreaGridCountX * m_fCellSize;
    startPos.y = -cameraPos.y - clientAreaGridCountY * m_fCellSize;

    // Clip the pos to grid.
    startPos.x = (int)(startPos.x / m_fCellSize) * m_fCellSize;
    startPos.y = (int)(startPos.y / m_fCellSize) * m_fCellSize;

    static const CColor lineColor = 0x60606060;
    for (size_t j = 0; j < clientAreaGridCountY * 2; ++j)
    {
        CVertexPC startPoint;
        startPoint.color = lineColor;
        startPoint.position.x = startPos.x;
        startPoint.position.y = startPos.y + j * m_fCellSize;
        startPoint.position.z = MAX_Z_VALUE;

        CVertexPC endPoint;
        endPoint.color = lineColor;
        endPoint.position.x = startPos.x + clientAreaGridCountX * 2 * m_fCellSize;
        endPoint.position.y = startPos.y + j * m_fCellSize;
        endPoint.position.z = MAX_Z_VALUE;

        pRenderMgr->Render2DLine(startPoint, endPoint);
    }
    for (size_t i = 0; i < clientAreaGridCountX * 2; ++i)
    {
        CVertexPC startPoint;
        startPoint.color = lineColor;
        startPoint.position.x = startPos.x + i * m_fCellSize;
        startPoint.position.y = startPos.y;
        startPoint.position.z = MAX_Z_VALUE;

        CVertexPC endPoint;
        endPoint.color = lineColor;
        endPoint.position.x = startPos.x + i * m_fCellSize;
        endPoint.position.y = startPos.y + clientAreaGridCountY * 2 * m_fCellSize;
        endPoint.position.z = MAX_Z_VALUE;

        pRenderMgr->Render2DLine(startPoint, endPoint);
    }

    //Draw Center Pos
    CVertexPC tmp[4];
    tmp[0].position.x = 0;
    tmp[0].position.y = m_fCellSize;
    tmp[0].position.z = MAX_Z_VALUE + 1;
    tmp[0].color = 0xffffffff;

    tmp[1].position.x = 0;
    tmp[1].position.y = -m_fCellSize;
    tmp[1].position.z = MAX_Z_VALUE + 1;
    tmp[1].color = 0xffffffff;


    tmp[2].position.x = -m_fCellSize;
    tmp[2].position.y = 0;
    tmp[2].position.z = MAX_Z_VALUE + 1;
    tmp[2].color = 0xffffffff;

    tmp[3].position.x = m_fCellSize;
    tmp[3].position.y = 0;
    tmp[3].position.z = MAX_Z_VALUE + 1;
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
            size_t uIndex = m_pComponentWindow->GetDraggingDependency()->GetIndex();
            pSelectedComponent->GetGraphics()->GetDependencyPosition(uIndex, &iDependencyPosX, &iDependencyPosY);
            CVertex2DPC line[2];
            ConvertGridPosToWorldPos(iDependencyPosX, iDependencyPosY, &line[0].position.x, &line[0].position.y);
            line[0].position.x += m_fCellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyWidth();
            line[0].position.y -= m_fCellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyHeight();
            wxPoint pos = wxGetMouseState().GetPosition();
            // Convert screen mouse position to rendering area position
            wxPoint clientPos = m_pComponentWindow->ScreenToClient(wxGetMousePosition());
            ConvertWindowPosToWorldPos(clientPos, &line[1].position.x, &line[1].position.y);
            line[0].color = (m_pComponentWindow->GetConnectComponent() != NULL) ? 0x00ff00ff : 0xff0000ff;
            line[1].color = (m_pComponentWindow->GetConnectComponent() != NULL) ? 0x00ff00ff : 0xff0000ff;
            CRenderManager::GetInstance()->Render2DLine(line[0], line[1]);
        }
    }
}

void CComponentRenderObject::RenderComponents()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_ComponnetInstance)
    CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_2D);
    if (CComponentGraphic_GL::m_pRenderBatch != NULL)
    {
        renderGroup->AddRenderBatch(CComponentGraphic_GL::m_pRenderBatch);
    }
    //1. Render all component instance.
    const TString& strCurWorkingFile = CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath();
    if (!strCurWorkingFile.empty())
    {
        const std::map<size_t, CComponentProxy*>& componentsInCurScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
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
        kmVec2 worldPos;
        kmVec2Fill(&worldPos, fMouseWorldPosX, fMouseWorldPosY);
        ConvertWorldPosToGridPos(&worldPos, &x, &y);
        pDraggingGraphics->SetPosition(x, y);
        pDraggingGraphics->OnRender(m_fCellSize, true);

        //BEATS_PRINT(_T("You must be dragging at world Pos%f, %f gridPos %d %d\n"), fMouseWorldPosX, fMouseWorldPosY, x, y);
    }
}

void CComponentRenderObject::ConvertWorldPosToGridPos(const kmVec2* pVectorPos, int* pOutGridX, int* pOutGridY)
{
    *pOutGridX = floor(pVectorPos->x / m_fCellSize);
    *pOutGridY = ceil(pVectorPos->y / m_fCellSize);
}

void CComponentRenderObject::ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY)
{
    *pOutWorldPosX = gridX * m_fCellSize;
    *pOutWorldPosY = gridY * m_fCellSize;
}

void CComponentRenderObject::ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY)
{
    wxSize clientSize = m_pComponentWindow->GetClientSize();
    const kmVec3& cameraPos = m_pCamera->GetViewPos();
    *pOutWorldPosX = (windowPos.x - clientSize.x * 0.5f) - cameraPos.x;
    *pOutWorldPosY = (windowPos.y - clientSize.y * 0.5f) - cameraPos.y;
}