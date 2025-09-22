# Chapter 2: Graphics Pipeline and Basic Rendering

## Overview
This chapter covers setting up the graphics rendering pipeline, creating shaders, and rendering your first voxel cube. We'll establish the foundation for the mesh-based voxel rendering system.

## Learning Objectives
By the end of this chapter, you will understand:
- Diligent Engine pipeline state creation
- Vertex and index buffer management
- Basic HLSL shader programming
- Matrix transformations for 3D rendering
- Uniform/constant buffer usage

## Key Concepts

### 1. Graphics Pipeline Overview

Modern graphics pipelines follow this flow:
```
Vertex Data → Vertex Shader → Rasterization → Pixel Shader → Frame Buffer
```

For voxel rendering, we need:
- **Vertex Shader**: Transform local coordinates to screen space
- **Pixel Shader**: Color each pixel of cube faces
- **Constant Buffer**: Store view/projection matrices
- **Vertex Buffer**: Store cube vertex positions, normals, UVs
- **Index Buffer**: Define triangle indices for cube faces

### 2. Voxel Cube Representation

A single voxel cube requires:
- **8 vertices** (cube corners)
- **6 faces** (front, back, left, right, top, bottom)
- **12 triangles** (2 triangles per face)
- **36 indices** (3 indices per triangle)

Each vertex contains:
```cpp
struct Vertex {
    float3 Position;  // 3D coordinate
    float3 Normal;    // Face normal for lighting
    float2 UV;        // Texture coordinates
};
```

### 3. Matrix Transformations

The rendering pipeline requires three matrices:
- **Model Matrix**: Local to world transformation (per object)
- **View Matrix**: World to camera transformation
- **Projection Matrix**: Camera to screen transformation

Combined: `MVP = Projection × View × Model`

## Implementation Walkthrough

### Step 1: Shader Creation

**Vertex Shader (Cube.vsh):**
```hlsl
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
    float3 WorldPos : WORLDPOS;
};

PSInput main(VSInput input)
{
    PSInput output;
    
    // Transform to world space (identity for now)
    float4 worldPos = float4(input.Pos, 1.0);
    
    // Transform to clip space
    output.Pos = mul(worldPos, ViewProjMatrix);
    output.WorldPos = worldPos.xyz;
    output.Normal = input.Normal;
    output.UV = input.UV;
    
    return output;
}
```

**Pixel Shader (Cube.psh):**
```hlsl
struct PSInput
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 WorldPos : WORLDPOS;
};

float4 main(PSInput input) : SV_TARGET
{
    // Simple face-based coloring for now
    float3 absNormal = abs(input.Normal);
    float3 color = absNormal; // Use normal as color
    
    // Add some lighting-like effect
    float3 lightDir = normalize(float3(1, 1, 1));
    float ndotl = max(0.0, dot(input.Normal, lightDir));
    color *= 0.5 + 0.5 * ndotl;
    
    return float4(color, 1.0);
}
```

### Step 2: Pipeline State Creation

**Pipeline State Object (PSO):**
```cpp
void ForgedFlightApp::CreateCubePipelineState()
{
    // Create shader source factory
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pDevice->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);

    // Create vertex shader
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.UseCombinedTextureSamplers = true;
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.Desc.Name = "Cube vertex shader";
        ShaderCI.Source = VSSource; // HLSL string
        m_pDevice->CreateShader(ShaderCI, &pVS);
    }

    // Create pixel shader (similar pattern)
    RefCntAutoPtr<IShader> pPS;
    // ... pixel shader creation

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
    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);
    // ... additional setup
    
    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pCubePSO);
}
```

### Step 3: Vertex and Index Buffers

**Cube Geometry Data:**
```cpp
// Cube vertices (position, normal, UV)
float CubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
    // ... 5 more faces
};

uint32_t CubeIndices[] = {
    // Front face
    0, 1, 2,  2, 3, 0,
    // Back face
    4, 5, 6,  6, 7, 4,
    // ... 4 more faces
};
```

**Buffer Creation:**
```cpp
void ForgedFlightApp::CreateVertexBuffer()
{
    BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "Cube vertex buffer";
    VertBuffDesc.Usage = USAGE_IMMUTABLE;
    VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    VertBuffDesc.Size = sizeof(CubeVertices);
    
    BufferData VBData;
    VBData.pData = CubeVertices;
    VBData.DataSize = sizeof(CubeVertices);
    
    m_pDevice->CreateBuffer(VertBuffDesc, &VBData, &m_pCubeVertexBuffer);
}
```

### Step 4: Constant Buffer for Matrices

**Constant Buffer Structure:**
```cpp
struct VSConstants
{
    float4x4 ViewProjMatrix;
};

void ForgedFlightApp::CreateUniformBuffer()
{
    BufferDesc CBDesc;
    CBDesc.Name = "VS constants CB";
    CBDesc.Size = sizeof(VSConstants);
    CBDesc.Usage = USAGE_DYNAMIC;
    CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
    CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    
    m_pDevice->CreateBuffer(CBDesc, nullptr, &m_pVSConstants);
}
```

### Step 5: Rendering Loop

**Frame Rendering:**
```cpp
void ForgedFlightApp::Render()
{
    // Clear render target
    float4 ClearColor = float4(0.2f, 0.2f, 0.3f, 1.0f);
    auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor.Data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Update constant buffer
    {
        MapHelper<VSConstants> CBConstants(m_pImmediateContext, m_pVSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
        CBConstants->ViewProjMatrix = m_pCamera->GetViewProjectionMatrix().Transpose();
    }

    // Set pipeline state
    m_pImmediateContext->SetPipelineState(m_pCubePSO);
    
    // Bind vertex buffer
    IBuffer* pBuffs[] = {m_pCubeVertexBuffer};
    Uint64 offsets[] = {0};
    m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    
    // Bind index buffer
    m_pImmediateContext->SetIndexBuffer(m_pCubeIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    
    // Bind resources
    m_pImmediateContext->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    
    // Draw
    DrawIndexedAttribs DrawAttrs;
    DrawAttrs.IndexType = VT_UINT32;
    DrawAttrs.NumIndices = 36;
    DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
    m_pImmediateContext->DrawIndexed(DrawAttrs);
}
```

## Factory System Considerations

### 1. **Instanced Rendering Foundation**
The basic cube rendering sets up for efficient factory machine rendering:
```cpp
// Future: Instance data for factory machines
struct InstanceData {
    float4x4 WorldMatrix;
    float4   ColorTint;
    uint32_t MachineType;
};
```

### 2. **Shader Flexibility**
The pipeline supports complex factory visuals:
```hlsl
// Future: Factory machine shader variations
#define MACHINE_TYPE_ASSEMBLER 1
#define MACHINE_TYPE_CONVEYOR  2
// etc.
```

### 3. **Resource Management**
Buffer management patterns scale to factory complexity:
```cpp
// Future: Dynamic buffer allocation for factory chunks
std::vector<RefCntAutoPtr<IBuffer>> m_FactoryBuffers;
```

## Common Issues and Solutions

### Issue 1: No Rendering Output
**Symptoms:** Black screen, no cube visible
**Solution:** Check matrix calculations, vertex data, clear color

### Issue 2: Cube Appears Distorted
**Symptoms:** Stretched or skewed cube
**Solution:** Verify aspect ratio in projection matrix

### Issue 3: Shader Compilation Errors
**Symptoms:** PSO creation fails
**Solution:** Check HLSL syntax, semantic names, constant buffer layout

### Issue 4: Z-Fighting or Depth Issues
**Symptoms:** Faces flickering or missing
**Solution:** Enable depth testing, check near/far planes

## Testing and Validation

### Visual Tests
1. **Single Cube**: One colored cube visible at origin
2. **Rotation**: Cube faces show different colors as camera moves
3. **Depth**: Cube faces render in correct order
4. **Resizing**: Cube maintains proportions when window resizes

### Performance Targets
- **Draw Calls**: 1 per cube (baseline for optimization)
- **GPU Memory**: < 10MB for basic geometry
- **Frame Rate**: 60+ FPS with single cube

## Next Steps

Chapter 3 will cover:
- Camera system implementation
- Mouse and keyboard input handling
- Free-look camera controls
- View matrix calculations

## Key Resources

- [Diligent Engine Tutorial 02 - Cube](https://github.com/DiligentGraphics/DiligentEngine/tree/master/Tutorials/Tutorial02_Cube)
- [HLSL Language Reference](https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl)
- [3D Math Primer](https://gamemath.com/)
- [Real-Time Rendering](http://www.realtimerendering.com/)

## Chapter Summary

This chapter established the core graphics rendering pipeline:
- **Shader Programming**: HLSL vertex and pixel shaders for cube rendering
- **Resource Management**: Vertex, index, and constant buffers
- **Pipeline State**: Complete rendering state configuration
- **Matrix Math**: 3D transformations for positioning and viewing

The rendering foundation now supports displaying 3D geometry and sets the stage for complex voxel world rendering in subsequent chapters.