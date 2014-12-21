#include "stdafx.h"
#include "Camera.h"
#include "RenderManager.h"
#include "RenderTarget.h"

CCamera::CCamera(ECameraType type /* = eCT_3D */)
    : m_bInvalidateViewMatrix(true)
    , m_bInvalidateProjectionMatrix(true)
    , m_bInvalidFrustumPlanes(true)
    , m_bLocked(false)
    , m_type(type)
    , m_fRotateSpeed(12.5f)
{
    m_viewMatrix.Identity();
    m_projectionMatrix.Identity();
    if (m_type == eCT_2D)
    {
        m_cameraData.m_vec3Pos = CVec3(0, 0, 5);// Set camera to z = 5, to make sure we can see things at z = 0.
    }
}

CCamera::~CCamera()
{
}

CCamera::ECameraType CCamera::GetType() const
{
    return m_type;
}

void  CCamera::Roll(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        if (!m_bLocked)
        {
            m_cameraData.m_vec3Rotation.Z() += angle * m_fRotateSpeed;
            InvalidateViewMatrix();
        }
    }
}
void  CCamera::Pitch(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        if (!m_bLocked)
        {
            m_cameraData.m_vec3Rotation.X() += angle * m_fRotateSpeed;
            InvalidateViewMatrix();
        }
    }
}

void  CCamera::Yaw(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        if (!m_bLocked)
        {
            m_cameraData.m_vec3Rotation.Y() += angle * m_fRotateSpeed;
            InvalidateViewMatrix();
        }
    }
}

float CCamera::GetPitch() const
{
    return m_cameraData.m_vec3Rotation.X();
}

float CCamera::GetYaw() const
{
    return m_cameraData.m_vec3Rotation.Y();
}

float CCamera::GetRoll() const
{
    return m_cameraData.m_vec3Rotation.Z();
}

void CCamera::SetNear(float fZNear)
{
    if (!BEATS_FLOAT_EQUAL(m_cameraData.m_fZNear, fZNear))
    {
        if (!m_bLocked)
        {
            m_cameraData.m_fZNear = fZNear;
            InvalidateProjectionMatrix();
        }
    }
}

void CCamera::SetFar(float fZFar)
{
    if (!BEATS_FLOAT_EQUAL(m_cameraData.m_fZFar, fZFar))
    {
        if (!m_bLocked)
        {
            m_cameraData.m_fZFar = fZFar;
            InvalidateProjectionMatrix();
        }
    }
}

void CCamera::SetFOV(float fFOV)
{
    if (!BEATS_FLOAT_EQUAL(m_cameraData.m_fFOV, fFOV))
    {
        if (!m_bLocked)
        {
            m_cameraData.m_fFOV = fFOV;
            InvalidateProjectionMatrix();
        }
    }
}

float CCamera::GetNear() const
{
    return m_cameraData.m_fZNear;
}

float CCamera::GetFar() const
{
    return m_cameraData.m_fZFar;
}

float CCamera::GetFOV() const
{
    return m_cameraData.m_fFOV;
}

void CCamera::SetViewPos(const CVec3& viewPos)
{
    if (!m_bLocked)
    {
        float fOldZValue = m_cameraData.m_vec3Pos.Z();
        m_cameraData.m_vec3Pos = viewPos;
        if (m_type == eCT_2D)
        {
            m_cameraData.m_vec3Pos.Z() = fOldZValue;
        }
        InvalidateViewMatrix();
    }
}

const CVec3& CCamera::GetViewPos() const
{
    return m_cameraData.m_vec3Pos;
}

void CCamera::SetRotation(const CVec3& rotation)
{
    if (!m_bLocked)
    {
        m_cameraData.m_vec3Rotation = rotation;
        InvalidateViewMatrix();
    }
}

const CVec3& CCamera::GetRotation() const
{
    return m_cameraData.m_vec3Rotation;
}

const CMat4& CCamera::GetViewMatrix(bool bWithShake/*= false*/)
{
    if (m_bInvalidateViewMatrix)
    {
        m_viewMatrix.Identity();
        switch(m_type)
        {
        case eCT_2D:
            m_viewMatrix.SetTranslate(m_cameraData.m_vec3Pos * -1); //Get the inverse matrix.
            break;
        case eCT_3D:
            {
                CVec3 eye = m_cameraData.m_vec3Pos;
                CVec3 center(0, 0, -100); //Opengl face to the negative Z as default.
                CMat4 rotateMatrix;
                rotateMatrix.FromPitchYawRoll(DegreesToRadians(m_cameraData.m_vec3Rotation.X()), DegreesToRadians(m_cameraData.m_vec3Rotation.Y()), DegreesToRadians(m_cameraData.m_vec3Rotation.Z()));
                CVec3 up;
                up = rotateMatrix.GetUpVec3();
                center *= rotateMatrix;
                center = center + eye;
                m_viewMatrix.LookAt(eye, center, up);
                if (m_cameraData.m_vec3ShakeOffset.LengthSq() != 0)
                {
                    eye = m_cameraData.m_vec3Pos;
                    eye += m_cameraData.m_vec3ShakeOffset;
                    center.Fill(0, 0, -100); //Opengl face to the negative Z as default.
                    rotateMatrix.FromPitchYawRoll(DegreesToRadians(m_cameraData.m_vec3Rotation.X()), DegreesToRadians(m_cameraData.m_vec3Rotation.Y()), DegreesToRadians(m_cameraData.m_vec3Rotation.Z()));
                    up = rotateMatrix.GetUpVec3();
                    center *= rotateMatrix;
                    center = center + eye;
                    m_viewMatrixWithShake.LookAt(eye, center, up);
                }
                else
                {
                    m_viewMatrixWithShake = m_viewMatrix;
                }
            }
            break;
        default:
            BEATS_ASSERT(false);
            break;
        }
        m_viewMatrixInverse = m_viewMatrix;
        m_viewMatrixInverse.Inverse();
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_viewProjectionMatrixWithShake = m_projectionMatrix * m_viewMatrixWithShake;
        m_bInvalidateViewMatrix = false;
    }
    bool b3DShakeMatrix = GetType() == CCamera::eCT_3D && bWithShake;
    return b3DShakeMatrix ? m_viewMatrixWithShake : m_viewMatrix;
}

const CMat4& CCamera::GetViewMatrixInverse()
{
    if (m_bInvalidateViewMatrix)
    {
        GetViewMatrix(); //Force update the m_viewMatrixInverse.
    }
    return m_viewMatrixInverse;
}

const CMat4& CCamera::GetViewProjectionMatrix(bool bWithShake/* = false*/)
{
    if (m_bInvalidateViewMatrix)
    {
        GetViewMatrix(); //Force update the m_viewMatrixInverse.
    }
    if (m_bInvalidateProjectionMatrix)
    {
        GetProjectionMatrix(); //Force update the m_projectionMatrixInverse.
    }
    bool b3DShakeMatrix = GetType() == CCamera::eCT_3D && bWithShake;
    return  b3DShakeMatrix ? m_viewProjectionMatrixWithShake : m_viewProjectionMatrix;
}

const CMat4& CCamera::GetProjectionMatrix()
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    CRenderTarget* pRenderTarget = pRenderMgr->GetCurrentRenderTarget();
    uint32_t uWidth = pRenderTarget->GetDeviceWidth();
    uint32_t uHeight = pRenderTarget->GetDeviceHeight();
    if (m_cameraData.m_uTargetWidth != uWidth || m_cameraData.m_uTargetHeight != uHeight)
    {
        m_cameraData.m_uTargetWidth = uWidth;
        m_cameraData.m_uTargetHeight = uHeight;
        m_bInvalidateProjectionMatrix = true;
    }
    if (m_bInvalidateProjectionMatrix)
    {
        switch(m_type)
        {
        case eCT_2D:
            m_projectionMatrix.OrthographicProjection(
                m_cameraData.m_vec2CameraCenterOffset.X(),
                m_cameraData.m_uTargetWidth + m_cameraData.m_vec2CameraCenterOffset.X(),
                m_cameraData.m_uTargetHeight + m_cameraData.m_vec2CameraCenterOffset.Y(),
                m_cameraData.m_vec2CameraCenterOffset.Y(),
                m_cameraData.m_fZNear, m_cameraData.m_fZFar);
            break;
        case eCT_3D:
            m_projectionMatrix.PerspectiveProjection(m_cameraData.m_fFOV, (float)m_cameraData.m_uTargetWidth / m_cameraData.m_uTargetHeight, m_cameraData.m_fZNear, m_cameraData.m_fZFar);
            break;
        }
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_viewProjectionMatrixWithShake = m_projectionMatrix * m_viewMatrixWithShake;
        m_bInvalidateProjectionMatrix = false;
    }
    return m_projectionMatrix;
}

void CCamera::SetCenterOffset(const CVec2& centerOffset)
{
    m_cameraData.m_vec2CameraCenterOffset.X() = centerOffset.X();
    m_cameraData.m_vec2CameraCenterOffset.Y() = centerOffset.Y();
    InvalidateProjectionMatrix();
}

const CVec2& CCamera::GetCenterOffset() const
{
    return m_cameraData.m_vec2CameraCenterOffset;
}

CVec3 CCamera::RayCast(float x, float y) const
{
    CRay3 ray = GetRayFromScreenPos(x, y);
    CPlane plane(0, 1, 0, 0);
    float fIntersectDistance = 0;
    bool bRet = ray.IntersectPlane(plane, fIntersectDistance);
    CVec3 ret = ray.GetStartPos() + ray.GetDirection() * fIntersectDistance;
    BEYONDENGINE_UNUSED_PARAM(bRet);
    BEATS_ASSERT(bRet);
    return ret;
}

CRay3 CCamera::GetRayFromScreenPos(float x, float y) const
{
    const CMat4& viewMatrixInverse = const_cast<CCamera*>(this)->GetViewMatrixInverse();
    CVec3 rightDirection = viewMatrixInverse.GetRightVec3();
    CVec3 upDirection = viewMatrixInverse.GetUpVec3();
    CVec3 forwardDirection = viewMatrixInverse.GetForwardVec3();

    float fFOVRadian = DegreesToRadians(m_cameraData.m_fFOV);
    float halfHeight = (float)(tanf(fFOVRadian * 0.5f) * m_cameraData.m_fZNear);
    CRenderTarget* pCurRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    uint32_t uWidth = pCurRenderTarget->GetLogicWidth();
    uint32_t uHeight = pCurRenderTarget->GetLogicHeight();
    float fHalfWidth = halfHeight * uWidth / uHeight;
    forwardDirection = forwardDirection * m_cameraData.m_fZNear;
    rightDirection = rightDirection * fHalfWidth;
    upDirection = upDirection * halfHeight;

    x -= uWidth * 0.5f;
    y -= uHeight * 0.5f;
    // normalize to 1
    x = ((float)x) / (uWidth * 0.5f);
    y = ((float)y) / (uHeight * 0.5f);
    y *= -1;
    rightDirection = rightDirection * x;
    upDirection = upDirection * y;

    CVec3 posOnNearPlane;
    posOnNearPlane = m_cameraData.m_vec3Pos - forwardDirection;
    posOnNearPlane = posOnNearPlane + rightDirection;
    posOnNearPlane = posOnNearPlane + upDirection;

    CVec3 rayDirection;
    rayDirection = posOnNearPlane - m_cameraData.m_vec3Pos;
    CRay3 ret;
    ret.FromPointAndDirection(m_cameraData.m_vec3Pos, rayDirection);
    return ret;
}

CVec3 CCamera::QueryCameraPos(const CVec3& worldPos, const CVec2& screenPos, const CMat4& viewMatrix,
    float fNear, float fFov, float fCameraHeight)
{
    CMat4 cleanViewMatrix = viewMatrix;
    cleanViewMatrix.RemoveTranslate();
    CMat4 viewInverseMat = cleanViewMatrix;
    viewInverseMat.Inverse();

    CVec2 screenPosInClamp = screenPos;
    float fWidth = (float)CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetLogicWidth();
    float fHeight = (float)CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetLogicHeight();
    screenPosInClamp.Y() = fHeight - screenPosInClamp.Y();
    screenPosInClamp.X() -= fWidth / 2;
    screenPosInClamp.Y() -= fHeight / 2;
    screenPosInClamp.X() /= fWidth / 2;
    screenPosInClamp.Y() /= fHeight / 2;
    float halfHeight = (float)(tanf(DegreesToRadians(fFov * 0.5f)) * fNear);
    float halfWidth = halfHeight * fWidth / fHeight;
    CVec3 tempDir(screenPosInClamp.X() * halfWidth, screenPosInClamp.Y() * halfHeight, -fNear);
    tempDir.Normalize();
    tempDir *= viewInverseMat;
    float fCameraViewDistance = fCameraHeight / tempDir.Dot(CVec3(0, -1, 0));
    CVec3 ret;
    ret.X() = worldPos.X() - tempDir.X() * fCameraViewDistance;
    ret.Y() = fCameraHeight;
    ret.Z() = worldPos.Z() - tempDir.Z() * fCameraViewDistance;
    return ret;
}

CVec2 CCamera::WorldToScreen(const CVec3& worldPos, bool bWithShake/* = false*/)
{
    CVec2 screenPos;
    BEATS_ASSERT( GetType() == eCT_3D );
    CVec4 pos(worldPos, 1.0f);
    const CMat4& vpMat = GetViewProjectionMatrix(bWithShake);
    pos *= vpMat;
    BEATS_ASSERT(!BEATS_FLOAT_EQUAL(pos.W(), 0));
    screenPos.X() = (pos.X() / pos.W() + 1.0f) * 0.5f;
    screenPos.Y() = (pos.Y() / pos.W() + 1.0f) * 0.5f;
    CRenderTarget* pCurrRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pCurrRenderTarget != NULL);
    screenPos.X() = pCurrRenderTarget->GetLogicWidth() * screenPos.X();
    screenPos.Y() = pCurrRenderTarget->GetLogicHeight() * (1 - screenPos.Y());
    return screenPos;
}

void CCamera::InvalidateViewMatrix()
{
    m_bInvalidateViewMatrix = true;
    m_bInvalidFrustumPlanes = true;
#ifdef EDITOR_MODE
    m_uLastUpdateFrameCounter = CEngineCenter::GetInstance()->GetFrameCounter();
#endif
}

void CCamera::InvalidateProjectionMatrix()
{
    m_bInvalidateProjectionMatrix = true;
    m_bInvalidFrustumPlanes = true;
#ifdef EDITOR_MODE
    m_uLastUpdateFrameCounter = CEngineCenter::GetInstance()->GetFrameCounter();
#endif
}

void CCamera::SetShakeOffset(float fOffsetX, float fOffsetY, float fOffsetZ)
{
    m_cameraData.m_vec3ShakeOffset.Fill(fOffsetX, fOffsetY, fOffsetZ);
    InvalidateViewMatrix();
}

const CVec3& CCamera::GetShakeOffset() const
{
    return m_cameraData.m_vec3ShakeOffset;
}

void CCamera::SetCameraData(const SCameraData& cameraData)
{
    m_cameraData = cameraData;
    InvalidateViewMatrix();
    InvalidateProjectionMatrix();
}

const SCameraData& CCamera::GetCameraData() const
{
    return m_cameraData;
}

const SFrustumPlanes& CCamera::GetFrustumPlanes()
{
    if (m_bInvalidFrustumPlanes)
    {
        CMat4 vpMatrix = GetProjectionMatrix() * GetViewMatrix();

        CVec3 vecNormal;
        vecNormal.X() = vpMatrix[3] + vpMatrix[0];
        vecNormal.Y() = vpMatrix[7] + vpMatrix[4];
        vecNormal.Z() = vpMatrix[11] + vpMatrix[8];
        float fDistance = (vpMatrix[15] + vpMatrix[12]) / vecNormal.Length();
        vecNormal.Normalize();
        m_frustumPlanes.m_planes[(uint32_t)EFrustumPlaneType::eFPT_Left].FromPointNormalAndDistance(vecNormal, fDistance);

        vecNormal.X() = vpMatrix[3] - vpMatrix[0];
        vecNormal.Y() = vpMatrix[7] - vpMatrix[4];
        vecNormal.Z() = vpMatrix[11] - vpMatrix[8];
        fDistance = (vpMatrix[15] - vpMatrix[12]) / vecNormal.Length();
        vecNormal.Normalize();
        m_frustumPlanes.m_planes[(uint32_t)EFrustumPlaneType::eFPT_Right].FromPointNormalAndDistance(vecNormal, fDistance);

        vecNormal.X() = vpMatrix[3] - vpMatrix[1];
        vecNormal.Y() = vpMatrix[7] - vpMatrix[5];
        vecNormal.Z() = vpMatrix[11] - vpMatrix[9];
        fDistance = (vpMatrix[15] - vpMatrix[13]) / vecNormal.Length();
        vecNormal.Normalize();
        m_frustumPlanes.m_planes[(uint32_t)EFrustumPlaneType::eFPT_Top].FromPointNormalAndDistance(vecNormal, fDistance);

        vecNormal.X() = vpMatrix[3] + vpMatrix[1];
        vecNormal.Y() = vpMatrix[7] + vpMatrix[5];
        vecNormal.Z() = vpMatrix[11] + vpMatrix[9];
        fDistance = (vpMatrix[15] + vpMatrix[13]) / vecNormal.Length();
        vecNormal.Normalize();
        m_frustumPlanes.m_planes[(uint32_t)EFrustumPlaneType::eFPT_Bottom].FromPointNormalAndDistance(vecNormal, fDistance);

        vecNormal.X() = vpMatrix[3] + vpMatrix[2];
        vecNormal.Y() = vpMatrix[7] + vpMatrix[6];
        vecNormal.Z() = vpMatrix[11] + vpMatrix[10];
        fDistance = (vpMatrix[15] + vpMatrix[14]) / vecNormal.Length();
        vecNormal.Normalize();
        m_frustumPlanes.m_planes[(uint32_t)EFrustumPlaneType::eFPT_Near].FromPointNormalAndDistance(vecNormal, fDistance);

        vecNormal.X() = vpMatrix[3] - vpMatrix[2];
        vecNormal.Y() = vpMatrix[7] - vpMatrix[6];
        vecNormal.Z() = vpMatrix[11] - vpMatrix[10];
        fDistance = (vpMatrix[15] - vpMatrix[14]) / vecNormal.Length();
        vecNormal.Normalize();
        m_frustumPlanes.m_planes[(uint32_t)EFrustumPlaneType::eFPT_Far].FromPointNormalAndDistance(vecNormal, fDistance);
        m_bInvalidFrustumPlanes = false;
    }
    return m_frustumPlanes;
}

void CCamera::Lock()
{
    m_bLocked = true;
}

void CCamera::Unlock()
{
    m_bLocked = false;
}
