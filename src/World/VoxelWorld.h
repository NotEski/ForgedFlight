#pragma once

#include "Chunk.h"
#include <unordered_map>
#include <memory>
#include <queue>
#include <vector>
#include <unordered_set>

struct ChunkCoordinate
{
    int x, y, z;
    
    ChunkCoordinate(int x, int y, int z) : x(x), y(y), z(z) {}
    
    bool operator==(const ChunkCoordinate& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator<(const ChunkCoordinate& other) const
    {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

// Hash function for ChunkCoordinate to use in unordered_set
struct ChunkCoordinateHash
{
    std::size_t operator()(const ChunkCoordinate& coord) const
    {
        return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.y) << 1) ^ (std::hash<int>()(coord.z) << 2);
    }
};

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
    
    // Access to loaded chunks for rendering
    const std::unordered_map<int64_t, std::unique_ptr<Chunk>>& GetLoadedChunks() const { return m_Chunks; }
    size_t GetChunkCount() const { return m_Chunks.size(); }
    
    // Chunk generation queue system
    void ProcessChunkQueue(int maxChunksPerFrame = 2);
    void QueueChunksAroundPlayer(const float3& playerPosition);
    void ClearChunkQueue();
    bool IsChunkQueued(int chunkX, int chunkY, int chunkZ) const;
    size_t GetQueueSize() const { return m_ChunkGenerationQueue.size(); }
    
    // Chunk deletion queue system
    void ProcessDeletionQueue(int maxChunksPerFrame = 1);
    void QueueChunksForDeletion(const float3& playerPosition);
    void ClearDeletionQueue();
    bool IsChunkQueuedForDeletion(int chunkX, int chunkY, int chunkZ) const;
    size_t GetDeletionQueueSize() const { return m_ChunkDeletionQueue.size(); }
    
    // Settings
    void SetRenderDistance(int distance) { m_RenderDistance = distance; }
    int GetRenderDistance() const { return m_RenderDistance; }

private:
    // Chunk storage
    std::unordered_map<int64_t, std::unique_ptr<Chunk>> m_Chunks;
    
    // Chunk generation queue system
    std::queue<ChunkCoordinate> m_ChunkGenerationQueue;
    std::unordered_set<ChunkCoordinate, ChunkCoordinateHash> m_QueuedChunks;
    
    // Chunk deletion queue system
    std::queue<ChunkCoordinate> m_ChunkDeletionQueue;
    std::unordered_set<ChunkCoordinate, ChunkCoordinateHash> m_QueuedForDeletion;
    
    int m_LastPlayerChunkX = INT_MAX;
    int m_LastPlayerChunkY = INT_MAX; 
    int m_LastPlayerChunkZ = INT_MAX;
    
    // World settings
    int m_RenderDistance = 16;  // Reduced default for better performance
    float3 m_LastPlayerPosition;
    
    // Helper methods
    int64_t GetChunkKey(int chunkX, int chunkY, int chunkZ) const;
    void GetChunkCoordinates(int worldX, int worldY, int worldZ, int& chunkX, int& chunkY, int& chunkZ, int& localX, int& localY, int& localZ) const;
};