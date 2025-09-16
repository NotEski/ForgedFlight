# Getting Started with Diligent Engine for Voxel Development

**Difficulty:** 🟢 Beginner  
**Estimated Time:** 4-6 hours  
**Prerequisites:** Basic C++ knowledge, understanding of 3D coordinate systems  
**Goal:** Set up Diligent Engine and render your first triangle

## 📖 Overview

This tutorial will guide you through setting up Diligent Engine in your voxel game project and understanding the basic rendering pipeline. You'll learn how to create a window, initialize the graphics device, and render your first geometry - the foundation for all voxel rendering.

## 🎯 What You'll Learn

- How to integrate Diligent Engine using CMake and git submodules
- Understanding the Device, Context, and SwapChain architecture
- Creating and using shaders (vertex and pixel shaders)
- Pipeline State Objects (PSO) and their role in rendering
- The basic render loop for real-time graphics

## 🔧 Project Setup

### Step 1: Initialize Git Submodules

First, let's add Diligent Engine as a git submodule to your project:

```bash
# Navigate to your project root
cd /path/to/forgedflight

# Add Diligent Engine as a submodule
git submodule add https://github.com/DiligentGraphics/DiligentEngine.git DiligentEngine

# Initialize and update submodules (this will take a few minutes)
git submodule update --init --recursive
```

### Step 2: CMake Integration

Create a basic `CMakeLists.txt` in your project root:

```cmake
cmake_minimum_required(VERSION 3.20)
project(ForgedFlight)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Diligent Engine
add_subdirectory(DiligentEngine)

# Create your executable
add_executable(ForgedFlight
    src/main.cpp
    src/VoxelApp.cpp
    src/VoxelApp.h
)

# Link against Diligent Engine
target_link_libraries(ForgedFlight
    PRIVATE
    Diligent-NativeApp
    Diligent-TextureLoader
    Diligent-Imgui
)

target_include_directories(ForgedFlight
    PRIVATE
    DiligentEngine/DiligentCore
    DiligentEngine/DiligentTools
)
```

### Step 3: Basic Application Structure

Create `src/VoxelApp.h`:

```cpp
#pragma once

#include "NativeApp.h"
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"

using namespace Diligent;

class VoxelApp final : public NativeApp
{
public:
    virtual void Initialize(const NativeAppInitAttribs& InitAttribs) override final;
    virtual void Render() override final;
    virtual void Update(double CurrTime, double ElapsedTime) override final;
    virtual const Char* GetAppTitle() const override final { return "Forged Flight - Voxel Engine"; }

private:
    void CreatePipelineState();
    void CreateVertexBuffer();
    
    RefCntAutoPtr<IPipelineState>        m_pPSO;
    RefCntAutoPtr<IBuffer>              m_pVertexBuffer;
    RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
};
```

## 📐 Understanding the Rendering Pipeline

### The Big Picture

Diligent Engine follows a modern graphics API design:

```
[CPU] Application Code
  ↓
[CPU] Diligent Engine Abstraction Layer  
  ↓
[GPU] Platform-Specific API (D3D11/12, Vulkan, Metal, OpenGL)
  ↓  
[GPU] Graphics Hardware
```

### Key Objects

1. **RenderDevice** - Factory for creating graphics resources
2. **DeviceContext** - Command submission and state management  
3. **SwapChain** - Display surface and back buffer management
4. **PipelineState** - Complete GPU pipeline configuration
5. **Buffer** - GPU memory for vertex, index, uniform data

## 🎨 Your First Triangle

### Step 4: Initialize Diligent Engine

In `src/VoxelApp.cpp`:

```cpp
#include "VoxelApp.h"
#include "ShaderSourceStreamFactory.h"
#include "MapHelper.hpp"

void VoxelApp::Initialize(const NativeAppInitAttribs& InitAttribs)
{
    // Base class handles window creation and device initialization
    NativeApp::Initialize(InitAttribs);
    
    CreatePipelineState();
    CreateVertexBuffer();
}

void VoxelApp::CreatePipelineState()
{
    // Create shader source factory for loading shaders from strings/files
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);

    // Define vertex shader source (HLSL)
    const char* VSSource = R"(
        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float3 Color : COLOR; 
        };

        void main(in  uint    VertId : SV_VertexID,
                  out PSInput PSIn) 
        {
            float4 Pos[3];
            Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
            Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
            Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

            float3 Col[3];
            Col[0] = float3(1.0, 0.0, 0.0); // red
            Col[1] = float3(0.0, 1.0, 0.0); // green
            Col[2] = float3(0.0, 0.0, 1.0); // blue

            PSIn.Pos   = Pos[VertId];
            PSIn.Color = Col[VertId];
        }
    )";

    // Define pixel shader source (HLSL)
    const char* PSSource = R"(
        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float3 Color : COLOR; 
        };

        struct PSOutput
        { 
            float4 Color : SV_TARGET; 
        };

        void main(in  PSInput  PSIn,
                  out PSOutput PSOut)
        {
            PSOut.Color = float4(PSIn.Color.rgb, 1.0);
        }
    )";

    // Create shaders
    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.UseCombinedTextureSamplers = true;
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

    // Create vertex shader
    RefCntAutoPtr<IShader> pVS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
    ShaderCI.EntryPoint      = "main";
    ShaderCI.Desc.Name       = "Triangle vertex shader";
    ShaderCI.Source          = VSSource;
    m_pDevice->CreateShader(ShaderCI, &pVS);

    // Create pixel shader
    RefCntAutoPtr<IShader> pPS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
    ShaderCI.EntryPoint      = "main";
    ShaderCI.Desc.Name       = "Triangle pixel shader";
    ShaderCI.Source          = PSSource;
    m_pDevice->CreateShader(ShaderCI, &pPS);

    // Create pipeline state
    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // Set render targets format
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
    
    // Set primitive topology
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    
    // Disable culling for now
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
    
    // Disable depth testing for this simple example
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

    // Set shaders
    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    
    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
    
    // Create shader resource binding (even though we don't use resources yet)
    m_pPSO->CreateShaderResourceBinding(&m_pSRB, true);
}
```

### Step 5: Render Loop

```cpp
void VoxelApp::Render()
{
    // Clear the screen
    const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f}; 
    ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();
    
    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, 
                                          RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0,
                                          RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Set pipeline state
    m_pImmediateContext->SetPipelineState(m_pPSO);
    
    // Commit shader resources (none in this example, but good practice)
    m_pImmediateContext->CommitShaderResources(m_pSRB, 
                                              RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Draw the triangle (3 vertices, no vertex buffer needed - using SV_VertexID)
    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3;
    m_pImmediateContext->Draw(drawAttrs);
}

void VoxelApp::Update(double CurrTime, double ElapsedTime)
{
    // Nothing to update for static triangle
    // Later we'll add camera updates, input handling, etc.
}
```

### Step 6: Main Function

Create `src/main.cpp`:

```cpp
#include "VoxelApp.h"

int main()
{
    VoxelApp TheApp;
    
    NativeAppInitAttribs AppInitAttribs;
    AppInitAttribs.WindowTitle = "Forged Flight - Voxel Engine";
    AppInitAttribs.ScreenWidth = 1024;
    AppInitAttribs.ScreenHeight = 768;
    
    try
    {
        TheApp.Run(AppInitAttribs);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
        return -1;
    }
    
    return 0;
}
```

## 🏗️ Build and Run

```bash
# Create build directory
mkdir build && cd build

# Generate CMake files
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
cmake --build . --config Debug

# Run your first triangle!
./Debug/ForgedFlight
```

## 🔍 Understanding What Happened

### Shader Compilation
1. **HLSL Source** → Diligent Engine → **Platform-Specific Shaders**
2. **Automatic Translation:** HLSL to GLSL for OpenGL, native compilation for D3D/Vulkan
3. **Cross-Platform:** Same source code works on all supported platforms

### Pipeline State Object (PSO)
- **Immutable State:** Once created, PSO cannot be modified (for performance)
- **Complete Configuration:** Defines entire GPU pipeline from vertices to pixels
- **Caching:** GPU drivers can optimize based on complete state knowledge

### Render Loop Architecture
1. **Clear Targets:** Prepare frame buffer for rendering
2. **Set Pipeline:** Configure GPU for specific rendering operation
3. **Commit Resources:** Bind textures, buffers, samplers to pipeline
4. **Draw:** Submit geometry for rendering

## 🚀 Next Steps

### Immediate Enhancements
1. **Add vertex buffer** instead of using `SV_VertexID` trick
2. **Add uniform buffer** for transformation matrices
3. **Implement camera controls** for 3D navigation

### Towards Voxel Rendering
1. **Render a colored cube** instead of triangle (next tutorial)
2. **Add instancing** to render multiple cubes efficiently
3. **Implement chunk system** for organized voxel data

### Debug and Validation
- **Enable validation layers** in Debug builds
- **Check for errors** after each Diligent Engine call
- **Use debug output** to understand GPU state

## 💡 Key Concepts for Voxel Development

### Why This Matters for Voxels
- **Instancing Foundation:** This triangle will become thousands of voxel instances
- **Shader Flexibility:** HLSL shaders will handle complex voxel lighting and texturing
- **Pipeline Efficiency:** Understanding PSO is crucial for voxel rendering performance

### Performance Considerations
- **State Changes Are Expensive:** Minimize PSO switches
- **Batch Similar Geometry:** Group voxels by material/shader type
- **GPU-Friendly Data:** Organize voxel data for efficient GPU access

## 🐛 Common Issues & Solutions

### Build Problems
- **Submodule Issues:** `git submodule update --init --recursive`
- **CMake Errors:** Ensure CMake 3.20+ and compatible compiler
- **Missing Dependencies:** Install platform-specific graphics drivers

### Runtime Issues
- **Black Screen:** Check shader compilation errors in debug output
- **Crashes:** Enable validation layers and check for proper resource cleanup
- **Performance:** Profile early - graphics debugging tools are essential

---

**Next Tutorial:** [02 - 3D Cube and Buffer Management](./02-cube-and-buffers.md)  
**Related Resources:** [Diligent Engine Documentation](https://github.com/DiligentGraphics/DiligentEngine)  
**Practice Project:** Try modifying the triangle colors and vertex positions