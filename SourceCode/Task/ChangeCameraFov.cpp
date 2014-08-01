#include "stdafx.h"
#include "ChangeCameraFov.h"
#include "Scene\SceneManager.h"
#include "Render\Camera.h"
#include "Scene\Scene.h"

CChangeCameraFov::CChangeCameraFov()
    : m_fElapsedTimeMs(0)
    , m_fSourceFov(0)
    , m_fTargetFov(0)
    , m_fSpendTimeMs(0)
{

}

CChangeCameraFov::~CChangeCameraFov()
{
    if (IsInitialized())
    {
        Unload();
        Uninitialize();
    }
}

size_t CChangeCameraFov::GetProgress()
{
    return (size_t)(m_fElapsedTimeMs / m_fSpendTimeMs * 100);
}

void CChangeCameraFov::Execute(float ddt)
{
    m_fElapsedTimeMs += ddt * 1000;
    if (m_fElapsedTimeMs > m_fSpendTimeMs)
    {
        m_fElapsedTimeMs = m_fSpendTimeMs;
    }
    float fCurFov = m_fElapsedTimeMs / m_fSpendTimeMs * (m_fTargetFov - m_fSourceFov) + m_fSourceFov;
    CCamera* pCamera = CSceneManager::GetInstance()->GetCurrentScene()->GetCamera(CCamera::eCT_3D);
    pCamera->SetFOV(fCurFov);
}

void CChangeCameraFov::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY( serializer, m_fSourceFov, true, 0xFFFFFFFF, _T("起始FOV"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fTargetFov, true, 0xFFFFFFFF, _T("最终FOV"), NULL, NULL, NULL );
    DECLARE_PROPERTY( serializer, m_fSpendTimeMs, true, 0xFFFFFFFF, _T("变化时间（毫秒）"), NULL, NULL, NULL );
}
