#ifndef BEYOND_ENGINE_RENDER_CAMERA_H__INCLUDE
#define BEYOND_ENGINE_RENDER_CAMERA_H__INCLUDE

class CCamera
{
public:
    enum ECameraType
    {
        eCT_2D,
        eCT_3D,
    };

    enum ECameraMoveType
    {
        eCMT_NOMOVE = 0,
        eCMT_TRANVERSE = 1,
        eCMT_STRAIGHT = 2,
        eCMT_UPDOWN = 4,

        eCMT_Count,
        eCMT_Force32Bit = 0xFFFFFFFF
    };

public:
    CCamera(ECameraType type = eCT_3D);
    ~CCamera();

    ECameraType GetType() const;

    void SetCamera(const kmVec3& pos,const kmVec3& rotate);
    void Roll(float angle);
    void Pitch(float angle);
    void Yaw(float angle);
    void Translate(float x, float y, float z);

    float GetPitch() const;
    float GetYaw() const;
    float GetRoll() const;

    void SetNear(float fZNear);
    void SetFar(float fZFar);
    void SetFOV(float fFOV);

    float GetNear() const;
    float GetFar() const;
    float GetFOV() const;

    void SetViewPos(float x, float y, float z);
    const kmVec3& GetViewPos() const;

    void SetRotation(float x, float y, float z);
    const kmVec3& GetRotation() const;

    const kmMat4& GetProjectionMatrix();
    const kmMat4& GetViewMatrix();

    void ExecuteMovement(const kmVec3& vec3Speed, int type);

    void SetCenterOffset(const kmVec2& centerOffset);
    const kmVec2& GetCenterOffset() const;

    CVec3 RayCast(float x, float y);
    // This method return a camera pos that world pos will be projected to screen pos.
    CVec3 QueryCameraPos(const kmVec3& worldPos, const kmVec2& screenPos);

    void WorldToScreen( float x, float y, float z , CVec2& screenPosition );

    void InvalidateViewMatrix();
    void InvalidateProjectionMatrix();

    size_t GetLastUpdateFrameCounter() const;

private:
    bool m_bInvalidateViewMatrix;
    bool m_bInvalidateProjectionMatrix;
    ECameraType m_type;
    size_t m_uWidth;
    size_t m_uHeight;
    float m_fRotateSpeed;
    float m_fZNear;
    float m_fZFar;
    float m_fFOV; //InDegree
    size_t m_uLastUpdateFrameCounter;
    kmMat4 m_viewMatrix;
    kmMat4 m_projectionMatrix;
    // TODO: m_vec3Pos and m_vec3Rotation should be removed.
    kmVec3 m_vec3Pos;
    kmVec3 m_vec3Rotation;//Rotation in degree.
    kmVec2 m_vec2CameraCenterOffset;
};
 
#endif