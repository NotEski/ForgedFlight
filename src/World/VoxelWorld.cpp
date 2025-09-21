#include "VoxelWorld.h"
#include <cmath>

VoxelWorld::VoxelWorld()
    : m_LastPlayerPosition(0, 0, 0)
{
}

void VoxelWorld::Update(const float3& playerPosition)
{
    // Check if player has moved significantly
    float3 delta = playerPosition - m_LastPlayerPosition;
    float distance = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    
    if (distance > 16.0f) // Update chunks if player moved more than one chunk
    {
        m_LastPlayerPosition = playerPosition;
        
        int playerChunkX = static_cast<int>(std::floor(playerPosition.x / CHUNK_SIZE));
        int playerChunkY = static_cast<int>(std::floor(playerPosition.y / CHUNK_HEIGHT));
        int playerChunkZ = static_cast<int>(std::floor(playerPosition.z / CHUNK_SIZE));
        
        // Load chunks within render distance (cubic volume)
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
                    if (dx * dx + dy * dy + dz * dz <= m_RenderDistance * m_RenderDistance)
                    {
                        if (GetChunk(x, y, z) == nullptr)
                        {
                            LoadChunk(x, y, z);
                        }
                    }
                }
            }
        }
        
        // Unload chunks outside render distance
        auto it = m_Chunks.begin();
        while (it != m_Chunks.end())
        {
            int chunkX = it->second->GetChunkX();
            int chunkY = it->second->GetChunkY();
            int chunkZ = it->second->GetChunkZ();
            
            int dx = chunkX - playerChunkX;
            int dy = chunkY - playerChunkY;
            int dz = chunkZ - playerChunkZ;
            
            if (dx * dx + dy * dy + dz * dz > (m_RenderDistance + 2) * (m_RenderDistance + 2))
            {
                it = m_Chunks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
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
        chunk->BuildMesh();
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
    chunkX = static_cast<int>(std::floor(static_cast<float>(worldX) / CHUNK_SIZE));
    chunkY = static_cast<int>(std::floor(static_cast<float>(worldY) / CHUNK_HEIGHT));
    chunkZ = static_cast<int>(std::floor(static_cast<float>(worldZ) / CHUNK_SIZE));
    
    localX = worldX - chunkX * CHUNK_SIZE;
    localY = worldY - chunkY * CHUNK_HEIGHT;
    localZ = worldZ - chunkZ * CHUNK_SIZE;
    
    // Handle negative coordinates
    if (localX < 0)
    {
        localX += CHUNK_SIZE;
        chunkX--;
    }
    if (localY < 0)
    {
        localY += CHUNK_HEIGHT;
        chunkY--;
    }
    if (localZ < 0)
    {
        localZ += CHUNK_SIZE;
        chunkZ--;
    }
}