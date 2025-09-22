#include "VoxelWorld.h"
#include <cmath>
#include <algorithm>

VoxelWorld::VoxelWorld()
    : m_LastPlayerPosition(0, 0, 0), m_RenderDistance(16), 
      m_LastPlayerChunkX(INT_MAX), m_LastPlayerChunkY(INT_MAX), m_LastPlayerChunkZ(INT_MAX)
{
}

void VoxelWorld::Update(const float3& playerPosition)
{
    // Calculate current player chunk position
    int playerChunkX = static_cast<int>(std::floor(playerPosition.x / CHUNK_X_SIZE));
    int playerChunkY = static_cast<int>(std::floor(playerPosition.y / CHUNK_Y_SIZE));
    int playerChunkZ = static_cast<int>(std::floor(playerPosition.z / CHUNK_Z_SIZE));
    
    // Check if player has moved to a new chunk
    if (playerChunkX != m_LastPlayerChunkX || 
        playerChunkY != m_LastPlayerChunkY || 
        playerChunkZ != m_LastPlayerChunkZ)
    {
        // Player moved to a new chunk - queue all chunks around new position
        m_LastPlayerChunkX = playerChunkX;
        m_LastPlayerChunkY = playerChunkY;
        m_LastPlayerChunkZ = playerChunkZ;
        m_LastPlayerPosition = playerPosition;
        
        QueueChunksAroundPlayer(playerPosition);
        QueueChunksForDeletion(playerPosition);
    }
    
    // Process queues each frame for smooth performance
    ProcessChunkQueue(2); // Process 2 chunks per frame for generation
    ProcessDeletionQueue(1); // Process 1 chunk per frame for deletion
}

Block VoxelWorld::GetBlock(int x, int y, int z) const
{
    int chunkX, chunkY, chunkZ, localX, localY, localZ;
    GetChunkCoordinates(x, y, z, chunkX, chunkY, chunkZ, localX, localY, localZ);
    
    Chunk* chunk = GetChunk(chunkX, chunkY, chunkZ);
    if (chunk == nullptr)
        return Block{}; // Return air if chunk doesn't exist
    
    return chunk->GetBlock(localX, localY, localZ);
}

void VoxelWorld::SetBlock(int x, int y, int z, BlockType type)
{
    int chunkX, chunkY, chunkZ, localX, localY, localZ;
    GetChunkCoordinates(x, y, z, chunkX, chunkY, chunkZ, localX, localY, localZ);
    
    Chunk* chunk = GetChunk(chunkX, chunkY, chunkZ);
    if (chunk != nullptr)
    {
        chunk->SetBlock(localX, localY, localZ, type);
    }
}

Chunk* VoxelWorld::GetChunk(int chunkX, int chunkY, int chunkZ) const
{
    int64_t key = GetChunkKey(chunkX, chunkY, chunkZ);
    auto it = m_Chunks.find(key);
    return (it != m_Chunks.end()) ? it->second.get() : nullptr;
}

void VoxelWorld::LoadChunk(int chunkX, int chunkY, int chunkZ)
{
    int64_t key = GetChunkKey(chunkX, chunkY, chunkZ);
    if (m_Chunks.find(key) == m_Chunks.end())
    {
        auto chunk = std::make_unique<Chunk>(chunkX, chunkY, chunkZ);
        chunk->Generate();
        chunk->BuildMesh(this);
        m_Chunks[key] = std::move(chunk);
    }
}

void VoxelWorld::UnloadChunk(int chunkX, int chunkY, int chunkZ)
{
    int64_t key = GetChunkKey(chunkX, chunkY, chunkZ);
    m_Chunks.erase(key);
}

int64_t VoxelWorld::GetChunkKey(int chunkX, int chunkY, int chunkZ) const
{
    // Create a 64-bit key from 3D coordinates (21 bits each, with some overflow protection)
    return (static_cast<int64_t>(chunkX & 0x1FFFFF) << 42) | 
           (static_cast<int64_t>(chunkY & 0x1FFFFF) << 21) | 
           (static_cast<int64_t>(chunkZ & 0x1FFFFF));
}

void VoxelWorld::GetChunkCoordinates(int worldX, int worldY, int worldZ, int& chunkX, int& chunkY, int& chunkZ, int& localX, int& localY, int& localZ) const
{
    chunkX = static_cast<int>(std::floor(static_cast<float>(worldX) / CHUNK_X_SIZE));
    chunkY = static_cast<int>(std::floor(static_cast<float>(worldY) / CHUNK_Y_SIZE));
    chunkZ = static_cast<int>(std::floor(static_cast<float>(worldZ) / CHUNK_Z_SIZE));

    localX = worldX - chunkX * CHUNK_X_SIZE;
    localY = worldY - chunkY * CHUNK_Y_SIZE;
    localZ = worldZ - chunkZ * CHUNK_Z_SIZE;

    // Handle negative coordinates
    if (localX < 0)
    {
        localX += CHUNK_X_SIZE;
        chunkX--;
    }
    if (localY < 0)
    {
        localY += CHUNK_Y_SIZE;
        chunkY--;
    }
    if (localZ < 0)
    {
        localZ += CHUNK_Z_SIZE;
        chunkZ--;
    }
}

void VoxelWorld::QueueChunksAroundPlayer(const float3& playerPosition)
{
    int playerChunkX = static_cast<int>(std::floor(playerPosition.x / CHUNK_X_SIZE));
    int playerChunkY = static_cast<int>(std::floor(playerPosition.y / CHUNK_Y_SIZE));
    int playerChunkZ = static_cast<int>(std::floor(playerPosition.z / CHUNK_Z_SIZE));
    
    // Clear existing queue since we have a new target position
    ClearChunkQueue();
    
    // Create a list of chunks to generate, sorted by distance from player
    std::vector<std::pair<float, ChunkCoordinate>> chunksToQueue;
    
    // Calculate all chunks within render distance
    for (int x = playerChunkX - m_RenderDistance; x <= playerChunkX + m_RenderDistance; ++x)
    {
        for (int y = playerChunkY - m_RenderDistance; y <= playerChunkY + m_RenderDistance; ++y)
        {
            for (int z = playerChunkZ - m_RenderDistance; z <= playerChunkZ + m_RenderDistance; ++z)
            {
                // Check if chunk is within spherical render distance
                int dx = x - playerChunkX;
                int dy = y - playerChunkY;
                int dz = z - playerChunkZ;
                float distanceSquared = static_cast<float>(dx * dx + dy * dy + dz * dz);
                
                if (distanceSquared <= m_RenderDistance * m_RenderDistance)
                {
                    // Only queue if chunk doesn't exist yet
                    if (GetChunk(x, y, z) == nullptr)
                    {
                        chunksToQueue.emplace_back(distanceSquared, ChunkCoordinate(x, y, z));
                    }
                }
            }
        }
    }
    
    // Sort chunks by distance (closest first)
    std::sort(chunksToQueue.begin(), chunksToQueue.end());
    
    // Add sorted chunks to the generation queue
    for (const auto& pair : chunksToQueue)
    {
        const ChunkCoordinate& coord = pair.second;
        if (m_QueuedChunks.find(coord) == m_QueuedChunks.end())
        {
            m_ChunkGenerationQueue.push(coord);
            m_QueuedChunks.insert(coord);
        }
    }
}

void VoxelWorld::ProcessChunkQueue(int maxChunksPerFrame)
{
    int chunksProcessed = 0;
    
    while (!m_ChunkGenerationQueue.empty() && chunksProcessed < maxChunksPerFrame)
    {
        ChunkCoordinate coord = m_ChunkGenerationQueue.front();
        m_ChunkGenerationQueue.pop();
        m_QueuedChunks.erase(coord);
        
        // Double-check the chunk still doesn't exist
        if (GetChunk(coord.x, coord.y, coord.z) == nullptr)
        {
            LoadChunk(coord.x, coord.y, coord.z);
            chunksProcessed++;
        }
    }
}

void VoxelWorld::ClearChunkQueue()
{
    // Clear the queue
    std::queue<ChunkCoordinate> empty;
    m_ChunkGenerationQueue.swap(empty);
    
    // Clear the set
    m_QueuedChunks.clear();
}

bool VoxelWorld::IsChunkQueued(int chunkX, int chunkY, int chunkZ) const
{
    return m_QueuedChunks.find(ChunkCoordinate(chunkX, chunkY, chunkZ)) != m_QueuedChunks.end();
}

void VoxelWorld::QueueChunksForDeletion(const float3& playerPosition)
{
    int playerChunkX = static_cast<int>(std::floor(playerPosition.x / CHUNK_X_SIZE));
    int playerChunkY = static_cast<int>(std::floor(playerPosition.y / CHUNK_Y_SIZE));
    int playerChunkZ = static_cast<int>(std::floor(playerPosition.z / CHUNK_Z_SIZE));
    
    // Clear existing deletion queue since we have a new target position
    ClearDeletionQueue();
    
    // Create a list of chunks to delete, sorted by distance from player (farthest first)
    std::vector<std::pair<float, ChunkCoordinate>> chunksToDelete;
    
    // Check all loaded chunks to see which ones are outside render distance
    for (const auto& [chunkKey, chunk] : m_Chunks)
    {
        if (!chunk) continue;
        
        int chunkX = chunk->GetChunkX();
        int chunkY = chunk->GetChunkY();
        int chunkZ = chunk->GetChunkZ();
        
        // Calculate distance from player
        int dx = chunkX - playerChunkX;
        int dy = chunkY - playerChunkY;
        int dz = chunkZ - playerChunkZ;
        float distanceSquared = static_cast<float>(dx * dx + dy * dy + dz * dz);
        
        // Queue for deletion if outside render distance (with buffer)
        float deletionDistanceSquared = (m_RenderDistance + 2) * (m_RenderDistance + 2);
        if (distanceSquared > deletionDistanceSquared)
        {
            chunksToDelete.emplace_back(distanceSquared, ChunkCoordinate(chunkX, chunkY, chunkZ));
        }
    }
    
    // Sort chunks by distance (farthest first for deletion)
    std::sort(chunksToDelete.begin(), chunksToDelete.end(), std::greater<std::pair<float, ChunkCoordinate>>());
    
    // Add sorted chunks to the deletion queue
    for (const auto& pair : chunksToDelete)
    {
        const ChunkCoordinate& coord = pair.second;
        if (m_QueuedForDeletion.find(coord) == m_QueuedForDeletion.end())
        {
            m_ChunkDeletionQueue.push(coord);
            m_QueuedForDeletion.insert(coord);
        }
    }
}

void VoxelWorld::ProcessDeletionQueue(int maxChunksPerFrame)
{
    int chunksProcessed = 0;
    
    while (!m_ChunkDeletionQueue.empty() && chunksProcessed < maxChunksPerFrame)
    {
        ChunkCoordinate coord = m_ChunkDeletionQueue.front();
        m_ChunkDeletionQueue.pop();
        m_QueuedForDeletion.erase(coord);
        
        // Double-check the chunk still exists and is still outside render distance
        Chunk* chunk = GetChunk(coord.x, coord.y, coord.z);
        if (chunk != nullptr)
        {
            // Calculate current distance from player
            int dx = coord.x - m_LastPlayerChunkX;
            int dy = coord.y - m_LastPlayerChunkY;
            int dz = coord.z - m_LastPlayerChunkZ;
            float distanceSquared = static_cast<float>(dx * dx + dy * dy + dz * dz);
            
            // Only delete if still outside render distance
            float deletionDistanceSquared = (m_RenderDistance + 2) * (m_RenderDistance + 2);
            if (distanceSquared > deletionDistanceSquared)
            {
                UnloadChunk(coord.x, coord.y, coord.z);
                chunksProcessed++;
            }
        }
    }
}

void VoxelWorld::ClearDeletionQueue()
{
    // Clear the queue
    std::queue<ChunkCoordinate> empty;
    m_ChunkDeletionQueue.swap(empty);
    
    // Clear the set
    m_QueuedForDeletion.clear();
}

bool VoxelWorld::IsChunkQueuedForDeletion(int chunkX, int chunkY, int chunkZ) const
{
    return m_QueuedForDeletion.find(ChunkCoordinate(chunkX, chunkY, chunkZ)) != m_QueuedForDeletion.end();
}