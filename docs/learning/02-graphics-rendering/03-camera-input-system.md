# Chapter 3: Camera System and Input Handling

## Overview
This chapter covers implementing a robust camera system with mouse look, keyboard movement, and input state management. We'll build the foundation for navigating the voxel world with modern FPS-style controls.

## Learning Objectives
By the end of this chapter, you will understand:
- 3D camera mathematics and transformations
- Windows input message handling
- Mouse look implementation with sensitivity controls
- Keyboard state management for smooth movement
- Mouse capture and release functionality

## Key Concepts

### 1. Camera Mathematics

A 3D camera requires several coordinate systems and transformations:

**World Space**: Global 3D coordinate system where objects exist
**View Space**: Camera-relative coordinates (camera at origin, looking down -Z)
**Clip Space**: Perspective-projected coordinates for rasterization

**Essential Matrices:**
- **View Matrix**: Transforms world space to view space
- **Projection Matrix**: Transforms view space to clip space
- **ViewProjection**: Combined transformation (Projection × View)

### 2. Camera Representation

We use **Euler angles** for simplicity:
```cpp
class Camera {
    float3 m_Position;    // Camera world position
    float  m_Yaw;         // Y-axis rotation (left/right)
    float  m_Pitch;       // X-axis rotation (up/down)
    
    // Computed vectors
    float3 m_Forward;     // Where camera is looking
    float3 m_Right;       // Camera's right direction
    float3 m_Up;          // Camera's up direction
};
```

**Coordinate System Conventions:**
- **Yaw**: 0° = looking down -Z axis, 90° = looking down -X axis
- **Pitch**: 0° = looking horizontally, +90° = looking up
- **Pitch Clamp**: -89° to +89° (prevents gimbal lock)

### 3. Input System Architecture

**Two-Layer Input Handling:**
1. **Raw Input Layer**: Windows messages (WM_KEYDOWN, WM_MOUSEMOVE)
2. **Game Input Layer**: Processed input state (IsKeyPressed, GetMouseDelta)

**Key Features:**
- **State Tracking**: Current frame vs previous frame comparison
- **Mouse Lock**: Capture mouse for camera control, release for UI
- **Smooth Movement**: Frame-rate independent movement using delta time

## Implementation Walkthrough

### Step 1: Camera Class Design

**Camera.h Structure:**
```cpp
class Camera
{
public:
    Camera();
    
    // Transform controls
    void SetPosition(const float3& position);
    void SetRotation(float yaw, float pitch);
    
    // Movement functions
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);
    void Rotate(float deltaYaw, float deltaPitch);
    
    // Matrix generation
    float4x4 GetViewMatrix() const;
    float4x4 GetProjectionMatrix() const;
    float4x4 GetViewProjectionMatrix() const;
    
    // Configuration
    void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
    void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }

private:
    // Camera state
    float3 m_Position;
    float3 m_Forward, m_Right, m_Up;
    float  m_Yaw, m_Pitch;
    
    // Settings
    float m_MovementSpeed;
    float m_MouseSensitivity;
    
    // Projection parameters
    float m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane;
    
    // Matrix caching
    mutable float4x4 m_ViewMatrix;
    mutable bool m_ViewMatrixDirty;
    
    void UpdateCameraVectors();
};
```

### Step 2: Camera Vector Calculation

**UpdateCameraVectors Implementation:**
```cpp
void Camera::UpdateCameraVectors()
{
    // Calculate forward vector from yaw and pitch
    float3 forward;
    forward.x = cos(m_Yaw * PI / 180.0f) * cos(m_Pitch * PI / 180.0f);
    forward.y = sin(m_Pitch * PI / 180.0f);
    forward.z = sin(m_Yaw * PI / 180.0f) * cos(m_Pitch * PI / 180.0f);
    
    m_Forward = normalize(forward);
    
    // Calculate right and up vectors
    m_Right = normalize(cross(m_Forward, float3(0, 1, 0))); // World up
    m_Up = normalize(cross(m_Right, m_Forward));
    
    m_ViewMatrixDirty = true;
}
```

**View Matrix Generation:**
```cpp
float4x4 Camera::GetViewMatrix() const
{
    if (m_ViewMatrixDirty)
    {
        // Look-at matrix calculation
        float3 target = m_Position + m_Forward;
        m_ViewMatrix = ViewMatrix(m_Position, target, m_Up);
        m_ViewMatrixDirty = false;
    }
    return m_ViewMatrix;
}
```

### Step 3: Input Message Handling

**Windows Message Processing:**
```cpp
// In WindowProc function
case WM_KEYDOWN:
{
    if (g_pTheApp)
        g_pTheApp->OnKeyDown(static_cast<UINT8>(wParam));
    return 0;
}

case WM_KEYUP:
{
    if (g_pTheApp)
        g_pTheApp->OnKeyUp(static_cast<UINT8>(wParam));
    return 0;
}

case WM_MOUSEMOVE:
{
    if (g_pTheApp)
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        g_pTheApp->OnMouseMove(xPos, yPos);
    }
    return 0;
}
```

**Application Input Processing:**
```cpp
void ForgedFlightApp::OnKeyDown(uint8_t key)
{
    m_KeyStates[key] = true;
}

void ForgedFlightApp::OnKeyUp(uint8_t key)
{
    m_KeyStates[key] = false;
}

void ForgedFlightApp::OnMouseMove(int x, int y)
{
    if (m_FirstMouseMove)
    {
        m_LastMouseX = x;
        m_LastMouseY = y;
        m_FirstMouseMove = false;
        return;
    }
    
    int deltaX = x - m_LastMouseX;
    int deltaY = y - m_LastMouseY;
    m_LastMouseX = x;
    m_LastMouseY = y;
    
    // Apply mouse input to camera (if mouse is captured)
    if (m_pCamera && m_MouseCaptured)
    {
        m_pCamera->Rotate(
            static_cast<float>(deltaX) * 0.1f,  // Sensitivity factor
            static_cast<float>(-deltaY) * 0.1f  // Invert Y axis
        );
    }
}
```

### Step 4: Mouse Capture System

**Mouse Lock Implementation:**
```cpp
class ForgedFlightApp 
{
private:
    bool m_MouseCaptured = false;
    HWND m_hWnd;
    
public:
    void SetMouseCaptured(bool captured)
    {
        m_MouseCaptured = captured;
        
        if (captured)
        {
            // Hide cursor and confine to window
            ShowCursor(FALSE);
            RECT windowRect;
            GetWindowRect(m_hWnd, &windowRect);
            ClipCursor(&windowRect);
            
            // Center cursor
            POINT center = {
                (windowRect.left + windowRect.right) / 2,
                (windowRect.top + windowRect.bottom) / 2
            };
            SetCursorPos(center.x, center.y);
        }
        else
        {
            // Show cursor and release confinement
            ShowCursor(TRUE);
            ClipCursor(nullptr);
        }
    }
};

// Toggle on left click
void ForgedFlightApp::OnMouseDown(int x, int y, unsigned int button)
{
    if (button == 0) // Left mouse button
    {
        SetMouseCaptured(!m_MouseCaptured);
    }
}
```

### Step 5: Camera Update Loop

**Frame Update with Input Processing:**
```cpp
void ForgedFlightApp::UpdateCamera(double deltaTime)
{
    if (!m_pCamera) return;
    
    float moveSpeed = m_pCamera->GetMovementSpeed() * static_cast<float>(deltaTime);
    
    // WASD movement
    if (m_KeyStates['W'] || m_KeyStates['w'])
        m_pCamera->MoveForward(moveSpeed);
    if (m_KeyStates['S'] || m_KeyStates['s'])
        m_pCamera->MoveForward(-moveSpeed);
    if (m_KeyStates['A'] || m_KeyStates['a'])
        m_pCamera->MoveRight(-moveSpeed);
    if (m_KeyStates['D'] || m_KeyStates['d'])
        m_pCamera->MoveRight(moveSpeed);
        
    // Vertical movement
    if (m_KeyStates[VK_SPACE])
        m_pCamera->MoveUp(moveSpeed);
    if (m_KeyStates[VK_LSHIFT])
        m_pCamera->MoveUp(-moveSpeed);
        
    // Escape to release mouse
    if (m_KeyStates[VK_ESCAPE])
        SetMouseCaptured(false);
}
```

### Step 6: Projection Matrix Setup

**Perspective Projection:**
```cpp
void Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    m_FOV = fov;
    m_AspectRatio = aspectRatio;
    m_NearPlane = nearPlane;
    m_FarPlane = farPlane;
    
    m_ProjectionMatrixDirty = true;
}

float4x4 Camera::GetProjectionMatrix() const
{
    if (m_ProjectionMatrixDirty)
    {
        m_ProjectionMatrix = PerspectiveMatrix(
            m_FOV * PI / 180.0f,  // Convert to radians
            m_AspectRatio,
            m_NearPlane,
            m_FarPlane
        );
        m_ProjectionMatrixDirty = false;
    }
    return m_ProjectionMatrix;
}
```

## Factory System Considerations

### 1. **Multi-Camera Support**
Factory systems may need multiple viewports:
```cpp
// Future: Camera management for factory overview
class CameraManager {
    std::vector<std::unique_ptr<Camera>> m_Cameras;
    size_t m_ActiveCamera = 0;
    
public:
    void AddFactoryOverviewCamera(const float3& position);
    void SwitchToCamera(size_t index);
};
```

### 2. **Input Context Switching**
Different input modes for building vs navigation:
```cpp
enum class InputMode {
    Navigation,    // Free camera movement
    Building,      // Block placement mode
    UI            // Interface interaction
};
```

### 3. **Camera Persistence**
Save/load camera positions for factory management:
```cpp
struct CameraState {
    float3 Position;
    float Yaw, Pitch;
    std::string Name; // "Main Factory", "Power Plant", etc.
};
```

## Common Issues and Solutions

### Issue 1: Camera Moves Too Fast/Slow
**Symptoms:** Jerky movement or too slow response
**Solution:** Adjust movement speed and mouse sensitivity, use delta time

### Issue 2: Mouse Look Jumps
**Symptoms:** Camera snaps to random orientations
**Solution:** Handle first mouse move separately, check delta calculations

### Issue 3: Gimbal Lock
**Symptoms:** Camera flips when looking straight up/down
**Solution:** Clamp pitch to ±89 degrees, consider quaternions for complex cases

### Issue 4: Cursor Not Hidden
**Symptoms:** Cursor visible during mouse look
**Solution:** Check ShowCursor(FALSE) calls, ensure proper mouse capture

## Testing and Validation

### Movement Tests
1. **Smooth Movement**: WASD moves camera smoothly in all directions
2. **Mouse Look**: Mouse movement rotates camera view correctly
3. **Mouse Toggle**: Left-click captures/releases mouse cursor
4. **Escape Release**: Escape key always releases mouse capture
5. **Speed Control**: Movement speed feels comfortable and controllable

### Matrix Tests
1. **View Matrix**: Objects appear in correct positions relative to camera
2. **Projection**: Perspective looks natural, no distortion
3. **Combined**: ViewProjection matrix transforms coordinates correctly

## Performance Considerations

- **Matrix Caching**: Only recalculate matrices when camera moves
- **Input Polling**: Process input once per frame, not per message
- **Memory Usage**: < 1KB per camera instance
- **CPU Impact**: < 1% CPU for camera updates

## Next Steps

Chapter 4 will cover:
- Block type system and properties
- Voxel data structures
- Chunk-based world organization
- Basic world generation

## Key Resources

- [3D Camera Implementation](https://learnopengl.com/Getting-started/Camera)
- [Game Input Systems](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-raw-input)
- [Matrix Mathematics](https://www.3dgep.com/understanding-the-view-matrix/)
- [Mouse Capture Documentation](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-clipcursor)

## Chapter Summary

This chapter implemented a complete camera and input system:
- **3D Camera**: Position, orientation, and movement with proper mathematics
- **Input Handling**: Keyboard and mouse input processing with state management
- **Mouse Capture**: Professional-grade mouse lock system for camera control
- **Performance**: Efficient matrix caching and frame-rate independent movement

The camera system now provides intuitive navigation controls and sets the foundation for exploring the voxel world that will be built in subsequent chapters.