#include "ChunkManager.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"

ChunkManager::ChunkManager(IRenderDevice* device, IDeviceContext* context)
    : m_pDevice(device), m_pContext(context)
{
}

void ChunkManager::RenderChunks(VoxelWorld* world, Camera* camera, IPipelineState* pso, IShaderResourceBinding* srb)
{
    if (!world || !camera || !pso || !srb)
        return;
    
    // Set pipeline state
    m_pContext->SetPipelineState(pso);
    m_pContext->CommitShaderResources(srb, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    
    // Render all loaded chunks
    for (auto& [key, renderData] : m_ChunkRenderData)
    {
        if (renderData.IndexCount == 0 || !renderData.VertexBuffer || !renderData.IndexBuffer)
            continue;
            
        // Set vertex and index buffers
        IBuffer* vertexBuffers[] = { renderData.VertexBuffer };
        m_pContext->SetVertexBuffers(0, 1, vertexBuffers, nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        m_pContext->SetIndexBuffer(renderData.IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        
        // Draw indexed
        DrawIndexedAttribs drawAttrs;
        drawAttrs.IndexType = VT_UINT32;
        drawAttrs.NumIndices = static_cast<Uint32>(renderData.IndexCount);
        m_pContext->DrawIndexed(drawAttrs);
    }
}

void ChunkManager::UpdateChunkBuffers(VoxelWorld* world)
{
    if (!world)
        return;
    
    std::cout << "UpdateChunkBuffers: Starting update..." << std::endl;
    
    // Get the loaded chunks from the voxel world
    const auto& loadedChunks = world->GetLoadedChunks();
    
    std::cout << "UpdateChunkBuffers: Found " << loadedChunks.size() << " loaded chunks" << std::endl;
    
    for (const auto& [chunkKey, chunk] : loadedChunks)
    {
        if (!chunk)
            continue;
        
        int chunkX = chunk->GetChunkX();
        int chunkY = chunk->GetChunkY();
        int chunkZ = chunk->GetChunkZ();
        
        std::cout << "UpdateChunkBuffers: Processing chunk (" << chunkX << ", " << chunkY << ", " << chunkZ << ")" << std::endl;
        
        // Generate mesh if needed
        if (chunk->IsDirty())
        {
            std::cout << "UpdateChunkBuffers: Chunk is dirty, building mesh..." << std::endl;
            chunk->BuildMesh();
        }
        
        // Get or create render data for this chunk
        ChunkRenderData& renderData = m_ChunkRenderData[chunkKey];
        
        // Create GPU buffers if mesh was built and we need to update
        if (chunk->IsMeshBuilt() && (renderData.NeedsUpdate || !renderData.VertexBuffer))
        {
            std::cout << "UpdateChunkBuffers: Creating buffers for chunk (" << chunkX << ", " << chunkY << ", " << chunkZ << ")" << std::endl;
            CreateChunkBuffers(chunk.get(), renderData);
        }
        else
        {
            std::cout << "UpdateChunkBuffers: Chunk (" << chunkX << ", " << chunkY << ", " << chunkZ << ") - MeshBuilt: " << chunk->IsMeshBuilt() << ", NeedsUpdate: " << renderData.NeedsUpdate << ", HasVertexBuffer: " << (renderData.VertexBuffer != nullptr) << std::endl;
        }
    }
    
    std::cout << "UpdateChunkBuffers: Completed. Total render data entries: " << m_ChunkRenderData.size() << std::endl;
}

void ChunkManager::CreateChunkBuffers(Chunk* chunk, ChunkRenderData& renderData)
{
    if (!chunk || !chunk->IsMeshBuilt())
    {
        std::cout << "CreateChunkBuffers: Early exit - chunk null or mesh not built" << std::endl;
        return;
    }
    
    const auto& vertices = chunk->GetVertices();
    const auto& indices = chunk->GetIndices();
    
    std::cout << "CreateChunkBuffers: Vertices: " << vertices.size() << ", Indices: " << indices.size() << std::endl;
    
    if (vertices.empty() || indices.empty())
    {
        std::cout << "CreateChunkBuffers: Empty mesh data - no geometry to render" << std::endl;
        renderData.IndexCount = 0;
        return;
    }
    
    // Create vertex buffer
    BufferDesc vertexBufferDesc;
    vertexBufferDesc.Name = "Chunk vertex buffer";
    vertexBufferDesc.Usage = USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = BIND_VERTEX_BUFFER;
    vertexBufferDesc.Size = vertices.size() * sizeof(float);
    
    BufferData vertexData;
    vertexData.pData = vertices.data();
    vertexData.DataSize = vertexBufferDesc.Size;
    
    m_pDevice->CreateBuffer(vertexBufferDesc, &vertexData, &renderData.VertexBuffer);
    
    // Create index buffer
    BufferDesc indexBufferDesc;
    indexBufferDesc.Name = "Chunk index buffer";
    indexBufferDesc.Usage = USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = BIND_INDEX_BUFFER;
    indexBufferDesc.Size = indices.size() * sizeof(uint32_t);
    
    BufferData indexData;
    indexData.pData = indices.data();
    indexData.DataSize = indexBufferDesc.Size;
    
    m_pDevice->CreateBuffer(indexBufferDesc, &indexData, &renderData.IndexBuffer);
    
    renderData.IndexCount = indices.size();
    renderData.NeedsUpdate = false;
    
    std::cout << "CreateChunkBuffers: Successfully created buffers with " << renderData.IndexCount << " indices" << std::endl;
}

int64_t ChunkManager::GetChunkKey(int chunkX, int chunkY, int chunkZ) const
{
    // Create a 64-bit key from 3D coordinates (21 bits each, with some overflow protection)
    return (static_cast<int64_t>(chunkX & 0x1FFFFF) << 42) | 
           (static_cast<int64_t>(chunkY & 0x1FFFFF) << 21) | 
           (static_cast<int64_t>(chunkZ & 0x1FFFFF));
}