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
    
    // This is a simplified approach - in a real implementation, you'd iterate through
    // the world's chunks and update only the dirty ones
    // For now, we'll assume the world manages chunk updates internally
}

void ChunkManager::CreateChunkBuffers(Chunk* chunk, ChunkRenderData& renderData)
{
    if (!chunk || !chunk->IsMeshBuilt())
        return;
    
    const auto& vertices = chunk->GetVertices();
    const auto& indices = chunk->GetIndices();
    
    if (vertices.empty() || indices.empty())
    {
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
}

int64_t ChunkManager::GetChunkKey(int chunkX, int chunkY, int chunkZ) const
{
    // Create a 64-bit key from 3D coordinates (21 bits each, with some overflow protection)
    return (static_cast<int64_t>(chunkX & 0x1FFFFF) << 42) | 
           (static_cast<int64_t>(chunkY & 0x1FFFFF) << 21) | 
           (static_cast<int64_t>(chunkZ & 0x1FFFFF));
}