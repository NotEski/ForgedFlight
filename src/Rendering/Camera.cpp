#include "Camera.h"
#include <algorithm>

Camera::Camera()
    : m_Position(0.0f, 10.0f, 10.0f)
    , m_WorldUp(0.0f, 1.0f, 0.0f)
    , m_Yaw(-90.0f)
    , m_Pitch(0.0f)
    , m_MovementSpeed(10.0f)
    , m_MouseSensitivity(0.1f)
    , m_FOV(45.0f)
    , m_AspectRatio(16.0f / 9.0f)
    , m_NearPlane(0.1f)
    , m_FarPlane(1000.0f)
    , m_ViewMatrixDirty(true)
    , m_ProjectionMatrixDirty(true)
{
    UpdateCameraVectors();
}

void Camera::SetPosition(const float3& position)
{
    m_Position = position;
    m_ViewMatrixDirty = true;
}

void Camera::SetRotation(float yaw, float pitch)
{
    m_Yaw = yaw;
    m_Pitch = std::clamp(pitch, -89.0f, 89.0f);
    UpdateCameraVectors();
    m_ViewMatrixDirty = true;
}

void Camera::MoveForward(float distance)
{
    // Create horizontal-only forward vector (ignore pitch, Y component = 0)
    float3 horizontalForward;
    horizontalForward.x = sin(m_Yaw * PI_F / 180.0f);
    horizontalForward.y = 0.0f;  // Keep Y at 0 for horizontal movement
    horizontalForward.z = -cos(m_Yaw * PI_F / 180.0f);
    horizontalForward = normalize(horizontalForward);
    
    m_Position += horizontalForward * -distance;
    m_ViewMatrixDirty = true;
}

void Camera::MoveRight(float distance)
{
    // Create horizontal-only right vector (ignore pitch, Y component = 0)
    float3 horizontalForward;
    horizontalForward.x = sin(m_Yaw * PI_F / 180.0f);
    horizontalForward.y = 0.0f;  // Keep Y at 0 for horizontal movement
    horizontalForward.z = -cos(m_Yaw * PI_F / 180.0f);
    
    float3 horizontalRight = normalize(cross(horizontalForward, m_WorldUp));
    m_Position += horizontalRight * distance;
    m_ViewMatrixDirty = true;
}

void Camera::MoveUp(float distance)
{
    // Use world up (Y-axis) instead of camera's local up vector
    // This ensures vertical movement is always along the world Y-axis
    m_Position += m_WorldUp * distance;
    m_ViewMatrixDirty = true;
}

void Camera::Rotate(float deltaYaw, float deltaPitch)
{
    m_Yaw += deltaYaw * m_MouseSensitivity;
    m_Pitch += deltaPitch * m_MouseSensitivity;
    m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
    
    UpdateCameraVectors();
    m_ViewMatrixDirty = true;
}

float4x4 Camera::GetViewMatrix() const
{
    if (m_ViewMatrixDirty)
    {
        // Create view matrix using translation and rotation like FirstPersonCamera
        float4x4 rotation = float4x4::RotationY(m_Yaw * PI_F / 180.0f) * 
                          float4x4::RotationX(m_Pitch * PI_F / 180.0f);
        m_ViewMatrix = float4x4::Translation(-m_Position) * rotation;
        m_ViewMatrixDirty = false;
    }
    return m_ViewMatrix;
}

float4x4 Camera::GetProjectionMatrix() const
{
    if (m_ProjectionMatrixDirty)
    {
        m_ProjectionMatrix = float4x4::Projection(m_FOV * PI_F / 180.0f, m_AspectRatio, m_NearPlane, m_FarPlane, false);
        m_ProjectionMatrixDirty = false;
    }
    return m_ProjectionMatrix;
}

float4x4 Camera::GetViewProjectionMatrix() const
{
    return GetViewMatrix() * GetProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    m_FOV = fov;
    m_AspectRatio = aspectRatio;
    m_NearPlane = nearPlane;
    m_FarPlane = farPlane;
    m_ProjectionMatrixDirty = true;
}

void Camera::UpdateCameraVectors()
{
    // Calculate the new front vector
    // Standard FPS camera: yaw=0 points forward (-Z), yaw=90 points right (+X)
    float3 front;
    front.x = sin(m_Yaw * PI_F / 180.0f) * cos(m_Pitch * PI_F / 180.0f);
    front.y = sin(m_Pitch * PI_F / 180.0f);
    front.z = -cos(m_Yaw * PI_F / 180.0f) * cos(m_Pitch * PI_F / 180.0f);
    
    m_Forward = normalize(front);
    m_Right = normalize(cross(m_Forward, m_WorldUp));
    m_Up = normalize(cross(m_Right, m_Forward));
}