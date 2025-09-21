#include "ForgedFlightApp.h"
#include "../Rendering/Camera.h"
#include "../Rendering/AdvancedRenderer.h"
#include "../World/VoxelWorld.h"
#include "../World/ChunkManager.h"

// ImGui includes
#include "ThirdParty/imgui/imgui.h"

// Windows includes for console
#include <windows.h>
#include <io.h>
#include <fcntl.h>

// Graphics engine includes
#include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#include "Common/interface/StringDataBlobImpl.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace Diligent;

ForgedFlightApp::ForgedFlightApp()
{
}

ForgedFlightApp::~ForgedFlightApp()
{
    if (m_pImmediateContext)
        m_pImmediateContext->Flush();
}

void ForgedFlightApp::Initialize(const NativeAppInitAttrib& InitAttrib)
{
    try 
    {
        // Allocate a console for this GUI application
        AllocConsole();
        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        
        std::cout << "Starting ForgedFlightApp::Initialize" << std::endl;
        
        m_WindowWidth = 1280;
        m_WindowHeight = 720;
        
        std::cout << "About to initialize graphics engine" << std::endl;
        
        // Initialize graphics engine
        InitializeDiligentEngine(InitAttrib.hWnd, InitAttrib.DeviceType);
        
        std::cout << "Graphics engine initialized, creating pipeline" << std::endl;
        
        // Create rendering resources
        CreateCubePipelineState();
        
        std::cout << "Pipeline created, creating uniform buffer" << std::endl;
        
        CreateUniformBuffer();
        
        std::cout << "Uniform buffer created, initializing voxel components" << std::endl;
        
        // Initialize voxel game components
        m_pCamera = std::make_unique<Camera>();
        m_pCamera->SetPerspective(45.0f, static_cast<float>(m_WindowWidth) / m_WindowHeight, 0.1f, 1000.0f);
        m_pCamera->SetPosition(float3(0.0f, 50.0f, 0.0f));
        
        std::cout << "Camera created, creating voxel world" << std::endl;
        
        m_pVoxelWorld = std::make_unique<VoxelWorld>();
        m_pChunkManager = std::make_unique<ChunkManager>(m_pDevice, m_pImmediateContext);
        
        std::cout << "About to initialize voxel world" << std::endl;
        
        InitializeVoxelWorld();
        
        std::cout << "Voxel world initialized, setting up advanced renderer" << std::endl;
        
        // Initialize advanced renderer with DiligentFX features
        m_pAdvancedRenderer = std::make_unique<AdvancedRenderer>();
        m_pAdvancedRenderer->Initialize(m_pDevice, m_pImmediateContext, m_WindowWidth, m_WindowHeight);
        
        std::cout << "Advanced renderer initialized" << std::endl;
        
        // Initialize ImGui
        InitializeImGui();
        
        std::cout << "ImGui initialized" << std::endl;
        
        // Don't initialize ImGui here - do it lazily when first needed
        
        std::cout << "Advanced renderer initialized with DiligentFX features" << std::endl;
        
        std::cout << "ForgedFlightApp initialization completed successfully!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::string error = "Failed to initialize ForgedFlightApp: ";
        error += e.what();
        std::cout << error << std::endl;
        MessageBoxA(nullptr, error.c_str(), "Initialization Error", MB_OK | MB_ICONERROR);
        throw;
    }
    catch (...)
    {
        std::cout << "Unknown error during ForgedFlightApp initialization" << std::endl;
        MessageBoxA(nullptr, "Unknown error during ForgedFlightApp initialization", "Initialization Error", MB_OK | MB_ICONERROR);
        throw;
    }
}

void ForgedFlightApp::InitializeDiligentEngine(HWND hWnd, RENDER_DEVICE_TYPE deviceType)
{
    SwapChainDesc SCDesc;
    SCDesc.Width = m_WindowWidth;
    SCDesc.Height = m_WindowHeight;
    
    switch (deviceType)
    {
#if D3D11_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D11:
        {
            EngineD3D11CreateInfo EngineCI;
            auto* pFactoryD3D11 = GetEngineFactoryD3D11();
            pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_pDevice, &m_pImmediateContext);
            Win32NativeWindow Window{hWnd};
            pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
            break;
        }
#endif

#if D3D12_SUPPORTED
        case RENDER_DEVICE_TYPE_D3D12:
        {
            EngineD3D12CreateInfo EngineCI;
            auto* pFactoryD3D12 = GetEngineFactoryD3D12();
            pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);
            Win32NativeWindow Window{hWnd};
            pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
            break;
        }
#endif

#if GL_SUPPORTED
        case RENDER_DEVICE_TYPE_GL:
        {
            auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
            EngineGLCreateInfo EngineCI;
            EngineCI.Window.hWnd = hWnd;
            pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);
            break;
        }
#endif

#if VULKAN_SUPPORTED
        case RENDER_DEVICE_TYPE_VULKAN:
        {
            EngineVkCreateInfo EngineCI;
            auto* pFactoryVk = GetEngineFactoryVk();
            pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_pDevice, &m_pImmediateContext);
            if (hWnd != nullptr)
            {
                Win32NativeWindow Window{hWnd};
                pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, Window, &m_pSwapChain);
            }
            break;
        }
#endif

        default:
            throw std::runtime_error("Unsupported device type");
    }
}

void ForgedFlightApp::CreateCubePipelineState()
{
    // Create shader source factory
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pDevice->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);

    // Vertex shader for voxel cubes
    const char* VSSource = R"(
        cbuffer Constants
        {
            float4x4 ViewProjMatrix;
        };

        struct VSInput
        {
            float3 Pos : ATTRIB0;
            float3 Normal : ATTRIB1;
            float2 UV : ATTRIB2;
        };

        struct PSInput
        {
            float4 Pos : SV_POSITION;
            float3 Normal : NORMAL;
            float2 UV : TEXCOORD;
            float3 WorldPos : WORLD_POS;
        };

        void main(in VSInput VSIn, out PSInput PSOut)
        {
            PSOut.WorldPos = VSIn.Pos;
            PSOut.Pos = mul(ViewProjMatrix, float4(VSIn.Pos, 1.0));
            PSOut.Normal = VSIn.Normal;
            PSOut.UV = VSIn.UV;
        }
    )";

    // Pixel shader for voxel cubes with UV-based coloring
    const char* PSSource = R"(
        struct PSInput
        {
            float4 Pos : SV_POSITION;
            float3 Normal : NORMAL;
            float2 UV : TEXCOORD;
            float3 WorldPos : WORLD_POS;
        };

        struct PSOutput
        {
            float4 Color : SV_TARGET;
        };

        void main(in PSInput PSIn, out PSOutput PSOut)
        {
            // Simple lighting calculation
            float3 lightDir = normalize(float3(0.3, 0.8, 0.5));
            float NdotL = max(dot(PSIn.Normal, lightDir), 0.2);
            
            // Use UV coordinates as RGB color
            // UV coordinates range from 0-1, perfect for RGB values
            float3 baseColor = float3(PSIn.UV.x, PSIn.UV.y, 0.5);
            
            // Add some variation based on the normal to distinguish faces
            if (abs(PSIn.Normal.x) > 0.5) // Left/Right faces
            {
                baseColor = float3(PSIn.UV.y, 0.5, PSIn.UV.x);
            }
            else if (abs(PSIn.Normal.z) > 0.5) // Front/Back faces  
            {
                baseColor = float3(0.5, PSIn.UV.x, PSIn.UV.y);
            }
            // Top/Bottom faces use the original UV mapping
            
            PSOut.Color = float4(baseColor * NdotL, 1.0);
        }
    )";

    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

    // Create vertex shader
    RefCntAutoPtr<IShader> pVS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = "Cube vertex shader";
    ShaderCI.Source = VSSource;
    m_pDevice->CreateShader(ShaderCI, &pVS);

    // Create pixel shader
    RefCntAutoPtr<IShader> pPS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = "Cube pixel shader";
    ShaderCI.Source = PSSource;
    m_pDevice->CreateShader(ShaderCI, &pPS);

    // Define vertex layout
    LayoutElement LayoutElems[] =
    {
        LayoutElement{0, 0, 3, VT_FLOAT32, False}, // Position
        LayoutElement{1, 0, 3, VT_FLOAT32, False}, // Normal
        LayoutElement{2, 0, 2, VT_FLOAT32, False}  // UV
    };

    // Create pipeline state
    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    PSOCreateInfo.PSODesc.Name = "Cube PSO";
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    
    // Define shader variables
    ShaderResourceVariableDesc Vars[] = 
    {
        {SHADER_TYPE_VERTEX, "Constants", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pCubePSO);

    if (!m_pCubePSO)
    {
        std::cout << "Failed to create graphics pipeline state" << std::endl;
        MessageBoxA(nullptr, "Failed to create graphics pipeline state", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    std::cout << "Pipeline state created, creating SRB" << std::endl;

    // Create shader resource binding
    m_pCubePSO->CreateShaderResourceBinding(&m_pSRB, true);
    
    if (!m_pSRB)
    {
        std::cout << "Failed to create shader resource binding" << std::endl;
        MessageBoxA(nullptr, "Failed to create shader resource binding", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    std::cout << "SRB created successfully" << std::endl;
}

void ForgedFlightApp::CreateUniformBuffer()
{
    try 
    {
        std::cout << "Starting CreateUniformBuffer" << std::endl;
        
        if (!m_pDevice)
        {
            std::cout << "Error: m_pDevice is null" << std::endl;
            return;
        }
        
        // Create uniform buffer for transformation matrices
        BufferDesc BuffDesc;
        BuffDesc.Name = "VS constants CB";
        BuffDesc.Size = sizeof(float4x4);
        BuffDesc.Usage = USAGE_DYNAMIC;
        BuffDesc.BindFlags = BIND_UNIFORM_BUFFER;
        BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pVSConstants);
        
        std::cout << "Buffer created successfully" << std::endl;
        
        if (!m_pSRB)
        {
            std::cout << "Error: m_pSRB is null, cannot bind buffer" << std::endl;
            return;
        }
        
        if (!m_pVSConstants)
        {
            std::cout << "Error: m_pVSConstants is null" << std::endl;
            return;
        }
        
        // Try different variable names that might be in the shader
        std::vector<std::string> possibleNames = {"Constants", "VSConstants", "cbConstants", "CBConstants", "UniformBuffer", "g_Constants"};
        
        bool found = false;
        for (const auto& name : possibleNames)
        {
            std::cout << "Trying to find variable: " << name << std::endl;
            
            auto* pVar = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, name.c_str());
            if (pVar)
            {
                std::cout << "Found variable '" << name << "', binding buffer" << std::endl;
                pVar->Set(m_pVSConstants);
                found = true;
                break;
            }
            else
            {
                std::cout << "Variable '" << name << "' not found" << std::endl;
            }
        }
        
        if (!found)
        {
            std::cout << "Error: Could not find any constants variable in shader with any of the tried names" << std::endl;
            return;
        }
        
        std::cout << "CreateUniformBuffer completed successfully" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::string error = "CreateUniformBuffer failed: ";
        error += e.what();
        std::cout << error << std::endl;
        MessageBoxA(nullptr, error.c_str(), "Error", MB_OK | MB_ICONERROR);
        throw;
    }
}

void ForgedFlightApp::InitializeVoxelWorld()
{
    // Load some initial chunks around the player (cubic area)
    for (int x = -2; x <= 2; ++x)
    {
        for (int y = -1; y <= 3; ++y)  // Load chunks from underground to sky
        {
            for (int z = -2; z <= 2; ++z)
            {
                m_pVoxelWorld->LoadChunk(x, y, z);
            }
        }
    }
}

void ForgedFlightApp::Update(double CurrTime, double ElapsedTime)
{
    std::cout << "Update: Starting" << std::endl;
    
    // Update camera based on input
    std::cout << "Update: About to call UpdateCamera" << std::endl;
    UpdateCamera(ElapsedTime);
    std::cout << "Update: UpdateCamera completed" << std::endl;
    
    // Temporarily disable voxel world updates to isolate the crash
    /*
    // Update voxel world
    if (m_pVoxelWorld)
    {
        std::cout << "Update: Updating voxel world" << std::endl;
        m_pVoxelWorld->Update(m_pCamera->GetPosition());
        std::cout << "Update: Voxel world updated" << std::endl;
    }
    
    // Update chunk manager
    if (m_pChunkManager)
    {
        std::cout << "Update: Updating chunk manager" << std::endl;
        m_pChunkManager->UpdateChunkBuffers(m_pVoxelWorld.get());
        std::cout << "Update: Chunk manager updated" << std::endl;
    }
    */
    
    std::cout << "Update: Completed" << std::endl;
}

void ForgedFlightApp::UpdateCamera(double deltaTime)
{
    if (!m_pCamera)
        return;
    
    float speed = m_pCamera->GetMovementSpeed() * static_cast<float>(deltaTime);
    
    // Movement controls - use safer key checking and convert to uppercase
    if ((m_KeyStates.find('W') != m_KeyStates.end() && m_KeyStates['W']) ||
        (m_KeyStates.find('w') != m_KeyStates.end() && m_KeyStates['w']))
        m_pCamera->MoveForward(speed);
        
    if ((m_KeyStates.find('S') != m_KeyStates.end() && m_KeyStates['S']) ||
        (m_KeyStates.find('s') != m_KeyStates.end() && m_KeyStates['s']))
        m_pCamera->MoveForward(-speed);
        
    if ((m_KeyStates.find('A') != m_KeyStates.end() && m_KeyStates['A']) ||
        (m_KeyStates.find('a') != m_KeyStates.end() && m_KeyStates['a']))
        m_pCamera->MoveRight(-speed);
        
    if ((m_KeyStates.find('D') != m_KeyStates.end() && m_KeyStates['D']) ||
        (m_KeyStates.find('d') != m_KeyStates.end() && m_KeyStates['d']))
        m_pCamera->MoveRight(speed);
        
    if (m_KeyStates.find(VK_SPACE) != m_KeyStates.end() && m_KeyStates[VK_SPACE])
        m_pCamera->MoveUp(speed);
        
    if (m_KeyStates.find(VK_LSHIFT) != m_KeyStates.end() && m_KeyStates[VK_LSHIFT])
        m_pCamera->MoveUp(-speed);
}

void ForgedFlightApp::Render()
{
    std::cout << "Render: Starting" << std::endl;
    
    // Begin advanced rendering frame (sets up HDR render targets)
    if (m_pAdvancedRenderer)
    {
        m_pAdvancedRenderer->BeginFrame();
    }
    
    // Clear the HDR render target or back buffer
    const float ClearColor[] = {0.4f, 0.6f, 1.0f, 1.0f}; // Sky blue
    ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();

    std::cout << "Render: Setting render targets" << std::endl;
    m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    std::cout << "Render: Cleared buffers" << std::endl;

    if (m_pCamera)
    {
        std::cout << "Render: Updating view-projection matrix" << std::endl;
        
        // Update view-projection matrix
        float4x4 viewProjMatrix = m_pCamera->GetViewProjectionMatrix();
        
        std::cout << "Render: About to map uniform buffer" << std::endl;
        
        // Map the buffer and write data
        MapHelper<float4x4> CBConstants(m_pImmediateContext, m_pVSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
        *CBConstants = viewProjMatrix.Transpose(); // Transpose for HLSL column-major matrices
        
        std::cout << "Render: Uniform buffer updated" << std::endl;
    }

    std::cout << "Render: About to render voxel world" << std::endl;
    
    // Render voxel world
    RenderVoxelWorld();
    
    // Apply post-processing effects (SSAO, Bloom, tone mapping)
    if (m_pAdvancedRenderer)
    {
        m_pAdvancedRenderer->ApplyPostProcessing();
    }
    
    // End advanced rendering frame (tone mapping to back buffer)
    if (m_pAdvancedRenderer)
    {
        m_pAdvancedRenderer->EndFrame();
    }
    
    std::cout << "Render: About to show camera debug info" << std::endl;
    
    // Show camera debug info in ImGui window
    RenderImGuiDebugWindow();
    
    std::cout << "Render: Completed" << std::endl;
}

void ForgedFlightApp::RenderVoxelWorld()
{
    if (m_pChunkManager && m_pVoxelWorld && m_pCamera)
    {
        m_pChunkManager->RenderChunks(m_pVoxelWorld.get(), m_pCamera.get(), m_pCubePSO, m_pSRB);
    }
}

void ForgedFlightApp::Present()
{
    m_pSwapChain->Present();
}

void ForgedFlightApp::WindowResize(Uint32 Width, Uint32 Height)
{
    if (m_pSwapChain)
    {
        m_pSwapChain->Resize(Width, Height);
        m_WindowWidth = Width;
        m_WindowHeight = Height;
        
        if (m_pCamera)
        {
            m_pCamera->SetPerspective(45.0f, static_cast<float>(Width) / Height, 0.1f, 1000.0f);
        }
        
        // Update ImGui DisplaySize when window resizes
        if (m_pImGuiImpl)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize.x = static_cast<float>(Width);
            io.DisplaySize.y = static_cast<float>(Height);
            std::cout << "Updated ImGui DisplaySize to: " << Width << "x" << Height << std::endl;
        }
    }
}

void ForgedFlightApp::OnKeyDown(UINT8 key)
{
    // Forward key down to ImGui first
    if (m_pImGuiImpl)
    {
        ImGuiIO& io = ImGui::GetIO();
        
        // Handle special keys
        if (key == VK_CONTROL) io.KeyCtrl = true;
        if (key == VK_SHIFT) io.KeyShift = true;
        if (key == VK_MENU) io.KeyAlt = true;
        if (key == VK_LWIN || key == VK_RWIN) io.KeySuper = true;
        
        // Add character input for text fields
        if (key >= 32 && key < 127)
        {
            io.AddInputCharacter(key);
        }
    }

    // Only handle camera movement if ImGui doesn't want keyboard input
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
        m_KeyStates[key] = true;
}

void ForgedFlightApp::OnKeyUp(UINT8 key)
{
    // Forward key up to ImGui first
    if (m_pImGuiImpl)
    {
        ImGuiIO& io = ImGui::GetIO();
        
        // Handle special keys
        if (key == VK_CONTROL) io.KeyCtrl = false;
        if (key == VK_SHIFT) io.KeyShift = false;
        if (key == VK_MENU) io.KeyAlt = false;
        if (key == VK_LWIN || key == VK_RWIN) io.KeySuper = false;
    }

    m_KeyStates[key] = false;
}

void ForgedFlightApp::OnMouseMove(int x, int y)
{
    // Forward mouse move to ImGui first
    if (m_pImGuiImpl)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos.x = static_cast<float>(x);
        io.MousePos.y = static_cast<float>(y);
    }

    if (m_FirstMouseMove)
    {
        m_LastMouseX = x;
        m_LastMouseY = y;
        m_FirstMouseMove = false;
        return;
    }
    
    int deltaX = x - m_LastMouseX;
    int deltaY = m_LastMouseY - y; // Reversed since y-coordinates go from bottom to top
    
    m_LastMouseX = x;
    m_LastMouseY = y;
    
    // Only handle camera rotation if ImGui doesn't want mouse input
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse && m_MouseButtons[1] && m_pCamera) // Right mouse button
    {
        m_pCamera->Rotate(static_cast<float>(deltaX), static_cast<float>(deltaY));
    }
}

void ForgedFlightApp::OnMouseDown(int x, int y, UINT button)
{
    // Forward mouse down to ImGui first
    if (m_pImGuiImpl && button < 3)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[button] = true;
    }

    // Only handle camera mouse input if ImGui doesn't want mouse input
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse && button < 3)
        m_MouseButtons[button] = true;
}

void ForgedFlightApp::OnMouseUp(int x, int y, UINT button)
{
    // Forward mouse up to ImGui first
    if (m_pImGuiImpl && button < 3)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[button] = false;
    }

    if (button < 3)
        m_MouseButtons[button] = false;
}

void ForgedFlightApp::OnMouseWheel(int delta)
{
    // Forward mouse wheel to ImGui first
    if (m_pImGuiImpl)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel += delta > 0 ? 1.0f : -1.0f;
    }

    // Only handle camera speed adjustment if ImGui doesn't want mouse input
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse && m_pCamera)
    {
        float currentSpeed = m_pCamera->GetMovementSpeed();
        float newSpeed = currentSpeed + (delta > 0 ? 2.0f : -2.0f);
        newSpeed = std::max(1.0f, std::min(50.0f, newSpeed));
        m_pCamera->SetMovementSpeed(newSpeed);
    }
}


void ForgedFlightApp::InitializeImGui()
{
    try 
    {
        std::cout << "Starting ImGui initialization" << std::endl;
        
        if (!m_pDevice || !m_pImmediateContext)
        {
            std::cout << "Error: Device or context is null" << std::endl;
            return;
        }
        
        // Create ImGui implementation
        ImGuiDiligentCreateInfo CreateInfo;
        CreateInfo.pDevice = m_pDevice;
        CreateInfo.BackBufferFmt = m_pSwapChain->GetDesc().ColorBufferFormat;
        CreateInfo.DepthBufferFmt = m_pSwapChain->GetDesc().DepthBufferFormat;
        
        std::cout << "ImGui CreateInfo set up with device and formats" << std::endl;
        
        m_pImGuiImpl = std::make_unique<ImGuiImplDiligent>(CreateInfo);
        
        std::cout << "ImGui implementation created successfully" << std::endl;
        
        // Set up ImGui style and ensure DisplaySize is set correctly
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(m_WindowWidth);
        io.DisplaySize.y = static_cast<float>(m_WindowHeight);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        std::cout << "ImGui IO configured with DisplaySize: " << io.DisplaySize.x << "x" << io.DisplaySize.y << std::endl;
    }
    catch (const std::exception& e)
    {
        std::string error = "Failed to initialize ImGui: ";
        error += e.what();
        std::cout << error << std::endl;
        MessageBoxA(nullptr, error.c_str(), "ImGui Error", MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        std::cout << "Unknown error during ImGui initialization" << std::endl;
        MessageBoxA(nullptr, "Unknown error during ImGui initialization", "ImGui Error", MB_OK | MB_ICONERROR);
    }
}

void ForgedFlightApp::RenderImGuiDebugWindow()
{
    if (!m_pImGuiImpl || !m_pCamera)
        return;
        
    // Begin ImGui frame
    m_pImGuiImpl->NewFrame(m_WindowWidth, m_WindowHeight, SURFACE_TRANSFORM_IDENTITY);
    
    // Create camera debug window with enhanced features
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Camera Debug Info", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::Text("=== CAMERA DEBUG INFO ===");
        ImGui::Separator();
        
        // Camera position with editable values
        float3 position = m_pCamera->GetPosition();
        float pos[3] = { position.x, position.y, position.z };
        if (ImGui::DragFloat3("Position", pos, 0.1f, -1000.0f, 1000.0f))
        {
            m_pCamera->SetPosition(float3(pos[0], pos[1], pos[2]));
        }
        
        // Camera rotation (pitch, yaw) with editable values
        float pitch = m_pCamera->GetPitch();
        float yaw = m_pCamera->GetYaw();
        
        bool rotationChanged = false;
        
        if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f, "%.1f°"))
        {
            rotationChanged = true;
        }
        
        if (ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f, "%.1f°"))
        {
            rotationChanged = true;
        }
        
        if (rotationChanged)
        {
            m_pCamera->SetRotation(yaw, pitch);
        }
        
        ImGui::Separator();
        
        // Camera direction vector (read-only)
        float3 forward = m_pCamera->GetForwardVector();
        ImGui::Text("Forward: (%.3f, %.3f, %.3f)", forward.x, forward.y, forward.z);
        
        // Movement speed with editable slider
        float speed = m_pCamera->GetMovementSpeed();
        if (ImGui::SliderFloat("Movement Speed", &speed, 1.0f, 50.0f, "%.1f"))
        {
            m_pCamera->SetMovementSpeed(speed);
        }
        
        ImGui::Separator();
        
        // Projection parameters with editable values
        float fov = m_pCamera->GetFOV();
        if (ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f, "%.1f°"))
        {
            float aspectRatio = m_pCamera->GetAspectRatio();
            float nearPlane = m_pCamera->GetNearPlane();
            float farPlane = m_pCamera->GetFarPlane();
            m_pCamera->SetPerspective(fov, aspectRatio, nearPlane, farPlane);
        }
        
        float aspectRatio = m_pCamera->GetAspectRatio();
        ImGui::Text("Aspect Ratio: %.3f", aspectRatio);
        
        float nearPlane = m_pCamera->GetNearPlane();
        float farPlane = m_pCamera->GetFarPlane();
        
        float planes[2] = { nearPlane, farPlane };
        if (ImGui::DragFloat2("Near/Far Planes", planes, 0.1f, 0.01f, 10000.0f))
        {
            float fov = m_pCamera->GetFOV();
            float aspectRatio = m_pCamera->GetAspectRatio();
            m_pCamera->SetPerspective(fov, aspectRatio, planes[0], planes[1]);
        }
        
        ImGui::Separator();
        
        // Reset button
        if (ImGui::Button("Reset Camera"))
        {
            m_pCamera->SetPosition(float3(0.0f, 50.0f, 0.0f));
            m_pCamera->SetRotation(0.0f, 0.0f); // yaw, pitch
            m_pCamera->SetMovementSpeed(10.0f);
        }
        
        ImGui::SameLine();
        
        // Quick position presets
        if (ImGui::Button("Ground Level"))
        {
            m_pCamera->SetPosition(float3(0.0f, 2.0f, 0.0f));
        }
        
        ImGui::Separator();
        ImGui::Text("Controls:");
        ImGui::BulletText("WASD - Move camera");
        ImGui::BulletText("Right Mouse - Look around");
        ImGui::BulletText("Mouse Wheel - Speed control");
        ImGui::BulletText("Drag values above to adjust camera");
        
        // Window size info
        ImGui::Separator();
        ImGui::Text("Window: %dx%d", m_WindowWidth, m_WindowHeight);
        
        // Frame rate info
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                   1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();
    
    // Render ImGui
    m_pImGuiImpl->Render(m_pImmediateContext);
}