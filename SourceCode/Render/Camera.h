#ifndef BEYOND_ENGINE_RENDER_CAMERA_H__INCLUDE
#define BEYOND_ENGINE_RENDER_CAMERA_H__INCLUDE

struct SCameraData
{
    SCameraData()
    {

    }
    SCameraData(const SCameraData& rhs)
        : m_vec2CameraCenterOffset(rhs.m_vec2CameraCenterOffset)
    {
        m_uTargetWidth = rhs.m_uTargetWidth;
        m_uTargetHeight = rhs.m_uTargetHeight;
        m_fZNear = rhs.m_fZNear;
        m_fZFar = rhs.m_fZFar;
        m_fFOV = rhs.m_fFOV;
        m_vec3Pos = rhs.m_vec3Pos;
        m_vec3Rotation = rhs.m_vec3Rotation;
        m_vec3ShakeOffset = rhs.m_vec3ShakeOffset;
    }
    uint32_t m_uTargetWidth = 0;
    uint32_t m_uTargetHeight = 0;
    float m_fZNear = 0.1f;
    float m_fZFar = 1000.f;
    float m_fFOV = 51.8f; //InDegree
    CVec2 m_vec2CameraCenterOffset;
    CVec3 m_vec3Pos;
    CVec3 m_vec3Rotation;//Rotation in degree.
    CVec3 m_vec3ShakeOffset; // TODO: HACK: Remove it, it should be in logic code.
};
enum class EFrustumPlaneType
{
    eFPT_Near = 0,
    eFPT_Far = 1,
    eFPT_Left = 2,
    eFPT_Right = 3,
    eFPT_Top = 4,
    eFPT_Bottom = 5,

    eFPT_Count,
    eFPT_Force32Bit = -1
};

struct SFrustumPlanes
{
    CPlane m_planes[(uint32_t)EFrustumPlaneType::eFPT_Count];
};

class CCamera
{
public:
    enum ECameraType
    {
        eCT_2D,
        eCT_3D,
    };

public:
    CCamera(ECameraType type = eCT_3D);
    ~CCamera();

    ECameraType GetType() const;

    void Roll(float angle);
    void Pitch(float angle);
    void Yaw(float angle);

    float GetPitch() const;
    float GetYaw() const;
    float GetRoll() const;

    void SetNear(float fZNear);
    void SetFar(float fZFar);
    void SetFOV(float fFOV);

    float GetNear() const;
    float GetFar() const;
    float GetFOV() const;

    void SetViewPos(const CVec3& viewPos);
    const CVec3& GetViewPos() const;

    void SetRotation(const CVec3& rotation);
    const CVec3& GetRotation() const;

    const CMat4& GetProjectionMatrix();
    const CMat4& GetViewMatrix(bool bWithShake = false);
    const CMat4& GetViewMatrixInverse();
    const CMat4& GetViewProjectionMatrix(bool bWithShake = false);

    void SetCenterOffset(const CVec2& centerOffset);
    const CVec2& GetCenterOffset() const;

    CVec3 RayCast(float x, float y) const;
    CRay3 GetRayFromScreenPos(float x, float y) const;
    CVec2 WorldToScreen(const CVec3& worldPos, bool bWithShake = false);

    void Lock();
    void Unlock();
    void InvalidateViewMatrix();
    void InvalidateProjectionMatrix();
    void SetShakeOffset(float fOffsetX, float fOffsetY, float fOffsetZ);
    const CVec3& GetShakeOffset() const;
    void SetCameraData(const SCameraData& cameraData);
    const SCameraData& GetCameraData() const;
    const SFrustumPlanes& GetFrustumPlanes();
    static CVec3 QueryCameraPos(const CVec3& worldPos, const CVec2& screenPos, const CMat4& viewMatrix,
        float fNear, float fFov, float fCameraHeight);

#ifdef EDITOR_MODE
public:
    uint32_t m_uLastUpdateFrameCounter = 0;
    float m_fMoveSpeed = 1;
    float m_fShiftMoveSpeedRate = 5;
#endif

private:
    bool m_bLocked;
    bool m_bInvalidateViewMatrix;
    bool m_bInvalidateProjectionMatrix;
    bool m_bInvalidFrustumPlanes;
    ECameraType m_type;
    float m_fRotateSpeed;
    CMat4 m_viewMatrix;
    CMat4 m_viewMatrixWithShake;
    CMat4 m_viewMatrixInverse;
    CMat4 m_projectionMatrix;
    CMat4 m_viewProjectionMatrix;
    CMat4 m_viewProjectionMatrixWithShake;
    SCameraData m_cameraData;
    SFrustumPlanes m_frustumPlanes;
};
 
#endif