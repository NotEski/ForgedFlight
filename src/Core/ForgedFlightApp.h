#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#ifndef NOMINMAX
#    define NOMINMAX
#endif
#include <Windows.h>

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"
#include "Graphics/GraphicsEngine/interface/Buffer.h"
#include "Common/interface/BasicMath.hpp"

// ImGui includes
#include "ImGui/interface/ImGuiImplDiligent.hpp"

using namespace Diligent;

// Forward declarations for voxel game components
class VoxelWorld;
class Camera;
class ChunkManager;
class AdvancedRenderer;

struct NativeAppInitAttrib
{
    HWND hWnd;
    RENDER_DEVICE_TYPE DeviceType;
};

class ForgedFlightApp
{
public:
    ForgedFlightApp();
    ~ForgedFlightApp();

    // Core application lifecycle
    void Initialize(const NativeAppInitAttrib& InitAttrib);
    void Update(double CurrTime, double ElapsedTime);
    void Render();
    void Present();
    void WindowResize(unsigned int Width, unsigned int Height);

    // Input handling
    void OnKeyDown(uint8_t key);
    void OnKeyUp(uint8_t key);
    void OnMouseMove(int x, int y);
    void OnMouseDown(int x, int y, unsigned int button);
    void OnMouseUp(int x, int y, unsigned int button);
    void OnMouseWheel(int delta);

    const char* GetAppTitle() const { return "Forged Flight - Voxel Game"; }

private:
    // Core rendering setup
    void InitializeDiligentEngine(HWND hWnd, RENDER_DEVICE_TYPE deviceType);
    void CreatePipelineState();
    void CreateCubePipelineState();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffer();

    // Voxel game specific
    void InitializeVoxelWorld();
    void UpdateCamera(double deltaTime);
    void RenderVoxelWorld();
    
    // Debug UI
    void InitializeImGui();
    void RenderImGuiDebugWindow();

    // Diligent Engine core objects
    RefCntAutoPtr<IRenderDevice>        m_pDevice;
    RefCntAutoPtr<IDeviceContext>       m_pImmediateContext;
    RefCntAutoPtr<ISwapChain>           m_pSwapChain;
    RefCntAutoPtr<IPipelineState>       m_pCubePSO;
    RefCntAutoPtr<IBuffer>              m_pCubeVertexBuffer;
    RefCntAutoPtr<IBuffer>              m_pCubeIndexBuffer;
    RefCntAutoPtr<IBuffer>              m_pVSConstants;
    RefCntAutoPtr<IShaderResourceBinding> m_pSRB;

    // ImGui integration
    std::unique_ptr<ImGuiImplDiligent>  m_pImGuiImpl;

    // Voxel game components
    std::unique_ptr<Camera>             m_pCamera;
    std::unique_ptr<VoxelWorld>         m_pVoxelWorld;
    std::unique_ptr<ChunkManager>       m_pChunkManager;
    std::unique_ptr<AdvancedRenderer>   m_pAdvancedRenderer;

    // Input state
    std::unordered_map<uint8_t, bool>       m_KeyStates;
    bool                                m_MouseButtons[3] = {false, false, false};
    int                                 m_LastMouseX = 0;
    int                                 m_LastMouseY = 0;
    bool                                m_FirstMouseMove = true;
    
    // Debug/rendering options
    // Timing
    double                              m_LastFrameTime = 0.0;

    // Window properties
    unsigned int                        m_WindowWidth = 1280;
    unsigned int                        m_WindowHeight = 720;
};