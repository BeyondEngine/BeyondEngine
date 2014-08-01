#include "stdafx.h"
#include "Camera.h"
#include "RenderManager.h"
#include "Renderer.h"
#include "RenderState.h"
#include "Framework/Application.h"
#include "RenderTarget.h"

CCamera::CCamera(ECameraType type /* = eCT_3D */)
    : m_bInvalidateViewMatrix(true)
    , m_bInvalidateProjectionMatrix(true)
    , m_uWidth(0)
    , m_uHeight(0)
    , m_type(type)
    , m_fRotateSpeed(12.5f)
    , m_fZNear(10.f)
    , m_fZFar(1000.f)
    , m_fFOV(51.8f)
    , m_uLastUpdateFrameCounter(0)
{
    kmVec3Zero(&m_vec3Pos);
    kmVec3Zero(&m_vec3Rotation);
    kmVec2Fill(&m_vec2CameraCenterOffset, 0, 0);
    kmMat4Identity(&m_viewMatrix);
    kmMat4Identity(&m_projectionMatrix);
    SetNear(m_type == eCT_2D ? -1.f : 0.1f);
}

CCamera::~CCamera()
{

}

CCamera::ECameraType CCamera::GetType() const
{
    return m_type;
}

void CCamera::SetCamera(const kmVec3& pos,const kmVec3& rotate)
{
    kmVec3Assign(& m_vec3Pos, &pos);
    kmVec3Assign(& m_vec3Rotation, &rotate);
    InvalidateViewMatrix();
}

void  CCamera::Roll(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        m_vec3Rotation.z += angle * m_fRotateSpeed;
        InvalidateViewMatrix();
    }
}
void  CCamera::Pitch(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        m_vec3Rotation.x += angle * m_fRotateSpeed;
        InvalidateViewMatrix();
    }
}

void  CCamera::Yaw(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        m_vec3Rotation.y += angle * m_fRotateSpeed;
        InvalidateViewMatrix();
    }
}

void  CCamera::Translate(float x, float y, float z)
{
    SetViewPos(m_vec3Pos.x + x, m_vec3Pos.y + y, m_vec3Pos.z + z);
}

float CCamera::GetPitch() const
{
    return m_vec3Rotation.x;
}

float CCamera::GetYaw() const
{
    return m_vec3Rotation.y;
}

float CCamera::GetRoll() const
{
    return m_vec3Rotation.z;
}

void CCamera::SetNear(float fZNear)
{
    if (!BEATS_FLOAT_EQUAL(m_fZNear, fZNear))
    {
        m_fZNear = fZNear;
        InvalidateProjectionMatrix();
    }
}

void CCamera::SetFar(float fZFar)
{
    if (!BEATS_FLOAT_EQUAL(m_fZFar, fZFar))
    {
        m_fZFar = fZFar;
        InvalidateProjectionMatrix();
    }
}

void CCamera::SetFOV(float fFOV)
{
    if (!BEATS_FLOAT_EQUAL(m_fFOV, fFOV))
    {
        m_fFOV = fFOV;
        InvalidateProjectionMatrix();
    }
}

float CCamera::GetNear() const
{
    return m_fZNear;
}

float CCamera::GetFar() const
{
    return m_fZFar;
}

float CCamera::GetFOV() const
{
    return m_fFOV;
}

void CCamera::SetViewPos(float x, float y, float z)
{
    kmVec3Fill(&m_vec3Pos, x, y, z);
    InvalidateViewMatrix();
}

const kmVec3& CCamera::GetViewPos() const
{
    return m_vec3Pos;
}

void CCamera::SetRotation(float x, float y, float z)
{
    kmVec3Fill(&m_vec3Rotation, x, y, z);
    InvalidateViewMatrix();
}

const kmVec3& CCamera::GetRotation() const
{
    return m_vec3Rotation;
}

const kmMat4& CCamera::GetViewMatrix()
{
    if (m_bInvalidateViewMatrix)
    {
        kmMat4Identity(&m_viewMatrix);
        switch(m_type)
        {
        case eCT_2D:
            kmMat4Translation(&m_viewMatrix, m_vec3Pos.x, m_vec3Pos.y, m_vec3Pos.z);
            break;
        case eCT_3D:
            {
                kmMat4 rotation, axisXRotation, axisYRotation;
                kmMat4RotationX(&axisXRotation, kmDegreesToRadians(m_vec3Rotation.x));
                kmMat4RotationY(&axisYRotation, kmDegreesToRadians(m_vec3Rotation.y));
                kmMat4Multiply(&rotation, &axisYRotation, &axisXRotation);

                kmMat4 translate;
                kmMat4Translation(&translate, m_vec3Pos.x, m_vec3Pos.y, m_vec3Pos.z);
                kmMat4Multiply(&m_viewMatrix, &translate, &rotation);
                kmMat4Inverse(&m_viewMatrix, &m_viewMatrix);
            }
            break;
        default:
            BEATS_ASSERT(false);
            break;
        }
        m_bInvalidateViewMatrix = false;
    }
    return m_viewMatrix;
}

const kmMat4& CCamera::GetProjectionMatrix()
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    CRenderTarget* pRenderTarget = pRenderMgr->GetCurrentRenderTarget();
    size_t uWidth = (size_t)(pRenderTarget->GetWidth() * pRenderTarget->GetScaleFactor());
    size_t uHeight = (size_t)(pRenderTarget->GetHeight() * pRenderTarget->GetScaleFactor());
    if (m_uWidth != uWidth || m_uHeight != uHeight)
    {
        m_uWidth = uWidth;
        m_uHeight = uHeight;
        m_bInvalidateProjectionMatrix = true;
    }
    if (m_bInvalidateProjectionMatrix)
    {
        switch(m_type)
        {
        case eCT_2D:
            kmMat4OrthographicProjection(&m_projectionMatrix, 
                m_vec2CameraCenterOffset.x, 
                m_uWidth  + m_vec2CameraCenterOffset.x,
                m_uHeight + m_vec2CameraCenterOffset.y, 
                m_vec2CameraCenterOffset.y,
                m_fZNear, m_fZFar);
            break;
        case eCT_3D:
            kmMat4PerspectiveProjection(&m_projectionMatrix, m_fFOV, (float)m_uWidth / m_uHeight, m_fZNear, m_fZFar);
            break;
        }
        m_bInvalidateProjectionMatrix = false;
    }
    return m_projectionMatrix;
}

void CCamera::ExecuteMovement(const kmVec3& vec3Speed, int type)
{
    kmMat4 cameraMat = GetViewMatrix();
    kmMat4Inverse(&cameraMat, &cameraMat);
    kmVec3 vec3Translation;
    kmVec3Zero(&vec3Translation);
    if ((type & eCMT_TRANVERSE) != 0)
    {
        kmVec3 tmpTranslation;
        kmMat4GetRightVec3(&tmpTranslation, &cameraMat);
        kmVec3Scale(&tmpTranslation, &tmpTranslation, vec3Speed.x);
        tmpTranslation.y = 0;
        kmVec3Add(&vec3Translation, &vec3Translation, &tmpTranslation);
    }
    if ((type & eCMT_STRAIGHT) != 0)
    {
        kmVec3 tmpTranslation;
        kmMat4GetForwardVec3(&tmpTranslation, &cameraMat);
        kmVec3Scale(&tmpTranslation, &tmpTranslation, vec3Speed.z);
        kmVec3Add(&vec3Translation, &vec3Translation, &tmpTranslation);
    }
    if ((type & eCMT_UPDOWN) != 0)
    {
        kmVec3 tmpTranslation;
        kmMat4GetUpVec3(&tmpTranslation, &cameraMat);
        kmVec3Scale(&tmpTranslation, &tmpTranslation, vec3Speed.y);
        tmpTranslation.x = 0;
        tmpTranslation.z = 0;
        kmVec3Add(&vec3Translation, &vec3Translation, &tmpTranslation);
    }
    Translate(vec3Translation.x, vec3Translation.y, vec3Translation.z);
}

void CCamera::SetCenterOffset(const kmVec2& centerOffset)
{
    m_vec2CameraCenterOffset.x = centerOffset.x;
    m_vec2CameraCenterOffset.y = centerOffset.y;
    InvalidateProjectionMatrix();
}

const kmVec2& CCamera::GetCenterOffset() const
{
    return m_vec2CameraCenterOffset;
}

CVec3 CCamera::RayCast(float x, float y)
{
    kmMat4 viewMatrixInverse;
    const kmMat4& viewMatrix = GetViewMatrix();
    kmMat4Inverse(&viewMatrixInverse, &viewMatrix);
    kmVec3 rightDirection;
    kmMat4GetRightVec3(&rightDirection, &viewMatrixInverse);
    kmVec3 upDirection;
    kmMat4GetUpVec3(&upDirection, &viewMatrixInverse);
    kmVec3 forwardDirection;
    kmMat4GetForwardVec3(&forwardDirection, &viewMatrixInverse);

    float fFOVRadian = kmDegreesToRadians(m_fFOV);
    float halfHeight = (float) (tanf(fFOVRadian * 0.5f) * m_fZNear);
    CRenderTarget* pCurRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    size_t uWidth = pCurRenderTarget->GetWidth();
    size_t uHeight = pCurRenderTarget->GetHeight();
    float fHalfWidth = halfHeight * uWidth / uHeight;
    kmVec3Scale(&forwardDirection, &forwardDirection, m_fZNear);
    kmVec3Scale(&rightDirection, &rightDirection, fHalfWidth);
    kmVec3Scale(&upDirection, &upDirection, halfHeight);

    x -= uWidth * 0.5f;
    y -= uHeight * 0.5f;
    // normalize to 1
    x = ((float) x) / (uWidth * 0.5f);
    y = ((float) y)/ (uHeight * 0.5f);
    y*= -1;
    kmVec3Scale(&rightDirection, &rightDirection, x);
    kmVec3Scale(&upDirection, &upDirection, y);

    kmVec3 pos;
    kmVec3Subtract(&pos, &m_vec3Pos, & forwardDirection);
    kmVec3Add(&pos, &pos, &rightDirection);
    kmVec3Add(&pos, &pos, &upDirection);

    kmVec3 directionReally;
    kmVec3Subtract(&directionReally, &pos, &m_vec3Pos);
    float fScale = -pos.y / directionReally.y;
    kmVec3 worldPos;
    worldPos.x = pos.x + directionReally.x * fScale;
    worldPos.z = pos.z + directionReally.z * fScale;
    worldPos.y = 0;
    return worldPos;
}

CVec3 CCamera::QueryCameraPos(const kmVec3& worldPos, const kmVec2& screenPos)
{
    //NOTICE: these codes are comes form calculation.
    // Basically, read these codes doesn't make sense.
    kmMat4 viewMatrix = GetViewMatrix();
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    float fTheta = kmDegreesToRadians(GetFOV() * 0.5f);
    float fTanTheta = tan(fTheta);
    float aspect = (float)pRenderTarget->GetWidth() / pRenderTarget->GetHeight();
    float viewPosZ = worldPos.x * viewMatrix.mat[2] + worldPos.y * viewMatrix.mat[6] + worldPos.z * viewMatrix.mat[10] + viewMatrix.mat[14];
    float viewposX = (screenPos.x / pRenderTarget->GetWidth() * 2 - 1)*(fTanTheta * aspect * -viewPosZ);
    float viewPosY = -((1.0f - screenPos.y/pRenderTarget->GetHeight()) * 2 - 1.0f) * (fTanTheta * viewPosZ);

    viewMatrix.mat[12] = viewposX - (worldPos.x * viewMatrix.mat[0] + worldPos.y * viewMatrix.mat[4] + worldPos.z * viewMatrix.mat[8]);
    viewMatrix.mat[13] = viewPosY - (worldPos.x * viewMatrix.mat[1] + worldPos.y * viewMatrix.mat[5] + worldPos.z * viewMatrix.mat[9]);
    viewMatrix.mat[14] = viewPosZ - (worldPos.x * viewMatrix.mat[2] + worldPos.y * viewMatrix.mat[6] + worldPos.z * viewMatrix.mat[10]);

    kmMat4 invserViewMatrix;
    kmMat4Inverse(&invserViewMatrix, &viewMatrix);
    return CVec3(invserViewMatrix.mat[12], invserViewMatrix.mat[13], invserViewMatrix.mat[14]);
}

void CCamera::WorldToScreen( float x, float y, float z, CVec2& screenPos )
{
    BEATS_ASSERT( GetType() == eCT_3D );
    const kmMat4& viewMatrix = GetViewMatrix();
    const kmMat4& projectMatrix = GetProjectionMatrix();
    kmVec4 pos,pos1,pos2;
    kmVec4Fill(&pos2, x, y, z, 1.0f);
    kmVec4Transform(&pos1, &pos2, &viewMatrix);
    kmVec4Transform(&pos, &pos1, &projectMatrix);
    BEATS_ASSERT(!BEATS_FLOAT_EQUAL(pos.w, 0));
    screenPos.x = (pos.x / pos.w + 1.0f) * 0.5f;
    screenPos.y = (pos.y / pos.w + 1.0f) * 0.5f;
    screenPos.x = CRenderManager::GetInstance()->GetWidth() * screenPos.x;
    screenPos.y = CRenderManager::GetInstance()->GetHeight() * (1 - screenPos.y);
}

void CCamera::InvalidateViewMatrix()
{
    m_bInvalidateViewMatrix = true;
    m_uLastUpdateFrameCounter = CEngineCenter::GetInstance()->GetFrameCounter();
}

void CCamera::InvalidateProjectionMatrix()
{
    m_bInvalidateProjectionMatrix = true;
    m_uLastUpdateFrameCounter = CEngineCenter::GetInstance()->GetFrameCounter();
}

size_t CCamera::GetLastUpdateFrameCounter() const
{
    return m_uLastUpdateFrameCounter;
}
