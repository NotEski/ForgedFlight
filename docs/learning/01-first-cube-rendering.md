# First Cube Rendering with Diligent Engine

This tutorial walks through getting your first cube rendering in the Forged Flight voxel engine using Diligent Engine.

## Overview

Your application already has most of the pieces in place:
- Diligent Engine initialization ✅
- Vertex/Pixel shader pipeline ✅  
- Camera system with controls ✅
- Debug UI with ImGui ✅

The main issue is that the chunk system isn't properly creating GPU buffers for the generated voxel meshes.

## Current Architecture

```
ForgedFlightApp
├── Diligent Engine (Renderer)
├── Camera (Movement/View matrices)
├── VoxelWorld (Data management)
├── ChunkManager (GPU buffer management) ← **ISSUE HERE**
└── Chunks (Mesh generation) ✅
```

## The Problem

In `ChunkManager.cpp`, the `UpdateChunkBuffers()` method is empty:

```cpp
void ChunkManager::UpdateChunkBuffers(VoxelWorld* world)
{
    // This is simplified - not actually creating buffers!
}
```

## Solution Approaches

### Approach 1: Quick Single Cube Test (Recommended First)
Add a simple hardcoded cube to `ForgedFlightApp::Render()` to verify the pipeline works.

### Approach 2: Fix Chunk System Integration
Properly implement the connection between Chunk mesh generation and GPU buffer creation.

## Key Diligent Engine Concepts

### 1. Buffer Creation
```cpp
// Vertex buffer
BufferDesc vertexBufferDesc;
vertexBufferDesc.Usage = USAGE_IMMUTABLE;
vertexBufferDesc.BindFlags = BIND_VERTEX_BUFFER;
vertexBufferDesc.Size = vertices.size() * sizeof(float);

BufferData vertexData;
vertexData.pData = vertices.data();
vertexData.DataSize = vertexBufferDesc.Size;

device->CreateBuffer(vertexBufferDesc, &vertexData, &vertexBuffer);
```

### 2. Rendering Pipeline
```cpp
// 1. Set pipeline state
context->SetPipelineState(pso);

// 2. Bind resources (uniforms, textures)
context->CommitShaderResources(srb, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

// 3. Set vertex/index buffers
context->SetVertexBuffers(0, 1, vertexBuffers, nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
context->SetIndexBuffer(indexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

// 4. Draw
DrawIndexedAttribs drawAttrs;
drawAttrs.IndexType = VT_UINT32;
drawAttrs.NumIndices = indexCount;
context->DrawIndexed(drawAttrs);
```

### 3. Coordinate System Notes
- Diligent Engine uses right-handed coordinate system by default
- Your shaders expect Position + Normal + UV vertex format
- Matrix multiplication order: `ViewProjMatrix * Position`

## Troubleshooting Checklist

- [ ] Are chunks being generated? (Check VoxelWorld::LoadChunk)
- [ ] Are chunk meshes being built? (Check Chunk::BuildMesh)
- [ ] Are GPU buffers being created? (Check ChunkManager::CreateChunkBuffers)
- [ ] Are buffers being bound for rendering? (Check ChunkManager::RenderChunks)
- [ ] Are draw calls being issued? (Check DrawIndexedAttribs)
- [ ] Is the camera positioned correctly? (Check debug UI)

## Next Steps

1. **Start Simple**: Add a hardcoded cube render test
2. **Fix Integration**: Connect chunk mesh generation to GPU buffers  
3. **Optimize**: Add frustum culling, LOD, etc.

## Resources

- [Diligent Engine Tutorial 03 - Texturing](https://github.com/DiligentGraphics/DiligentSamples/tree/master/Tutorials/Tutorial03_Texturing)
- [Buffer Management in Diligent Engine](https://github.com/DiligentGraphics/DiligentEngine/blob/master/doc/BufferManagement.md)
- [Graphics Pipeline State Objects](https://github.com/DiligentGraphics/DiligentEngine/blob/master/doc/GraphicsPipelineStateObject.md)