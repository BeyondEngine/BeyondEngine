#ifndef BEYOND_ENGINE_SCENE_SCENE_H_INCLUDE
#define BEYOND_ENGINE_SCENE_SCENE_H_INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "Render/Camera.h"
#include "Event/EventSlot.h"

class CNode;
class CNode2D;
class CNode3D;
class CEventDispatcher;
class CActionBase;
class CBaseEvent;

class CScene : public CComponentInstance
{
    DECLARE_REFLECT_GUID( CScene, 0x1458a522, CComponentInstance )
public:
    CScene();
    virtual ~CScene();

    virtual void ReflectData( CSerializer& serializer ) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
    virtual bool OnDependencyListChange(void* pComponentAddr, enum EDependencyChangeAction action, CComponentBase* pComponent) override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual void OnEnter();
    virtual void Update( float dtt );
    virtual void OnLeave();

    CEventDispatcher *EventDispatcher() const;
    void SetName(const TCHAR* pszName);
    const TString& GetName() const;
    bool Add2DNode( CNode2D* pNode );
    bool Add3DNode( CNode3D* pNode );
    bool Remove2DNode( CNode2D* pNode );
    bool Remove3DNode( CNode3D* pNode );
    const std::vector<CNode2D*>& GetNode2DVector() const;

    virtual void Render();
    CCamera* GetCamera( CCamera::ECameraType type );
    void Activate();
    void Deactivate();
    bool IsActive() const;

    void SetInitCameraPos(float x, float y, float z);
    void SetInitCameraRotation(float x, float y, float z);
    void SetInitCameraNear(float fNear);
    void SetInitCameraFar(float fFar);
    void SetInitCameraFov(float fFov);
    const CVec3& GetInitCameraPos() const;
    const CVec3& GetInitCameraRotation() const;
    float GetInitCameraNear() const;
    float GetInitCameraFar() const;
    float GetInitCameraFov() const;

    void RestoreCamera();
    void OnTouchEvent(CBaseEvent *event);

private:
    bool m_bActive;
    float m_fCameraInitNear;
    float m_fCameraInitFar;
    float m_fCameraInitFov;
    std::vector<CNode2D*> m_node2DVector;
    std::vector<CNode3D*> m_node3DVector;
    TString m_strName;

    CCamera* m_pCamera2D;
    CCamera* m_pCamera3D;
    CVec3 m_cameraInitPos;
    CVec3 m_cameraInitRotation;

    CActionBase* m_pEnterAction;
    CActionBase* m_pUpdateAction;
    CActionBase* m_pLeaveAction;
    CEventDispatcher *m_pEventDispatcher;
    CEventSlot m_slotTouch[6];
};

#endif