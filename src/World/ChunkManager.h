#pragma once

#include "VoxelWorld.h"
#include "../Rendering/Camera.h"
#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/Buffer.h"
#include <unordered_map>

using namespace Diligent;

struct ChunkRenderData
{
    RefCntAutoPtr<IBuffer> VertexBuffer;
    RefCntAutoPtr<IBuffer> IndexBuffer;
    size_t IndexCount = 0;
    bool NeedsUpdate = true;
};

class ChunkManager
{
public:
    ChunkManager(IRenderDevice* device, IDeviceContext* context);
    ~ChunkManager() = default;
    
    void RenderChunks(VoxelWorld* world, Camera* camera, IPipelineState* pso, IShaderResourceBinding* srb);
    void UpdateChunkBuffers(VoxelWorld* world);
    
private:
    IRenderDevice* m_pDevice;
    IDeviceContext* m_pContext;
    
    std::unordered_map<int64_t, ChunkRenderData> m_ChunkRenderData;
    
    void CreateChunkBuffers(Chunk* chunk, ChunkRenderData& renderData);
    int64_t GetChunkKey(int chunkX, int chunkY, int chunkZ) const;
};