#include "stdafx.h"
#include "Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Node3D.h"
#include "Scene/Node2D.h"
#include "Render/Camera.h"
#include "Render/RenderManager.h"
#include "CompWrapper.h"
#include "Event/EventDispatcher.h"
#include "Action/ActionBase.h"
#include "GUI/WindowManager.h"
#include "GUI/Window/Window.h"
#include "Event/TouchDelegate.h"

CScene::CScene()
    : m_bActive(false)
    , m_fCameraInitNear(0)
    , m_fCameraInitFar(0)
    , m_fCameraInitFov(0)
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
    BEATS_SAFE_DELETE( m_pCamera2D );
    BEATS_SAFE_DELETE( m_pCamera3D );
    BEATS_SAFE_DELETE(m_pEventDispatcher);
}

void CScene::SetName(const TCHAR* pszName)
{
    BEATS_ASSERT(pszName != NULL);
    m_strName.assign(pszName);
}

const TString& CScene::GetName() const
{
    return m_strName;
}

void CScene::OnEnter()
{
    if (m_pEnterAction != NULL)
    {
        m_pEnterAction->Execute(NULL);
    }
}

void CScene::Update( float dtt )
{
    //render nodes
    BEYONDENGINE_PERFORMDETECT_START(ePNT_Scene3DNodeUpdate);
    for ( auto iter : m_node3DVector )
    {
        iter->Update( dtt );
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_Scene3DNodeUpdate);

    BEYONDENGINE_PERFORMDETECT_START(ePNT_Scene2DNodeUpdate);
    for (auto iter : m_node2DVector)
    {
        iter->Update(dtt);
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_Scene2DNodeUpdate);
    if (m_pUpdateAction != NULL)
    {
        m_pUpdateAction->Execute(NULL);
    }
}

void CScene::OnLeave()
{
    if (m_pLeaveAction != NULL)
    {
        m_pLeaveAction->Execute(NULL);
    }
}

void CScene::Render()
{
    for ( auto iter : m_node3DVector )
    {
        iter->Render();
    }
    for ( auto iter : m_node2DVector )
    {
        iter->Render();
    }
}

bool CScene::Add2DNode( CNode2D* pNode )
{
    bool ret = true;
    if (ret)
    {
        m_node2DVector.push_back( pNode );
        if(IsActive())
        {
            pNode->Activate();
        }
    }
    return ret;
}

bool CScene::Add3DNode( CNode3D* pNode )
{
    bool ret = true;
    if (ret)
    {
        m_node3DVector.push_back( pNode );
        if(IsActive())
        {
            pNode->Activate();
        }
    }
    return ret;
}

void CScene::ReflectData( CSerializer& serializer )
{
    super::ReflectData( serializer );
    DECLARE_PROPERTY( serializer, m_strName, true, 0xFFFFFFFF, _T("场景名字"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_cameraInitPos, true, 0xFFFFFFFF, _T("位置"), _T("相机初始参数"), NULL, _T("DefaultValue:100@100@100") );
    DECLARE_PROPERTY( serializer, m_cameraInitRotation, true, 0xFFFFFFFF, _T("旋转"), _T("相机初始参数"), NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fCameraInitNear, true, 0xFFFFFFFF, _T("近裁面"), _T("相机初始参数"), NULL, _T("DefaultValue:0.1") );
    DECLARE_PROPERTY( serializer, m_fCameraInitFar, true, 0xFFFFFFFF, _T("远裁面"), _T("相机初始参数"), NULL, _T("DefaultValue:1000") );
    DECLARE_PROPERTY( serializer, m_fCameraInitFov, true, 0xFFFFFFFF, _T("视野"), _T("相机初始参数"), NULL, _T("DefaultValue:51.2") );

    DECLARE_DEPENDENCY( serializer, m_pEnterAction, _T("进入"), eDT_Weak );
    DECLARE_DEPENDENCY( serializer, m_pUpdateAction, _T("更新"), eDT_Weak );
    DECLARE_DEPENDENCY( serializer, m_pLeaveAction, _T("离开"), eDT_Weak );

    DECLARE_DEPENDENCY_LIST( serializer, m_node2DVector, _T("2D节点"), eDT_Weak );
    DECLARE_DEPENDENCY_LIST( serializer, m_node3DVector, _T("3D节点"), eDT_Weak );
}

bool CScene::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool ret = super::OnPropertyChange( pVariableAddr, pSerializer );
    if ( !ret )
    {
        if ( &m_strName == pVariableAddr )
        {
            DeserializeVariable( m_strName, pSerializer );
            CSceneManager::GetInstance()->ChangeAddSceneName( this );
            ret = true;
        }
    }
    return ret;
}

bool CScene::OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent)
{
    bool ret = super::OnDependencyListChange( pComponentAddr, action, pComponent );
    if (!ret)
    {
        if ( &m_node2DVector == pComponentAddr )
        {
            BEATS_ASSERT(action == eDCA_Delete || dynamic_cast<CNode2D*>(pComponent) != NULL);
            if (action == eDCA_Add)
            {
                Add2DNode( (CNode2D*)pComponent );
            }
            else if ( action == eDCA_Change || action == eDCA_Delete )
            {
                BEATS_ASSERT(pComponent != NULL);
                Remove2DNode( (CNode2D*)pComponent );
            }
            ret = true;
        }
        else if ( &m_node3DVector == pComponentAddr )
        {
            BEATS_ASSERT(action == eDCA_Delete || dynamic_cast<CNode3D*>(pComponent) != NULL);
            if (action == eDCA_Add)
            {
                Add3DNode( (CNode3D*)pComponent );
            }
            else if ( action == eDCA_Change || action == eDCA_Delete )
            {
                BEATS_ASSERT(pComponent != NULL);
                Remove3DNode( (CNode3D*)pComponent );
            }
            ret = true;
        }
    }
    return ret;
}

void CScene::Initialize()
{
    super::Initialize();
    CSceneManager::GetInstance()->AddScene( this );
    CSceneManager::GetInstance()->SetCurrentScene( this );

    m_slotTouch[0] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_BEGAN,
        &CScene::OnTouchEvent, this, CEventDispatcher::eHP_HIGHEST);
    m_slotTouch[1] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_MOVED,
        &CScene::OnTouchEvent, this, CEventDispatcher::eHP_HIGHEST);
    m_slotTouch[2] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_ENDED,
        &CScene::OnTouchEvent, this, CEventDispatcher::eHP_HIGHEST);
    m_slotTouch[3] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_CANCELLED,
        &CScene::OnTouchEvent, this, CEventDispatcher::eHP_HIGHEST);
    m_slotTouch[4] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_DOUBLE_TAPPED,
        &CScene::OnTouchEvent, this, CEventDispatcher::eHP_HIGHEST);
    m_slotTouch[5] = CTouchDelegate::GetInstance()->SubscribeEvent(eET_EVENT_TOUCH_SINGLE_TAPPED,
        &CScene::OnTouchEvent, this, CEventDispatcher::eHP_HIGHEST);

}


void CScene::Uninitialize()
{
    super::Uninitialize();
    CSceneManager::GetInstance()->RemoveScene( this );
}

CEventDispatcher *CScene::EventDispatcher() const
{
    return m_pEventDispatcher;
}

bool CScene::Remove2DNode( CNode2D* pNode )
{
    bool bRet = false;
    if ( m_node2DVector.size() > 0 )
    {
        std::vector<CNode2D*>::iterator iter = m_node2DVector.begin();
        for ( ; iter != m_node2DVector.end();++iter )
        {
            if ( (*iter) == pNode )
            {
                m_node2DVector.erase( iter );
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

bool CScene::Remove3DNode( CNode3D* pNode )
{
    bool bRet = false;
    if ( m_node3DVector.size() > 0 )
    {
        std::vector<CNode3D*>::iterator iter = m_node3DVector.begin();
        for ( ; iter != m_node3DVector.end();++iter )
        {
            if ( (*iter) == pNode )
            {
                m_node3DVector.erase( iter );
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

CCamera* CScene::GetCamera( CCamera::ECameraType type )
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
    if(!m_bActive)
    {
        m_bActive = true;

        for(auto pNode : m_node3DVector)
        {
            pNode->Activate();
        }
        for(auto pNode : m_node2DVector)
        {
            pNode->Activate();
        }

    }
}

void CScene::Deactivate()
{
    if(m_bActive)
    {
        for(auto pNode : m_node3DVector)
        {
            pNode->Deactivate();
        }
        for(auto pNode : m_node2DVector)
        {
            pNode->Deactivate();
        }
        m_bActive = false;
    }
}

bool CScene::IsActive() const
{
    return m_bActive;
}

void CScene::SetInitCameraPos(float x, float y, float z)
{
    kmVec3Fill(&m_cameraInitPos, x, y, z);
    UPDATE_PROPERTY_PROXY(m_cameraInitPos);
}

void CScene::SetInitCameraRotation(float x, float y, float z)
{
    kmVec3Fill(&m_cameraInitRotation, x, y, z);
    UPDATE_PROPERTY_PROXY(m_cameraInitRotation);
}

void CScene::SetInitCameraNear(float fNear)
{
    m_fCameraInitNear = fNear;
    UPDATE_PROPERTY_PROXY(m_fCameraInitNear);
}

void CScene::SetInitCameraFar(float fFar)
{
    m_fCameraInitFar = fFar;
    UPDATE_PROPERTY_PROXY(m_fCameraInitFar);
}
void CScene::SetInitCameraFov(float fFov)
{
    m_fCameraInitFov = fFov;
    UPDATE_PROPERTY_PROXY(m_fCameraInitFov);
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
    p3DCamera->SetViewPos(m_cameraInitPos.x, m_cameraInitPos.y, m_cameraInitPos.z);
    p3DCamera->SetRotation(m_cameraInitRotation.x, m_cameraInitRotation.y, m_cameraInitRotation.z);
    p3DCamera->SetNear(m_fCameraInitNear);
    p3DCamera->SetFar(m_fCameraInitFar);
    p3DCamera->SetFOV(m_fCameraInitFov);
}

void CScene::OnTouchEvent(CBaseEvent* event)
{
    if ( !event->Stopped() )
    {
        CTouchEvent *pTouchEvent = down_cast<CTouchEvent *>(event);
        bool bUIHandled = false;
        // Reverse handle event, because the last drawing window will react first.
        for (int i = m_node2DVector.size() - 1; i >=0; --i)
        {
            bUIHandled = bUIHandled || m_node2DVector[i]->OnTouchEvent(pTouchEvent);
        }
        if (bUIHandled)
        {
            event->StopPropagation();
        }
    }
}

const std::vector<CNode2D*>& CScene::GetNode2DVector() const
{
    return m_node2DVector;
}
