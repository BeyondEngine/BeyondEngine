#ifndef BEYOND_ENGINE_SCENE_SCENE_H__INCLUDE
#define BEYOND_ENGINE_SCENE_SCENE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "Render/Camera.h"
#include "Event/EventSlot.h"
#include "Language/LanguageText.h"

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
#ifdef EDITOR_MODE
    virtual bool OnDependencyListChange(void* pComponentAddr, enum EDependencyChangeAction action, CComponentBase* pComponent) override;
#endif
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    virtual void OnEnter();
    virtual void Update( float dtt );
    virtual void OnLeave();
    virtual void ExecuteEnterTask();

    CEventDispatcher *EventDispatcher() const;
    const TString& GetDisplayName() const;
    bool Add2DNode( CNode2D* pNode );
    bool Add3DNode( CNode3D* pNode );
    std::vector<CNode3D*> Get3DNode();
    bool Remove2DNode( CNode2D* pNode );
    bool Remove3DNode( CNode3D* pNode );
    const std::vector<CNode2D*>& GetNode2DVector() const;

    virtual void Render();
    CCamera* GetCamera( CCamera::ECameraType type );
    virtual void Activate();
    virtual void Deactivate();
    bool IsActive() const;

    void SetInitCameraPos(const CVec3& pos);
    void SetInitCameraRotation(const CVec3& rotation);
    void SetInitCameraNear(float fNear);
    void SetInitCameraFar(float fFar);
    void SetInitCameraFov(float fFov);
    const CVec3& GetInitCameraPos() const;
    const CVec3& GetInitCameraRotation() const;
    float GetInitCameraNear() const;
    float GetInitCameraFar() const;
    float GetInitCameraFov() const;
    float GetMaxCameraFov() const;
    void SetMaxCameraFov(float fFov);
    float GetMinCameraFov() const;
    void SetMinCameraFov(float fFov);

    void RestoreCamera();
    void OnTouchEvent(CBaseEvent *event);
    void SetEnterAction(CActionBase* pAction);
    CActionBase* GetEnterAction() const;
    void SetFreezeScreenWhenLeave(bool bFlag);

protected:
    virtual void OnTouchBegan(CBaseEvent *event);
    virtual void OnTouchMove(CBaseEvent *event);
    virtual void OnTouchEnd(CBaseEvent* event);
    virtual void OnPinched(CBaseEvent *event);

private:
    bool m_bActive;
    bool m_bFreezeScreenWhenLeave = false;
    float m_fMaxFov_Pad;
    float m_fMaxFov_Phone;
    float m_fCameraMinFov;
    float m_fCameraInitNear;
    float m_fCameraInitFar;
    float m_fCameraInitFov;
    std::vector<CNode2D*> m_node2DVector;
    std::vector<CNode3D*> m_node3DVector;
    CLanguageText m_strDisplayName;

    CCamera* m_pCamera2D;
    CCamera* m_pCamera3D;
    CVec3 m_cameraInitPos;
    CVec3 m_cameraInitRotation;

    CActionBase* m_pEnterAction;
    CActionBase* m_pUpdateAction;
    CActionBase* m_pLeaveAction;
    CEventDispatcher *m_pEventDispatcher;
    CEventSlot m_slotTouch[5];
};

#endif