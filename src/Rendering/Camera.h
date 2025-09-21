#pragma once

#include "Common/interface/BasicMath.hpp"

using namespace Diligent;

class Camera
{
public:
    Camera();
    ~Camera() = default;

    // Camera controls
    void SetPosition(const float3& position);
    void SetRotation(float yaw, float pitch);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);
    void Rotate(float deltaYaw, float deltaPitch);

    // Getters
    const float3& GetPosition() const { return m_Position; }
    const float3& GetForward() const { return m_Forward; }
    const float3& GetRight() const { return m_Right; }
    const float3& GetUp() const { return m_Up; }
    
    // Rotation getters
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    float3 GetForwardVector() const { return m_Forward; }
    
    // Projection getters
    float GetFOV() const { return m_FOV; }
    float GetAspectRatio() const { return m_AspectRatio; }
    float GetNearPlane() const { return m_NearPlane; }
    float GetFarPlane() const { return m_FarPlane; }
    
    float4x4 GetViewMatrix() const;
    float4x4 GetProjectionMatrix() const;
    float4x4 GetViewProjectionMatrix() const;

    // Camera settings
    void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
    void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
    float GetMovementSpeed() const { return m_MovementSpeed; }

    // Update camera vectors based on yaw and pitch
    void UpdateCameraVectors();

private:
    // Camera position and orientation
    float3 m_Position;
    float3 m_Forward;
    float3 m_Right;
    float3 m_Up;
    float3 m_WorldUp;

    // Euler angles
    float m_Yaw;
    float m_Pitch;

    // Camera settings
    float m_MovementSpeed;
    float m_MouseSensitivity;

    // Projection settings
    float m_FOV;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    // Cached matrices
    mutable float4x4 m_ViewMatrix;
    mutable float4x4 m_ProjectionMatrix;
    mutable bool m_ViewMatrixDirty;
    mutable bool m_ProjectionMatrixDirty;
};