#include "stdafx.h"
#include "ViewAgentBase.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "BeyondEngineEditorGLWindow.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "Render/RenderManager.h"
#include "Render/CoordinateRenderObject.h"
#include "WxGLRenderWindow.h"
#include "Scene/SceneManager.h"
#include "EditorSceneWindow.h"
#include "Event/TouchDelegate.h"
#include "ChangeValueRecord.h"
#include "OperationRecordManager.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "ParticleControlWnd.h"
#include "ParticleSystem/ParticleManager.h"

CViewAgentBase::CViewAgentBase()
: m_pGLWindow(nullptr)
{
    m_pMainFrame = ((CEngineEditor*)wxApp::GetInstance())->GetMainFrame();
}

CViewAgentBase::~CViewAgentBase()
{

}

void CViewAgentBase::ProcessMouseEvent( wxMouseEvent& event )
{
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pRenderTarget != NULL);
    wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
    BEATS_ASSERT(pSceneRenderWnd != NULL);
    float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetLogicWidth() : pSceneRenderWnd->GetScaleFactor();

    CommonHandleMouse(event);
    if (!event.GetSkipped())
    {
        if (CSceneManager::GetInstance()->GetCurrentScene() == nullptr && !CSceneManager::GetInstance()->GetSwitchSceneState())
        {
            // Simulate change Fov
            if (event.GetWheelRotation() != 0)
            {
                event.Skip(true);
                CCamera *pCamera = m_pGLWindow->GetCamera();
                float fNewFov = event.GetWheelRotation() < 0 ? 0.5f : -0.5f;
                if (event.ShiftDown())
                {
                    fNewFov *= 3;
                }
                pCamera->SetFOV(pCamera->GetFOV() + fNewFov);
            }
            // Select objects
            if (event.LeftDown())
            {
                const std::map<uint32_t, CComponentProxy*>& componentsInScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
                for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
                {
                    CComponentProxy* pProxy = iter->second;
                    if (pProxy->GetHostComponent() &&
                        pProxy->GetBeConnectedDependencyLines()->size() == 0)
                    {
                        CNode* pNode = dynamic_cast<CNode*>(pProxy->GetHostComponent());
                        if (pNode && pNode->GetParentNode() == NULL)
                        {
                            if (pNode->HitTest(CVec2(event.GetX() / fScaleFactor, event.GetY() / fScaleFactor)))
                            {
                                m_pMainFrame->SelectComponent(pProxy);
                            }
                        }
                    }
                }
            }
            // Simulate the scene move logic
            if (event.LeftIsDown())
            {
                const CVec2 curPos = CVec2(event.GetX() / fScaleFactor, event.GetY() / fScaleFactor);
                CCamera *pCamera = m_pGLWindow->GetCamera();
                CVec3 newCameraPos = pCamera->QueryCameraPos(m_startMoveWorldPos, curPos, pCamera->GetViewMatrix(),
                    pCamera->GetNear(), pCamera->GetFOV(), pCamera->GetViewPos().Y());
                pCamera->SetViewPos(newCameraPos);
            }
        }
        else
        {
            DeliverMouseEventToTouch(event);
        }
    }
}

void CViewAgentBase::InView()
{

}

void CViewAgentBase::OutView()
{
}

void CViewAgentBase::CreateTools()
{

}

void CViewAgentBase::SelectComponent( CComponentProxy* pComponentProxy )
{
    CNode* pSelectNode = nullptr;
    if (pComponentProxy != nullptr && pComponentProxy->GetHostComponent() != nullptr)
    {
        pSelectNode = dynamic_cast<CNode*>(pComponentProxy->GetHostComponent());
    }
    CRenderManager::GetInstance()->GetCoordinateRenderObject()->SetRenderNode(pSelectNode);
    bool bSelectParticleEmitter = pSelectNode != nullptr && pSelectNode->GetGuid() == CParticleEmitter::REFLECT_GUID;
    m_pMainFrame->GetParticleControlWnd()->AttachEmitter(bSelectParticleEmitter ? down_cast<CParticleEmitter*>(pSelectNode) : nullptr);
    m_pMainFrame->GetParticleControlWnd()->Show(bSelectParticleEmitter);
    CParticleManager::GetInstance()->m_pCurrSelectEmitter = nullptr;
}

void CViewAgentBase::OnOpenComponentFile()
{

}

void CViewAgentBase::OnTreeCtrlSelect(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnActivateTreeItem(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnCloseComponentFile()
{

}

void CViewAgentBase::OnUpdateComponentInstance()
{

}

void CViewAgentBase::OnPropertyChanged(wxPropertyGridEvent& /*event*/)
{

}

void CViewAgentBase::OnTreeCtrlRightClick(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnEditTreeItemLabel(wxTreeEvent& /*event*/)
{

}

void CViewAgentBase::OnCommandEvent(wxCommandEvent& /*event*/)
{

}

void CViewAgentBase::Update(float dtt)
{
    if (m_pMainFrame->GetParticleControlWnd()->IsShown())
    {
        m_pMainFrame->GetParticleControlWnd()->Update(dtt);
    }
}

void CViewAgentBase::SetGLWindow(CBeyondEngineEditorGLWindow* pWindow)
{
    m_pGLWindow = pWindow;
}

void CViewAgentBase::CommonHandleMouse(wxMouseEvent& event)
{
    CCoordinateRenderObject* pCoordinateRenderObj = CRenderManager::GetInstance()->GetCoordinateRenderObject();
    static wxPoint startDragPos;
    static bool bCursorSwitcher = true;
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pRenderTarget != NULL);
    wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
    BEATS_ASSERT(pSceneRenderWnd != NULL);
    float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetLogicWidth() : pSceneRenderWnd->GetScaleFactor();

    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        wxWindow* pWindow = (wxWindow*)m_pMainFrame->m_pViewScreen;
        if (!pWindow->HasCapture())
        {
            pWindow->CaptureMouse();
        }
        SetFocus(pWindow->GetHWND());
        if (pCoordinateRenderObj->GetRenderNode() != nullptr)
        {
            CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
            BEATS_ASSERT(pRenderTarget != NULL);
            wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
            BEATS_ASSERT(pSceneRenderWnd != NULL);
            float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetLogicWidth() : pSceneRenderWnd->GetScaleFactor();
            m_mouseDownPos.Fill(event.GetX() / fScaleFactor, event.GetY() / fScaleFactor);
            m_mouseLastPos = m_mouseDownPos;
            if (pCoordinateRenderObj->RefreshTranslateMode(m_mouseDownPos.X(), m_mouseDownPos.Y()))
            {
                m_recordMovePos = pCoordinateRenderObj->GetRenderNode()->GetPosition();
                pCoordinateRenderObj->SetOperateFlag(true);
            }
        }
        CCamera *pCamera = m_pGLWindow->GetCamera();
        m_startMoveWorldPos = pCamera->RayCast(event.GetX() / fScaleFactor, event.GetY() / fScaleFactor);
    }
    else if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        wxWindow* pWindow = (wxWindow*)m_pMainFrame->m_pViewScreen;
        if (!pWindow->HasCapture())
        {
            pWindow->CaptureMouse();
        }
        SetFocus(pWindow->GetHWND());
        startDragPos = event.GetPosition();
    }
    else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        wxWindow* pWindow = (wxWindow*)m_pMainFrame->m_pViewScreen;
        if (pWindow->HasCapture())
        {
            pWindow->ReleaseMouse();
        }
        if (pCoordinateRenderObj->GetOperateFlag())
        {
            CNode* pNode = pCoordinateRenderObj->GetRenderNode();
            if (pNode)
            {
                CPropertyDescriptionBase* pProperty = pNode->GetProxyComponent()->GetProperty("m_pos");
                CChangeValueRecord* pChangeValueRecord = COperationRecordManager::GetInstance()->RequestRecord<CChangeValueRecord>(EOperationRecordType::eORT_ChangeValue);
                pChangeValueRecord->SetPropertyDescription(pProperty);
                pChangeValueRecord->GetOldData() << m_recordMovePos;
                pProperty->Serialize(pChangeValueRecord->GetNewData(), eVT_CurrentValue);
                COperationRecordManager::GetInstance()->AppendRecord(pChangeValueRecord);
                pCoordinateRenderObj->SetOperateFlag(false);
            }
        }
    }
    else if (event.ButtonUp(wxMOUSE_BTN_RIGHT))
    {
        if (!bCursorSwitcher)
        {
            ShowCursor(true);
            bCursorSwitcher = true;
        }
    }
    else if (event.Dragging())
    {
        if (event.RightIsDown())
        {
            wxPoint curPos = event.GetPosition();
            wxPoint pnt = m_pGLWindow->ClientToScreen(startDragPos);
            SetCursorPos(pnt.x, pnt.y);
            float fDeltaX = startDragPos.x - curPos.x;
            float fDeltaY = startDragPos.y - curPos.y;
            wxSize clientSize = m_pGLWindow->GetClientSize();
            fDeltaX /= clientSize.x;
            fDeltaY /= clientSize.y;
            float fScale = m_pGLWindow->GetRenderWindow()->GetScaleFactor();
            fDeltaX /= fScale;
            fDeltaY /= fScale;
            CCamera *pCamera = m_pGLWindow->GetCamera();
            if (pCamera)
            {
                pCamera->Yaw(fDeltaX);
                pCamera->Pitch(fDeltaY);
            }
            if (bCursorSwitcher)
            {
                ShowCursor(false);
                bCursorSwitcher = false;
            }
        }
        else if (event.LeftIsDown())
        {
            if (pCoordinateRenderObj->GetOperateFlag())
            {
                CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
                BEATS_ASSERT(pRenderTarget != NULL);
                wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
                BEATS_ASSERT(pSceneRenderWnd != NULL);
                float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetLogicWidth() : pSceneRenderWnd->GetScaleFactor();
                pCoordinateRenderObj->TranslateObject(event.GetX() / fScaleFactor, event.GetY() / fScaleFactor);
                CNode* pRenderNode = pCoordinateRenderObj->GetRenderNode();
                if (pRenderNode)
                {
                    UPDATE_PROXY_PROPERTY_BY_NAME(pRenderNode, pRenderNode->GetPosition(), "m_pos");
                }
                event.Skip();
            }
            m_mouseLastPos = m_mouseCurrPos;
            m_mouseCurrPos.Fill(event.GetX() / fScaleFactor, event.GetY() / fScaleFactor);
        }
    }
}

void CViewAgentBase::DeliverMouseEventToTouch(wxMouseEvent& event)
{
    wxPoint MousePos = event.GetPosition();
    size_t id = 0;
    float fX = (float)MousePos.x;
    float fY = (float)MousePos.y;
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pRenderTarget != NULL);
    wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
    BEATS_ASSERT(pSceneRenderWnd != NULL);
    float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetLogicWidth() : pSceneRenderWnd->GetScaleFactor();
    fX /= fScaleFactor;
    fY /= fScaleFactor;
    if (event.ButtonDClick(wxMOUSE_BTN_LEFT))
    {
        CTouchDelegate::GetInstance()->OnTouchBegan(1, &id, &fX, &fY);
    }
    else
    {
        if (event.LeftDown())
        {
            CTouchDelegate::GetInstance()->OnTouchBegan(1, &id, &fX, &fY);
        }
        else if (event.LeftUp())
        {
            CTouchDelegate::GetInstance()->OnTouchEnded(1, &id, &fX, &fY);
        }
        else if (event.LeftIsDown())
        {
            CTouchDelegate::GetInstance()->OnTouchMoved(1, &id, &fX, &fY);
        }
    }
    if (event.GetWheelRotation() != 0)
    {
        CTouchDelegate::GetInstance()->OnPinched(EGestureState::eGS_BEGAN, 1.0f);
        CTouchDelegate::GetInstance()->OnPinched(EGestureState::eGS_CHANGED, event.GetWheelRotation() > 0 ? 1.5f : 0.5f);
    }
}

void CViewAgentBase::ProcessKeyboardEvent(wxKeyEvent& event)
{
    CEditorSceneWindow* pSceneWnd = m_pMainFrame->GetSceneWindow();
    CCamera* pCamera = pSceneWnd->GetCamera();
    int keyCode = event.GetKeyCode();
    const CMat4& inverseMat = pCamera->GetViewMatrixInverse();
    float fCameraSpeed = pCamera->m_fMoveSpeed;
    if (event.ShiftDown())
    {
        fCameraSpeed *= pCamera->m_fShiftMoveSpeedRate;
    }
    const CVec3& viewPos = pCamera->GetViewPos();
    CVec3 newPos;
    bool bPressKey = false;
    switch (keyCode)
    {
    case 'w':
    case 'W':
    {
        newPos = inverseMat.GetForwardVec3();
        newPos *= fCameraSpeed;
        newPos = viewPos - newPos;
        bPressKey = true;
    }
    break;
    case 's':
    case 'S':
    {
        newPos = inverseMat.GetForwardVec3();
        newPos *= fCameraSpeed;
        newPos = viewPos + newPos;
        bPressKey = true;
    }
        break;
    case 'a':
    case 'A':
    {
        newPos = inverseMat.GetRightVec3();
        newPos *= fCameraSpeed;
        newPos = viewPos - newPos;
        bPressKey = true;
    }
        break;
    case 'd':
    case 'D':
    {
        newPos = inverseMat.GetRightVec3();
        newPos *= fCameraSpeed;
        newPos += viewPos;
        bPressKey = true;
    }
        break;
    case WXK_F3:
        CRenderManager::GetInstance()->SwitchPolygonMode();
        break;
    default:
        break;
    }
    if (bPressKey)
    {
        SCameraData cameraData = pSceneWnd->GetCamera()->GetCameraData();
        cameraData.m_vec3Pos = newPos;
        pSceneWnd->GetCamera()->SetCameraData(cameraData);
    }
}

void CViewAgentBase::OnTimeBarTreeItemDrag()
{

}

void CViewAgentBase::OnTimeBarTreeItemSelect()
{

}

void CViewAgentBase::OnTimeBarChoice()
{

}

void CViewAgentBase::OnTimeBarItemContainerRClick(wxCommandEvent& /*event*/)
{

}

void CViewAgentBase::OnTimeBarAddButtonClick()
{

}

void CViewAgentBase::OnTimeBarMinusButtonClick()
{

}

void CViewAgentBase::OnTimeBarCurSorChange(int /*nCursorPos*/)
{

}

void CViewAgentBase::OnComponentPropertyChange(CComponentBase* /*pComponent*/)
{

}

void CViewAgentBase::OnComponentStartDrag()
{

}

void CViewAgentBase::OnComponentEndDrag()
{

}

void CViewAgentBase::OnTimeBarSelectFrameChange()
{

}

void CViewAgentBase::OnTimeBarDraggingFrame()
{

}

void CViewAgentBase::OnPropertyGridSelect(wxPropertyGridEvent& event)
{
    bool bSelectParticleEmitter = false;
    CParticleEmitter* pEmitter = nullptr;
    wxPGProperty* pSelectProperty = event.GetProperty();
    CNode* pSelectNode = nullptr;
    if (pSelectProperty != nullptr)
    {
        wxPGProperty* pDetectProperty = pSelectProperty;
        if (pDetectProperty->GetClientData() != nullptr)
        {
            CPropertyDescriptionBase* pPropertyDescription = (CPropertyDescriptionBase*)(pDetectProperty->GetClientData());
            BEATS_ASSERT(pPropertyDescription && pPropertyDescription->GetOwner());
            pSelectNode = dynamic_cast<CNode*>(pPropertyDescription->GetOwner()->GetHostComponent());
        }
        while (pDetectProperty != nullptr)
        {
            CPropertyDescriptionBase* pPropertyBase = (CPropertyDescriptionBase*)(pDetectProperty->GetClientData());
            if (pPropertyBase != nullptr)
            {
                CComponentProxy* pProxy = pPropertyBase->GetOwner();
                if (pProxy->GetGuid() == CParticleEmitter::REFLECT_GUID)
                {
                    bSelectParticleEmitter = true;
                    pEmitter = down_cast<CParticleEmitter*>(pProxy->GetHostComponent());
                    break;
                }
            }
            pDetectProperty = pDetectProperty->GetParent();
        }
        CRenderManager::GetInstance()->GetCoordinateRenderObject()->SetRenderNode(pSelectNode);
        m_pMainFrame->GetParticleControlWnd()->AttachEmitter(pEmitter);
        m_pMainFrame->GetParticleControlWnd()->Show(bSelectParticleEmitter);
    }
    CParticleManager::GetInstance()->m_pCurrSelectEmitter = pEmitter;
}
