#include "stdafx.h"
#include <wx/numdlg.h>
#include "BeyondEngineEditorComponentWindow.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"
#include "Render/RenderManager.h"
#include "Render/Renderer.h"
#include "Render/Camera.h"
#include "Render/ShaderProgram.h"
#include "Render/Shader.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescriptionLine.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentReference.h"
#include "Render/RenderGroupManager.h"
#include "Utility/PerformDetector/PerformDetector.h"
#include "WxGLRenderWindow.h"
#include "ComponentRenderObject.h"
#include "Render/Viewport.h"
#include "Scene/Scene.h"
#include "Scene/Node2D.h"
#include "Scene/SceneManager.h"
#include "Framework/Application.h"
#include "ComponentGraphics_GL.h"
#include "GUI/WindowManager.h"
#include "GUI/Window/Window.h"

BEGIN_EVENT_TABLE(CBeyondEngineEditorComponentWindow, wxGLCanvas)
    EVT_SIZE(CBeyondEngineEditorComponentWindow::OnSize)
    EVT_ERASE_BACKGROUND(CBeyondEngineEditorComponentWindow::OnEraseBackground)
    EVT_MOUSEWHEEL(CBeyondEngineEditorComponentWindow::OnMouseMidScroll)
    EVT_MOTION(CBeyondEngineEditorComponentWindow::OnMouseMove)
    EVT_RIGHT_UP(CBeyondEngineEditorComponentWindow::OnMouseRightUp)
    EVT_RIGHT_DOWN(CBeyondEngineEditorComponentWindow::OnMouseRightDown)
    EVT_LEFT_UP(CBeyondEngineEditorComponentWindow::OnMouseLeftUp)
    EVT_LEFT_DOWN(CBeyondEngineEditorComponentWindow::OnMouseLeftDown)
    EVT_KEY_DOWN(CBeyondEngineEditorComponentWindow::OnKeyDown)
    EVT_MOUSE_CAPTURE_LOST(CBeyondEngineEditorGLWindow::OnCaptureLost)
END_EVENT_TABLE()

static const float MAX_Z_VALUE = -1000.f;
static const float MIN_CELL_SIZE = 5.f;
static const int MIN_SKEW_DEPENDENCY = 4;
static const int MIN_SKEW_DEPENDENCY_HIGHT = 3;

enum EComponentMenuType
{
    eCMT_Copy,
    eCMT_Paste,
    eCMT_Delete,
    eCMT_Search,
    eCMT_CreateRef,
    eCMT_SetUserDefineName,

    eCMT_Count,
    eCMT_Force32Bit = 0xFFFFFFFF
};

CBeyondEngineEditorComponentWindow::CBeyondEngineEditorComponentWindow(wxWindow *parent,
                                                     wxGLContext* pContext,
                                                     const wxPoint& pos,
                                                     const wxSize& size, long style,
                                                     const wxString& name)
                                                     : CBeyondEngineEditorGLWindow(parent, pContext, NULL, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
                                                     , m_iWidth(0)
                                                     , m_iHeight(0)
                                                     , m_pOutX(0)
                                                     , m_pOutY(0)
                                                     , m_fCellSize(15.0)
                                                     , m_pLineProgram(NULL)
                                                     , m_pAutoSetDependency(NULL)
                                                     , m_pDraggingComponent(NULL)
                                                     , m_pConnectComponent(NULL)
                                                     , m_pCopyComponent(NULL)
                                                     , m_pDraggingDependency(NULL)
                                                     , m_pSelectedDependencyLine(NULL)
                                                     , m_pClickedComponent(NULL)
                                                     , m_pNode(NULL)
{
    BEATS_ASSERT(pContext);
    m_pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    kmVec2Fill(&m_startDragPos, 0, 0);

    m_pComponentMenu = new wxMenu;
    m_pAddDependencyMenu = new wxMenu;
    m_pComponentMenu->Append(eCMT_Copy, _T("拷贝"));
    m_pComponentMenu->Append(eCMT_Paste, _T("粘贴"));
    m_pComponentMenu->Append(eCMT_Delete, _T("删除"));
    m_pComponentMenu->Append(eCMT_Search, _T("搜索"));
    m_pComponentMenu->Append(eCMT_CreateRef, _T("创建引用"));
    m_pComponentMenu->Append(eCMT_SetUserDefineName, _T("设置名称"));

    m_pComponentMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CBeyondEngineEditorComponentWindow::OnComponentMenuClicked), NULL, this);
    m_pAddDependencyMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CBeyondEngineEditorComponentWindow::OnDependencyMenuClicked), NULL, this);
    m_pCamera = new CCamera(CCamera::eCT_2D);
    GetRenderWindow()->SetScaleFactor(1.f);
    GetRenderWindow()->GetViewport()->SetClearColor(0x044080FF);

    m_pNode = new CComponentRenderObject;
    m_pNode->SetComponentWindow(this);
    m_pNode->SetMainFrame(m_pMainFrame);
    m_pNode->SetCamera(m_pCamera);
    m_pNode->SetCellSize(m_fCellSize);
    m_pNode->Initialize();
}

CBeyondEngineEditorComponentWindow::~CBeyondEngineEditorComponentWindow()
{
    BEATS_SAFE_DELETE(m_pComponentMenu);
    BEATS_SAFE_DELETE(m_pAddDependencyMenu);
    BEATS_SAFE_DELETE(m_pCopyComponent);
    BEATS_SAFE_DELETE(m_pCamera);
    m_pNode->Uninitialize();
    BEATS_SAFE_DELETE(m_pNode);
}
CCamera* CBeyondEngineEditorComponentWindow::GetCamera()
{
    return m_pCamera;
}

void CBeyondEngineEditorComponentWindow::UpdateAllDependencyLine()
{
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentProxyManager::GetInstance()->GetInstance()->GetComponentInstanceMap();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::const_reverse_iterator iter = pInstanceMap->rbegin();
    for (; iter != pInstanceMap->rend(); ++iter)
    {
        std::map<size_t, CComponentBase*>::const_reverse_iterator subIter = iter->second->rbegin();
        for (; subIter != iter->second->rend(); ++subIter)
        {
            CComponentProxy* pComponent = static_cast<CComponentProxy*>(subIter->second);
            const std::vector<CDependencyDescription*>* pDependencies = pComponent->GetDependencies();
            if (pDependencies != NULL)
            {
                size_t uDepedencyCount = pDependencies->size();
                for (size_t i = 0; i < uDepedencyCount; ++i)
                {
                    CDependencyDescription* pDesc = pComponent->GetDependency(i);
                    size_t uDependencyLineCount = pDesc->GetDependencyLineCount();
                    for (size_t j = 0; j < uDependencyLineCount; ++j)
                    {
                        pDesc->GetDependencyLine(j)->UpdateRect(m_fCellSize, true);
                    }
                }
            }
        }
    }
}

void CBeyondEngineEditorComponentWindow::DeleteSelectedComponent()
{
    CComponentProxy* pSelectedComponent = m_pMainFrame->GetSelectedComponent();
    if (pSelectedComponent != NULL)
    {
        int iResult = wxMessageBox(_T("真的要删除这个组件吗？"), _T("删除确认"), wxYES_NO | wxCENTRE);
        if (iResult == wxYES)
        {
            m_pMainFrame->SelectComponent(NULL);
            CComponentProxyManager::GetInstance()->OnDeleteComponentInScene(pSelectedComponent);
        }
    }
}

void CBeyondEngineEditorComponentWindow::DeleteSelectedDependencyLine()
{
    if (m_pSelectedDependencyLine != NULL)
    {
        int iResult = wxMessageBox(_T("真的要删除这个依赖吗？"), _T("删除确认"), wxYES_NO | wxCENTRE);
        if (iResult == wxYES)
        {
            BEATS_SAFE_DELETE(m_pSelectedDependencyLine);
        }
    }
}

void CBeyondEngineEditorComponentWindow::ConvertWorldPosToGridPos(const kmVec2* pVectorPos, int* pOutGridX, int* pOutGridY)
{
    *pOutGridX = floor(pVectorPos->x / m_fCellSize);
    *pOutGridY = ceil(pVectorPos->y / m_fCellSize);
}

void CBeyondEngineEditorComponentWindow::ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY)
{
    *pOutWorldPosX = gridX * m_fCellSize;
    *pOutWorldPosY = gridY * m_fCellSize;
}

void CBeyondEngineEditorComponentWindow::ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY)
{
    wxSize clientSize = GetClientSize();
    const kmVec3& cameraPos = GetCamera()->GetViewPos();
    *pOutWorldPosX = (windowPos.x - clientSize.x * 0.5f) - cameraPos.x;
    *pOutWorldPosY = (windowPos.y - clientSize.y * 0.5f) - cameraPos.y;
}

void CBeyondEngineEditorComponentWindow::OnSize(wxSizeEvent& /*event*/)
{
    ResetProjectionMode();
}

void CBeyondEngineEditorComponentWindow::OnEraseBackground(wxEraseEvent& /*event*/)
{

}

void CBeyondEngineEditorComponentWindow::OnMouseMidScroll(wxMouseEvent& event)
{
    // Resize render area.
    int rotation = event.GetWheelRotation();
    const kmVec3& cameraPos = GetCamera()->GetViewPos();

    int gridPosX = cameraPos.x / m_fCellSize;
    int gridPosY = cameraPos.y / m_fCellSize;

    float delta = m_fCellSize* 0.1f * rotation / event.GetWheelDelta();
    m_fCellSize += delta;
    if (m_fCellSize <= MIN_CELL_SIZE)
    {
        m_fCellSize = MIN_CELL_SIZE;
    }
    float maxSize = min(GetSize().GetWidth(), GetSize().GetHeight());
    if (m_fCellSize > maxSize)
    {
        m_fCellSize = maxSize;
    }
    GetCamera()->SetViewPos(gridPosX * m_fCellSize, gridPosY * m_fCellSize, cameraPos.z);
    UpdateAllDependencyLine();
    m_pNode->SetCellSize(m_fCellSize);
    event.Skip(true);
}

void CBeyondEngineEditorComponentWindow::OnMouseMove(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    // Drag the screen
    if(event.RightIsDown())
    {
        float deltaX = pos.x - m_startDragPos.x;
        float deltaY = pos.y - m_startDragPos.y;
        kmVec3 cameraPos = GetCamera()->GetViewPos();
        cameraPos.x += deltaX;
        cameraPos.y += deltaY;
        GetCamera()->SetViewPos(cameraPos.x, cameraPos.y, cameraPos.z);
        m_startDragPos.x = pos.x;
        m_startDragPos.y = pos.y;
        //BEATS_PRINT(_T("Screen Moved, Current Client Pos %f, %f\n"), m_currentClientPos.x, m_currentClientPos.y);
    }
    if (event.LeftIsDown())
    {
        m_pConnectComponent = NULL;

        //Drag Component
        CComponentProxy* pSelectedComponent = m_pMainFrame->GetSelectedComponent();
        if (pSelectedComponent != NULL)
        {
            if (m_pDraggingDependency == NULL)
            {
                kmVec2 worldPos;
                ConvertWindowPosToWorldPos(pos, &worldPos.x, &worldPos.y);
                int gridPosX = 0;
                int gridPosY = 0;
                ConvertWorldPosToGridPos(&worldPos, &gridPosX, &gridPosY);
                pSelectedComponent->GetGraphics()->SetPosition(gridPosX, gridPosY);
                const std::vector<CDependencyDescription*>* pDependencies = pSelectedComponent->GetDependencies();
                if (pDependencies != NULL)
                {
                    for (size_t i = 0; i < pDependencies->size(); ++i)
                    {
                        CDependencyDescription* pDesc = pDependencies->at(i);
                        size_t uLineCount = pDesc->GetDependencyLineCount();
                        for (size_t j = 0; j < uLineCount; ++j)
                        {
                            pDesc->GetDependencyLine(j)->UpdateRect(m_fCellSize, true);
                        }
                    }
                }
                const std::vector<CDependencyDescriptionLine*>* pBeConnectedLines = pSelectedComponent->GetBeConnectedDependencyLines();
                if (pBeConnectedLines != NULL)
                {
                    for (size_t i = 0; i < pBeConnectedLines->size(); ++i)
                    {
                        CDependencyDescriptionLine* pDescLine = pBeConnectedLines->at(i);
                        pDescLine->UpdateRect(m_fCellSize, true);
                    }
                }
                //BEATS_PRINT(_T("You must dragging component instance %s to worldPos %f %f, gridPos %d %d\n"), pSelectedComponent->GetClassName(), worldPos.x, worldPos.y, gridPosX, gridPosY);
            }
            else
            {
                EComponentAeraRectType aeraType;
                CComponentProxy* pComponentMoveTo = HitTestForComponent(pos, &aeraType);
                if (aeraType == eCART_Connection)
                {
                    if (m_pDraggingDependency->IsMatch(pComponentMoveTo) && pComponentMoveTo != m_pDraggingDependency->GetOwner() && !m_pDraggingDependency->IsInDependency(pComponentMoveTo))
                    {
                        m_pConnectComponent = pComponentMoveTo;
                    }
                }
            }
        }
    }
    else
    {
        m_pDraggingComponent = NULL;
    }
    m_bShowMenu = false;
    event.Skip(true);
}

void CBeyondEngineEditorComponentWindow::OnMouseRightUp(wxMouseEvent& event)
{
    if (HasCapture())
    {
        ReleaseMouse();
    }
    if (m_bShowMenu)
    {
        if (CComponentProxyManager::GetInstance()->GetProject()->GetRootDirectory() != NULL)
        {
            wxPoint pos = event.GetPosition();
            EComponentAeraRectType aeraRectType = eCART_Invalid;
            void* pData = NULL;
            m_pClickedComponent = HitTestForComponent(pos, &aeraRectType, &pData);
            m_pMainFrame->SelectComponent(m_pClickedComponent);
            m_pComponentMenu->Enable(eCMT_Copy, m_pClickedComponent != NULL);
            m_pComponentMenu->Enable(eCMT_Delete, m_pClickedComponent != NULL);
            m_pComponentMenu->Enable(eCMT_Paste, m_pClickedComponent == NULL && m_pCopyComponent != NULL);
            m_pComponentMenu->Enable(eCMT_SetUserDefineName, m_pClickedComponent != NULL);
            if (!m_menuItemAndGuidMap.empty())
            {
                for (auto iter = m_menuItemAndGuidMap.begin(); iter != m_menuItemAndGuidMap.end(); ++iter)
                {
                    m_pAddDependencyMenu->Delete(iter->first);
                }
            }
            m_menuItemAndGuidMap.clear();
            wxMenu* pMenu = NULL;
            if (aeraRectType == eCART_Dependency)
            {
                CDependencyDescription* pDescription = (CDependencyDescription*)(pData);
                m_pAutoSetDependency = pDescription;
                size_t uDependencyGuid = pDescription->GetDependencyGuid();
                std::vector<size_t> derivedClassGuid;
                CComponentProxyManager::GetInstance()->QueryDerivedClass(uDependencyGuid, derivedClassGuid, true);
                derivedClassGuid.insert(derivedClassGuid.begin(), uDependencyGuid);
                CComponentProxyManager::GetInstance()->QueryComponentName(uDependencyGuid);
                for (size_t i = 0; i < derivedClassGuid.size(); ++i)
                {
                    CComponentBase* pBase = CComponentProxyManager::GetInstance()->GetComponentTemplate(derivedClassGuid[i]);
                    if (pBase != NULL)//If it is NULL, it must be an abstract class.
                    {
                        TString strName = CComponentProxyManager::GetInstance()->QueryComponentName(derivedClassGuid[i]);
                        wxMenuItem* pItem = m_pAddDependencyMenu->Append(i, strName);
                        BEATS_ASSERT(m_menuItemAndGuidMap.find(pItem) == m_menuItemAndGuidMap.end());
                        m_menuItemAndGuidMap[pItem] = derivedClassGuid[i];
                    }
                    BEATS_ASSERT(pBase != NULL ||
                        CComponentProxyManager::GetInstance()->GetAbstractComponentNameMap().find(derivedClassGuid[i]) != CComponentProxyManager::GetInstance()->GetAbstractComponentNameMap().end(),
                        _T("We can't get a template component with guid %d while it can't be found in abstract class map!"), derivedClassGuid[i]);
                }
                pMenu = m_pAddDependencyMenu;
            }
            // Force render before menu pop up.
            else
            {
                pMenu = m_pComponentMenu;
            }
            Update();
            PopupMenu(pMenu, wxDefaultPosition);
        }
        m_bShowMenu = false;
    }
    event.Skip(true);
}

void CBeyondEngineEditorComponentWindow::OnMouseRightDown(wxMouseEvent& event)
{
    if (!HasCapture())
    {
        CaptureMouse();
    }
    wxPoint pos = event.GetPosition();
    m_startDragPos.x = pos.x;
    m_startDragPos.y = pos.y;
    m_bShowMenu = true;
    event.Skip(true);
}

void CBeyondEngineEditorComponentWindow::OnMouseLeftDown(wxMouseEvent& event)
{
    if (!HasCapture())
    {
        CaptureMouse();
    }
    wxPoint pos = event.GetPosition();
    m_startDragPos.x = pos.x;
    m_startDragPos.y = pos.y;
    EComponentAeraRectType aeraType = eCART_Invalid;
    void* pData = NULL;
    CComponentProxy* pClickedComponent = HitTestForComponent(pos, &aeraType, &pData);
    BEATS_ASSERT(pClickedComponent == NULL || aeraType != eCART_Invalid);
    m_pMainFrame->SelectComponent(pClickedComponent);
    if (m_pSelectedDependencyLine != NULL)
    {
        m_pSelectedDependencyLine->SetSelected(false);
        m_pSelectedDependencyLine = NULL;
    }
    if (aeraType == eCART_Dependency)
    {
        BEATS_ASSERT(pClickedComponent != NULL);
        m_pDraggingDependency = (CDependencyDescription*)pData;
        if (!m_pDraggingDependency->IsListType())
        {
            m_pDraggingDependency->Hide();
        }
    }
    else if (aeraType == eCART_DependencyLine)
    {
        BEATS_ASSERT(pClickedComponent == NULL);
        m_pSelectedDependencyLine = (CDependencyDescriptionLine*)pData;
        BEATS_ASSERT(m_pSelectedDependencyLine != NULL);
        m_pSelectedDependencyLine->SetSelected(true);
    }
    event.Skip(true);
}

void CBeyondEngineEditorComponentWindow::OnMouseLeftUp(wxMouseEvent& /*event*/)
{
    if (HasCapture())
    {
        ReleaseMouse();
    }
    CDependencyDescriptionLine* pLine = NULL;
    if (m_pConnectComponent != NULL)
    {
        BEATS_ASSERT(m_pDraggingDependency != NULL);
        if (m_pDraggingDependency->GetDependencyLineCount() > 0 && !m_pDraggingDependency->IsListType())
        {
            pLine = m_pDraggingDependency->SetDependency(0, m_pConnectComponent);
        }
        else
        {
            pLine = m_pDraggingDependency->AddDependency(m_pConnectComponent);
        }
    }
    if (pLine != NULL)
    {
        pLine->UpdateRect(m_fCellSize, true);
    }
    if (m_pDraggingDependency != NULL && !m_pDraggingDependency->IsVisible())
    {
        m_pDraggingDependency->Show();
    }
    m_pConnectComponent = NULL;
    m_pDraggingDependency = NULL;
}

void CBeyondEngineEditorComponentWindow::OnComponentMenuClicked(wxMenuEvent& event)
{
    CEditorMainFrame* pMFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CComponentProxy* pComponentProxy = m_pMainFrame->GetSelectedComponent();
    switch (event.GetId())
    {
    case eCMT_Delete:
        {
            BEATS_ASSERT(pComponentProxy != NULL, _T("Impossible to delete component from menu when there is no components selected."));
            DeleteSelectedComponent();
            pMFrame->UpdateComponentInstanceTreeCtrl();
        }
        break;
    case eCMT_Copy:
        {
            BEATS_ASSERT(pComponentProxy != NULL, _T("Impossible to copy component from menu when there is no components selected."));
            if (pComponentProxy != m_pCopyComponent)
            {
                BEATS_SAFE_DELETE(m_pCopyComponent);
                m_pCopyComponent = static_cast<CComponentProxy*>(pComponentProxy->Clone(true, NULL, 0xFFFFFFFF));
            }
        }
        break;
    case eCMT_Paste:
        {
            BEATS_ASSERT(pComponentProxy == NULL && m_pCopyComponent != NULL, _T("Impossible to paste component from menu when there is a component selected or no copy component."));
            CComponentProxy* pNewComponentEditorProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->CreateComponentByRef(m_pCopyComponent, true));
            wxPoint mouseWindowPos = wxGetMousePosition();
            mouseWindowPos = ScreenToClient(mouseWindowPos);
            kmVec2 mouseWorldPos;
            ConvertWindowPosToWorldPos(mouseWindowPos, &mouseWorldPos.x, &mouseWorldPos.y);
            int iGridPosX, iGridPosY;
            ConvertWorldPosToGridPos(&mouseWorldPos, &iGridPosX, &iGridPosY);
            pNewComponentEditorProxy->GetGraphics()->SetPosition(iGridPosX - 1, iGridPosY + 1);
            pMFrame->UpdateComponentInstanceTreeCtrl();
            CComponentProxyManager::GetInstance()->OnCreateComponentInScene(pNewComponentEditorProxy);
        }
        break;
    case eCMT_Search:
        {
            int iComponentId = wxGetNumberFromUser(_T("搜索组件"), _T("组件ID"), _T("搜索组件"), 0, 0xFFFFFFFF, 0x7FFFFFFE);
            if (iComponentId != 0xFFFFFFFF)
            {
                CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
                size_t uFileId = pProject->QueryFileId(iComponentId, false);
                if (uFileId != 0xFFFFFFFF)
                {
                    TString filePath = pProject->GetComponentFileName(uFileId);
                    if (filePath.compare(CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath()) != 0)
                    {
                        TCHAR szInfo[1024];
                        _stprintf(szInfo, _T("搜索到ID为%d的组件位于文件%s,是否跳转到该组件?"), iComponentId, filePath.c_str());
                        int iAnswer = MessageBox(NULL, szInfo, _T("搜索结果"), MB_YESNO);
                        if (iAnswer == IDYES)
                        {
                            m_pMainFrame->ActivateFile(filePath.c_str());
                            CComponentBase* pComponent = CComponentProxyManager::GetInstance()->GetComponentInstance(iComponentId);
                            m_pMainFrame->SelectComponent(static_cast<CComponentProxy*>(pComponent));
                        }
                    }
                    else
                    {
                        CComponentBase* pComponent = CComponentProxyManager::GetInstance()->GetComponentInstance(iComponentId);
                        m_pMainFrame->SelectComponent(static_cast<CComponentProxy*>(pComponent));
                    }
                }
                else
                {
                    TCHAR szInfo[1024];
                    _stprintf(szInfo, _T("未能找到ID为%d的组件."), iComponentId);
                    MessageBox(NULL, szInfo, _T("搜索结果"), MB_OK);
                }
            }
        }
        break;
    case eCMT_CreateRef:
        {
            long id = wxGetNumberFromUser(wxEmptyString, _T("请输入引用组件的ID:"), _T("输入"), 0, 0xFFFFFFFF, 0x7FFFFFFE);
            if (id != 0xFFFFFFFF)
            {
                CComponentBase* pComponentBase = CComponentProxyManager::GetInstance()->GetComponentInstance(id);
                if (pComponentBase != NULL)
                {
                    CComponentProxy* pProxy = down_cast<CComponentProxy*>(pComponentBase);
                    CComponentReference* pNewReference = CComponentProxyManager::GetInstance()->CreateReference(pProxy->GetId(), pProxy->GetGuid());
                    pNewReference->Initialize();
                    CComponentGraphic_GL* pGraphics = down_cast<CComponentGraphic_GL*>(pNewReference->GetGraphics());
                    pGraphics->SetReferenceFlag(true);
                    wxPoint mouseWindowPos = wxGetMousePosition();
                    mouseWindowPos = ScreenToClient(mouseWindowPos);
                    kmVec2 mouseWorldPos;
                    ConvertWindowPosToWorldPos(mouseWindowPos, &mouseWorldPos.x, &mouseWorldPos.y);
                    int iGridPosX, iGridPosY;
                    ConvertWorldPosToGridPos(&mouseWorldPos, &iGridPosX, &iGridPosY);
                    pNewReference->GetGraphics()->SetPosition(iGridPosX - 1, iGridPosY + 1);
                    CComponentProxyManager::GetInstance()->OnCreateComponentInScene(pNewReference);
                }
                else
                {
                    wxMessageBox(wxString::Format(_T("无法找到id为%d的组件！"), id), _T("创建引用失败"), wxOK);
                }
            }
        }
        break;
    case eCMT_SetUserDefineName:
        {
            BEATS_ASSERT(pComponentProxy != NULL);
            wxString strName = wxGetTextFromUser(_T("请输入自定义名称:"), _T("输入"), pComponentProxy->GetUserDefineDisplayName());
            pComponentProxy->SetUserDefineDisplayName(strName);
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Never reach here!"));
        break;
    }
}

void CBeyondEngineEditorComponentWindow::OnDependencyMenuClicked(wxMenuEvent& event)
{
    size_t uIndex = event.GetId();
    wxMenuItem* pMenuItem = m_pAddDependencyMenu->FindChildItem(uIndex);
    auto iter = m_menuItemAndGuidMap.find(pMenuItem);
    CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->CreateComponent(iter->second, true));
    BEATS_ASSERT(m_pAutoSetDependency != NULL);
    int countLine = m_pAutoSetDependency->GetDependencyLineCount();
    if ( countLine == 0)
    {
        m_pClickedComponent->GetGraphics()->GetPosition(&m_pOutX, &m_pOutY);
        m_pOutX = m_pOutX + MIN_WIDTH + CONNECTION_WIDTH + MIN_SKEW_DEPENDENCY;
        m_pOutY = m_pOutY - MIN_SKEW_DEPENDENCY;
        pProxy->GetGraphics()->SetPosition(m_pOutX, m_pOutY);
    }
    else
    {
        pProxy->GetGraphics()->SetPosition(m_pOutX, m_pOutY + MIN_SKEW_DEPENDENCY_HIGHT * countLine);
    }
    m_pAutoSetDependency->AddDependency(pProxy);
    this->UpdateAllDependencyLine();
    CEditorMainFrame* pWFram = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pWFram->UpdateComponentInstanceTreeCtrl();
    CComponentProxyManager::GetInstance()->OnCreateComponentInScene(pProxy);
}

CComponentProxy* CBeyondEngineEditorComponentWindow::HitTestForComponent( const wxPoint& pos, EComponentAeraRectType* pOutAreaType/* = NULL*/, void** pReturnData /* = NULL*/ )
{
    float fClickWorldPosX = 0;
    float fClickWorldPosY = 0;
    ConvertWindowPosToWorldPos(pos, &fClickWorldPosX, &fClickWorldPosY);
    //BEATS_PRINT(_T("Left Click at window pos :%d, %d worldPos: %f, %f\n"), pos.x, pos.y, fClickWorldPosX, fClickWorldPosY);
    bool bFoundResult = false;
    CComponentProxy* pClickedComponent = NULL;
    const std::map<size_t, CComponentProxy*>& componentsInCurScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
    std::map<size_t, CComponentProxy*>::const_iterator iter = componentsInCurScene.begin();
    for (; iter != componentsInCurScene.end() && !bFoundResult; ++iter)
    {
        CComponentProxy* pComponentProxy = down_cast<CComponentProxy*>(iter->second);
        BEATS_ASSERT(pComponentProxy != NULL);
        CComponentGraphic* pGraphics = pComponentProxy->GetGraphics();
        int x = 0;
        int y = 0;
        pGraphics->GetPosition(&x, &y);    // Left top pos.
        float fWorldPosX = 0;
        float fWorldPosY = 0;
        ConvertGridPosToWorldPos(x, y, &fWorldPosX, &fWorldPosY);
        float width = pGraphics->GetWidth() * m_fCellSize;
        float height = pGraphics->GetHeight() * m_fCellSize;
        float fDelatX = fClickWorldPosX - fWorldPosX;
        if ( fDelatX > 0 && fDelatX < width)
        {
            float fDelatY = fClickWorldPosY - fWorldPosY;
            if (fDelatY > 0 && fDelatY < height)
            {
                pClickedComponent = pComponentProxy;
                if (pOutAreaType != NULL)
                {
                    kmVec2 worldPos;
                    worldPos.x = fClickWorldPosX;
                    worldPos.y = fClickWorldPosY;
                    int iClickGridPosX = 0;
                    int iClickGridPosY = 0;
                    ConvertWorldPosToGridPos(&worldPos, &iClickGridPosX, &iClickGridPosY);
                    *pOutAreaType = pGraphics->HitTestForAreaType(iClickGridPosX, iClickGridPosY, pReturnData);
                }
                bFoundResult = true;
                break;
            }
        }
        //check dependency line.
        const std::vector<CDependencyDescription*>* pDependencies = pComponentProxy->GetDependencies();
        if (pDependencies != NULL)
        {
            for (size_t i = 0; i < pDependencies->size() && !bFoundResult; ++i)
            {
                CDependencyDescription* pDependency = pDependencies->at(i);
                int iDependencyX, iDependencyY;
                pGraphics->GetDependencyPosition(i, &iDependencyX, &iDependencyY);
                for (size_t j = 0; j < pDependency->GetDependencyLineCount(); ++j)
                {
                    CDependencyDescriptionLine* pLine = pDependency->GetDependencyLine(j);
                    bool bInLine = pLine->HitTest(fClickWorldPosX, fClickWorldPosY);
                    if (bInLine)
                    {
                        if (pOutAreaType != NULL && pReturnData != NULL)
                        {
                            *pOutAreaType = eCART_DependencyLine;
                            *pReturnData = pLine;
                            break;
                        }
                        bFoundResult = true;
                    }
                }
            }
        }
    }
    return pClickedComponent;
}

void CBeyondEngineEditorComponentWindow::ResetProjectionMode()
{
    GetClientSize(&m_iWidth, &m_iHeight);
    if ( IsShownOnScreen() )
    {
        GetRenderWindow()->SetSize(m_iWidth, m_iHeight);
        CRenderManager::GetInstance()->SetCurrentRenderTarget(GetRenderWindow());
    }
    kmVec2 centerOffset;
    kmVec2Fill(&centerOffset, m_iWidth * -0.5f, m_iHeight * -0.5f);
    GetCamera()->SetCenterOffset(centerOffset);
}

void CBeyondEngineEditorComponentWindow::SetDraggingComponent( CComponentProxy* pDraggingComponent )
{
    m_pDraggingComponent = pDraggingComponent;
}

CComponentProxy* CBeyondEngineEditorComponentWindow::GetDraggingComponent()
{
    return m_pDraggingComponent;
}

CComponentProxy* CBeyondEngineEditorComponentWindow::GetConnectComponent()
{
    return m_pConnectComponent;
}

CDependencyDescription* CBeyondEngineEditorComponentWindow::GetDraggingDependency()
{
    return m_pDraggingDependency;
}

void CBeyondEngineEditorComponentWindow::OnKeyDown( wxKeyEvent& event )
{
    int nKeyCode = event.GetKeyCode();
    switch (nKeyCode)
    {
    case WXK_DELETE:
        {
            DeleteSelectedComponent();
            DeleteSelectedDependencyLine();
            CEditorMainFrame* pMFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
            pMFrame->UpdateComponentInstanceTreeCtrl();
        }
        break;
    case 44:
    case 46:
        {
            if (m_pSelectedDependencyLine != NULL && wxGetKeyState(WXK_CONTROL))
            {
                CDependencyDescription* pDependency = m_pSelectedDependencyLine->GetOwnerDependency();
                CDependencyDescriptionLine* pSwapLine = NULL;
                if (nKeyCode == 44 && m_pSelectedDependencyLine->GetIndex() > 0)
                {
                    pSwapLine = pDependency->GetDependencyLine(m_pSelectedDependencyLine->GetIndex() - 1);
                }
                else if (nKeyCode == 46 && (m_pSelectedDependencyLine->GetIndex() < pDependency->GetDependencyLineCount() - 1))
                {
                    pSwapLine = pDependency->GetDependencyLine(m_pSelectedDependencyLine->GetIndex() + 1);
                }
                if (pSwapLine)
                {
                    pDependency->SwapLineOrder(m_pSelectedDependencyLine->GetIndex(), pSwapLine->GetIndex());
                    m_pSelectedDependencyLine->UpdateRect(m_fCellSize, true);
                    pSwapLine->UpdateRect(m_fCellSize, true);

                    m_pSelectedDependencyLine->SetSelected(false);
                    m_pSelectedDependencyLine = pSwapLine;
                    m_pSelectedDependencyLine->SetSelected(true);
                }
            }
        }
        break;
    default:
        break;
    }
}

void CBeyondEngineEditorComponentWindow::Update()
{
    if ( IsShownOnScreen())
    {
        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderComponentWindow)
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
        CRenderGroupManager::GetInstance()->SetDefault2DCamera(m_pCamera);
        CViewport* pCurViewport = m_pRenderWindow->GetViewport();
        pCurViewport->Apply();
        m_pNode->Render();
        CRenderManager::GetInstance()->Render();
        m_pRenderWindow->GetCanvas()->SwapBuffers();
#ifdef _DEBUG
        CEngineCenter::GetInstance()->ResetDrawCall();
#endif
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderComponentWindow)
    }
    //HACK: Force disable alpha test
    CRenderer::GetInstance()->DisableGL((CBoolRenderStateParam::EBoolStateParam)GL_ALPHA_TEST);
}

void CBeyondEngineEditorComponentWindow::OnCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    if (HasCapture())
    {
        ReleaseCapture();
    }
}
