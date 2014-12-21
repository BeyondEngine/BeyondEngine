#include "stdafx.h"
#include "Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Node3D.h"
#include "Scene/Node2D.h"
#include "Render/RenderManager.h"
#include "EnginePublic/CompWrapper.h"
#include "Event/EventDispatcher.h"
#include "Action/ActionBase.h"
#include "Event/TouchDelegate.h"
#include "Task/TaskManager.h"
#include "external/Configuration.h"
#include "Render/RenderTarget.h"

CScene::CScene()
: m_bActive(false)
, m_fCameraInitNear(0)
, m_fCameraInitFar(0)
, m_fCameraInitFov(0)
, m_fMaxFov_Pad(0)
, m_fMaxFov_Phone(0)
, m_fCameraMinFov(0)
, m_pEnterAction(NULL)
, m_pUpdateAction(NULL)
, m_pLeaveAction(NULL)
, m_pCamera2D(nullptr)
, m_pCamera3D(nullptr)
{
    m_pEventDispatcher = new CCompWrapper<CEventDispatcher, CScene>(this);
}

CScene::~CScene()
{
    BEATS_SAFE_DELETE(m_pCamera2D);
    BEATS_SAFE_DELETE(m_pCamera3D);
    BEATS_SAFE_DELETE(m_pEventDispatcher);
}

void CScene::OnEnter()
{
    BEATS_ASSERT(IsInitialized());
    CSceneManager::GetInstance()->SetRenderSwitcher(true);
    CRenderManager::GetInstance()->GetCurrentRenderTarget()->SetRenderSwitcher(true); // If the screen is froze in SwitchSceneTask, we start it.
}

void CScene::Update(float dtt)
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_Scene3DNodeUpdate);
    for (size_t i = 0; i < m_node3DVector.size(); ++i)
    {
        m_node3DVector[i]->Update(dtt);
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_Scene3DNodeUpdate);

    BEYONDENGINE_PERFORMDETECT_START(ePNT_Scene2DNodeUpdate);
    for (size_t i = 0; i < m_node2DVector.size(); ++i)
    {
        m_node2DVector[i]->Update(dtt);
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_Scene2DNodeUpdate);
    BEYONDENGINE_PERFORMDETECT_START(ePNT_SceneUpdateAction);
    if (m_pUpdateAction != NULL)
    {
        m_pUpdateAction->Execute(NULL);
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_SceneUpdateAction);
}

void CScene::OnLeave()
{
    CTaskManager::GetInstance()->CancelSceneTask(this);
    BEATS_ASSERT(IsInitialized());
    if (m_pLeaveAction != NULL)
    {
        BEATS_ASSERT(m_pLeaveAction->IsInitialized());
        m_pLeaveAction->Execute(NULL);
    }
    if (m_bFreezeScreenWhenLeave)
    {
        CRenderManager::GetInstance()->GetCurrentRenderTarget()->SetRenderSwitcher(false); // Freeze the screen until new scene entered.
    }
}

void CScene::Render()
{
    for (auto iter : m_node3DVector)
    {
        iter->Render();
    }
    for (auto iter : m_node2DVector)
    {
        iter->Render();
    }
}

const TString& CScene::GetDisplayName() const
{
    return m_strDisplayName.GetValueString();
}

bool CScene::Add2DNode(CNode2D* pNode)
{
    bool ret = true;
    if (ret)
    {
        m_node2DVector.push_back(pNode);
        if (IsActive())
        {
            pNode->Activate();
        }
    }
    return ret;
}

bool CScene::Add3DNode(CNode3D* pNode)
{
    bool ret = true;
    if (ret)
    {
        m_node3DVector.push_back(pNode);
        if (IsActive())
        {
            pNode->Activate();
        }
    }
    return ret;
}

std::vector<CNode3D*> CScene::Get3DNode()
{
    return m_node3DVector;
}

void CScene::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strDisplayName, true, 0xFFFFFFFF, _T("场景显示名称"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_cameraInitPos, true, 0xFFFFFFFF, _T("位置"), _T("相机初始参数"), NULL, _T("DefaultValue:100@100@100"));
    DECLARE_PROPERTY(serializer, m_cameraInitRotation, true, 0xFFFFFFFF, _T("旋转"), _T("相机初始参数"), NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fCameraInitNear, true, 0xFFFFFFFF, _T("近裁面"), _T("相机初始参数"), NULL, _T("DefaultValue:0.1"));
    DECLARE_PROPERTY(serializer, m_fCameraInitFar, true, 0xFFFFFFFF, _T("远裁面"), _T("相机初始参数"), NULL, _T("DefaultValue:1000"));
    DECLARE_PROPERTY(serializer, m_fCameraInitFov, true, 0xFFFFFFFF, _T("视野"), _T("相机初始参数"), NULL, _T("DefaultValue:51.2"));
    DECLARE_PROPERTY(serializer, m_fMaxFov_Pad, true, 0xFFFFFFFF, _T("最大Fov(Pad)"), _T("相机"), NULL, _T("DefaultValue:100.0, MinValue:1.0, MaxValue:100.0"));
    DECLARE_PROPERTY(serializer, m_fMaxFov_Phone, true, 0xFFFFFFFF, _T("最大Fov(phone)"), _T("相机"), NULL, _T("DefaultValue:100.0, MinValue:1.0, MaxValue:100.0"));
    DECLARE_PROPERTY(serializer, m_fCameraMinFov, true, 0xFFFFFFFF, _T("最小Fov"), _T("相机"), NULL, _T("DefaultValue:1.0, MinValue: 1.0, MaxValue:100.0"));


    DECLARE_DEPENDENCY(serializer, m_pEnterAction, _T("进入"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pUpdateAction, _T("更新"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pLeaveAction, _T("离开"), eDT_Weak);

    DECLARE_DEPENDENCY_LIST(serializer, m_node2DVector, _T("2D节点"), eDT_Weak);
    DECLARE_DEPENDENCY_LIST(serializer, m_node3DVector, _T("3D节点"), eDT_Weak);
}
#ifdef EDITOR_MODE
bool CScene::OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent)
{
    bool ret = super::OnDependencyListChange(pComponentAddr, action, pComponent);
    if (!ret)
    {
        if (&m_node2DVector == pComponentAddr)
        {
            BEATS_ASSERT(action == eDCA_Delete || dynamic_cast<CNode2D*>(pComponent) != NULL);
            if (action == eDCA_Add)
            {
                Add2DNode(down_cast<CNode2D*>(pComponent));
            }
            else if (action == eDCA_Delete)
            {
                BEATS_ASSERT(pComponent != NULL);
                Remove2DNode(down_cast<CNode2D*>(pComponent));
            }
            ret = true;
        }
        else if (&m_node3DVector == pComponentAddr)
        {
            BEATS_ASSERT(action == eDCA_Delete || dynamic_cast<CNode3D*>(pComponent) != NULL);
            if (action == eDCA_Add)
            {
                Add3DNode(down_cast<CNode3D*>(pComponent));
            }
            else if (action == eDCA_Delete)
            {
                BEATS_ASSERT(pComponent != NULL);
                Remove3DNode(down_cast<CNode3D*>(pComponent));
            }
            ret = true;
        }
    }
    return ret;
}
#endif
void CScene::Initialize()
{
    super::Initialize();
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(GetId() != 0xFFFFFFFF) // Maybe we are copying a scene.
        CSceneManager::GetInstance()->AddScene(this);
#ifdef EDITOR_MODE
        CSceneManager::GetInstance()->SetCurrentScene(this);
#endif
        CSceneManager::GetInstance()->SetRenderSwitcher(false); //We can start render the scene after it is called OnEnter.
    ASSUME_VARIABLE_IN_EDITOR_END
}

void CScene::Uninitialize()
{
    CSceneManager::GetInstance()->RemoveScene(this);
    super::Uninitialize();
}

CEventDispatcher *CScene::EventDispatcher() const
{
    return m_pEventDispatcher;
}

bool CScene::Remove2DNode(CNode2D* pNode)
{
    bool bRet = false;
    if (m_node2DVector.size() > 0)
    {
        std::vector<CNode2D*>::iterator iter = m_node2DVector.begin();
        for (; iter != m_node2DVector.end(); ++iter)
        {
            if ((*iter) == pNode)
            {
                m_node2DVector.erase(iter);
                bRet = true;
                break;
            }
        }
    }
    if (pNode->IsInitialized())
    {
        pNode->Deactivate();
    }
    return bRet;
}

bool CScene::Remove3DNode(CNode3D* pNode)
{
    bool bRet = false;
    if (m_node3DVector.size() > 0)
    {
        std::vector<CNode3D*>::iterator iter = m_node3DVector.begin();
        for (; iter != m_node3DVector.end(); ++iter)
        {
            if ((*iter) == pNode)
            {
                m_node3DVector.erase(iter);
                bRet = true;
                break;
            }
        }
    }
    if (pNode->IsInitialized())
    {
        pNode->Deactivate();
    }
    return bRet;
}

CCamera* CScene::GetCamera(CCamera::ECameraType type)
{
    CCamera*& pOperateCamera = type == CCamera::eCT_2D ? m_pCamera2D : m_pCamera3D;
    if (pOperateCamera == NULL)
    {
        pOperateCamera = new CCamera(type);
        if (type == CCamera::eCT_3D)
        {
            RestoreCamera();
        }
    }
    return pOperateCamera;
}

void CScene::Activate()
{
    if (!m_bActive)
    {
        m_bActive = true;

        for (auto pNode : m_node3DVector)
        {
            pNode->Activate();
        }
        for (auto pNode : m_node2DVector)
        {
            pNode->Activate();
        }
        m_slotTouch[0] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_BEGAN,
            &CScene::OnTouchBegan, this, CEventDispatcher::eHP_HIGHEST);
        m_slotTouch[1] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_MOVED,
            &CScene::OnTouchMove, this, CEventDispatcher::eHP_HIGHEST);
        m_slotTouch[2] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_ENDED,
            &CScene::OnTouchEnd, this, CEventDispatcher::eHP_HIGHEST);
        m_slotTouch[3] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_CANCELLED,
            &CScene::OnTouchEnd, this, CEventDispatcher::eHP_HIGHEST);
        m_slotTouch[4] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_PINCHED,
            &CScene::OnPinched, this, CEventDispatcher::eHP_HIGHEST);
    }
}

void CScene::Deactivate()
{
    if (IsInitialized())
    {
        if (m_bActive)
        {
            // TODO:HACK: if it is not 0xFFFFFFFF, the dependency component in m_node3DVector and m_node2DVector may already be deleted.
            // Tmp fix, may lead some other problem.
            if (GetId() == 0xFFFFFFFF)
            {
                for (auto pNode : m_node3DVector)
                {
                    pNode->Deactivate();
                }
                for (auto pNode : m_node2DVector)
                {
                    pNode->Deactivate();
                }
            }
            m_bActive = false;
            m_slotTouch[0].Disconnect();
            m_slotTouch[1].Disconnect();
            m_slotTouch[2].Disconnect();
            m_slotTouch[3].Disconnect();
            m_slotTouch[4].Disconnect();
        }
    }
}

bool CScene::IsActive() const
{
    return m_bActive;
}

void CScene::SetInitCameraPos(const CVec3& pos)
{
    m_cameraInitPos = pos;
}

void CScene::SetInitCameraRotation(const CVec3& rotation)
{
    m_cameraInitRotation = rotation;
}

void CScene::SetInitCameraNear(float fNear)
{
    m_fCameraInitNear = fNear;
}

void CScene::SetInitCameraFar(float fFar)
{
    m_fCameraInitFar = fFar;
}
void CScene::SetInitCameraFov(float fFov)
{
    m_fCameraInitFov = fFov;
}

const CVec3& CScene::GetInitCameraPos() const
{
    return m_cameraInitPos;
}

const CVec3& CScene::GetInitCameraRotation() const
{
    return m_cameraInitRotation;
}

float CScene::GetInitCameraNear() const
{
    return m_fCameraInitNear;
}

float CScene::GetInitCameraFar() const
{
    return m_fCameraInitFar;
}

float CScene::GetInitCameraFov() const
{
    return m_fCameraInitFov;
}

void CScene::RestoreCamera()
{
    CCamera* p3DCamera = GetCamera(CCamera::eCT_3D);
    p3DCamera->SetViewPos(m_cameraInitPos);
    p3DCamera->SetRotation(m_cameraInitRotation);
    p3DCamera->SetNear(m_fCameraInitNear);
    p3DCamera->SetFar(m_fCameraInitFar);
    p3DCamera->SetFOV(m_fCameraInitFov);
}

void CScene::OnTouchEvent(CBaseEvent* event)
{
    if (!event->Stopped())
    {
        CTouchEvent *pTouchEvent = down_cast<CTouchEvent *>(event);
        bool bHandled = false;
        // Reverse handle event, because the last drawing window will react first.
        for (int i = (int)m_node2DVector.size() - 1; i >= 0; --i)
        {
            bHandled = bHandled || m_node2DVector[i]->OnTouchEvent(pTouchEvent);
        }

        for (auto node : m_node3DVector)
        {
            bHandled = bHandled || node->OnTouchEvent(pTouchEvent);
        }

        if (bHandled)
        {
            event->StopPropagation();
        }
    }
}

void CScene::SetEnterAction(CActionBase* pAction)
{
    m_pEnterAction = pAction;
}

CActionBase* CScene::GetEnterAction() const
{
    return m_pEnterAction;
}

void CScene::SetFreezeScreenWhenLeave(bool bFlag)
{
    m_bFreezeScreenWhenLeave = bFlag;
}

void CScene::OnTouchBegan(CBaseEvent* event)
{
    OnTouchEvent(event);
}

void CScene::OnTouchMove(CBaseEvent* event)
{
    OnTouchEvent(event);
}

void CScene::OnTouchEnd(CBaseEvent* event)
{
    OnTouchEvent(event);
}

void CScene::OnPinched(CBaseEvent* /*event*/)
{
    // Do nothing
}

const std::vector<CNode2D*>& CScene::GetNode2DVector() const
{
    return m_node2DVector;
}

float CScene::GetMaxCameraFov() const
{
    bool bIsPad = CConfiguration::GetInstance()->GetDeviceType() != eDT_Phone;
    return bIsPad ? m_fMaxFov_Pad : m_fMaxFov_Phone;
}

void CScene::SetMaxCameraFov(float fFov)
{
    bool bIsPad = CConfiguration::GetInstance()->GetDeviceType() != eDT_Phone;
    if (bIsPad)
    {
        m_fMaxFov_Pad = fFov;
    }
    else
    {
        m_fMaxFov_Phone = fFov;
    }
}

float CScene::GetMinCameraFov() const
{
    return m_fCameraMinFov;
}

void CScene::SetMinCameraFov(float fFov)
{
    m_fCameraMinFov = fFov;
}

void CScene::ExecuteEnterTask()
{
    if (m_pEnterAction != NULL)
    {
        BEATS_ASSERT(m_pEnterAction->IsInitialized());
        m_pEnterAction->Execute(NULL);
    }
}
