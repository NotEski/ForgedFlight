#pragma once

#include "Chunk.h"
#include <unordered_map>
#include <memory>

class VoxelWorld
{
public:
    VoxelWorld();
    ~VoxelWorld() = default;
    
    // World management
    void Update(const float3& playerPosition);
    void Render();
    
    // Block access
    Block GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, BlockType type);
    
    // Chunk management
    Chunk* GetChunk(int chunkX, int chunkY, int chunkZ) const;
    void LoadChunk(int chunkX, int chunkY, int chunkZ);
    void UnloadChunk(int chunkX, int chunkY, int chunkZ);
    
    // Settings
    void SetRenderDistance(int distance) { m_RenderDistance = distance; }
    int GetRenderDistance() const { return m_RenderDistance; }

private:
    // Chunk storage
    std::unordered_map<int64_t, std::unique_ptr<Chunk>> m_Chunks;
    
    // World settings
    int m_RenderDistance = 8;
    float3 m_LastPlayerPosition;
    
    // Helper methods
    int64_t GetChunkKey(int chunkX, int chunkY, int chunkZ) const;
    void GetChunkCoordinates(int worldX, int worldY, int worldZ, int& chunkX, int& chunkY, int& chunkZ, int& localX, int& localY, int& localZ) const;
};