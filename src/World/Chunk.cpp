#include "Chunk.h"
#include "VoxelWorld.h"
#include <random>
#include <cmath>

Chunk::Chunk(int x, int y, int z)
    : m_ChunkX(x), m_ChunkY(y), m_ChunkZ(z)
{
    // Initialize all blocks to air
    for (int x = 0; x < CHUNK_X_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_Y_SIZE; ++y)
        {
            for (int z = 0; z < CHUNK_Z_SIZE; ++z)
            {
                m_Blocks[x][y][z].type = BlockType::Air;
            }
        }
    }
}

Block Chunk::GetBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_X_SIZE || y < 0 || y >= CHUNK_Y_SIZE || z < 0 || z >= CHUNK_Z_SIZE)
        return Block{}; // Return air for out-of-bounds
    
    return m_Blocks[x][y][z];
}

void Chunk::SetBlock(int x, int y, int z, BlockType type)
{
    if (x < 0 || x >= CHUNK_X_SIZE || y < 0 || y >= CHUNK_Y_SIZE || z < 0 || z >= CHUNK_Z_SIZE)
        return;
    
    m_Blocks[x][y][z].type = type;
    m_Dirty = true;
}

void Chunk::Generate()
{
    // Clear all blocks to air first
    for (int x = 0; x < CHUNK_X_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_Y_SIZE; ++y)
        {
            if (y == 0) // Bedrock layer
            {
                for (int z = 0; z < CHUNK_Z_SIZE; ++z)
                {
                    m_Blocks[x][y][z].type = BlockType::Stone;
                }
                continue;
            }
            for (int z = 0; z < CHUNK_Z_SIZE; ++z)
            {
                m_Blocks[x][y][z].type = BlockType::Air;
            }
        }
    }
    
    // For debugging: place blocks to see chunk positioning
    if (m_ChunkY == 0 && m_ChunkX == 0 && m_ChunkZ == 0)
    {
        // Origin chunk - place a stone block at (0,0,0)
        m_Blocks[0][0][0].type = BlockType::Stone;
    }
    m_Dirty = true;
}

void Chunk::BuildMesh(VoxelWorld* world)
{
    if (!m_Dirty)
        return;
    
    m_Vertices.clear();
    m_Indices.clear();
    
    uint32_t indexOffset = 0;
    
    for (int x = 0; x < CHUNK_X_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_Y_SIZE; ++y)
        {
            for (int z = 0; z < CHUNK_Z_SIZE; ++z)
            {
                Block block = m_Blocks[x][y][z];
                if (block.type == BlockType::Air)
                    continue;
                
                // Transform local chunk coordinates to world space
                float3 worldOffset = float3(
                    static_cast<float>(m_ChunkX * CHUNK_X_SIZE), 
                    static_cast<float>(m_ChunkY * CHUNK_Y_SIZE), 
                    static_cast<float>(m_ChunkZ * CHUNK_Z_SIZE)
                );
                float3 blockPos = worldOffset + float3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                
                // Check each face of the block - all faces use counter-clockwise winding
                if (ShouldRenderFace(x, y, z, x, y + 1, z, world)) // Top face
                {
                    AddFace(blockPos + float3(0, 1, 0), float3(0, 1, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x, y - 1, z, world)) // Bottom face
                {
                    AddFace(blockPos, float3(0, -1, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x + 1, y, z, world)) // Right face
                {
                    AddFace(blockPos + float3(1, 0, 0), float3(1, 0, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x - 1, y, z, world)) // Left face
                {
                    AddFace(blockPos, float3(-1, 0, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x, y, z + 1, world)) // Front face
                {
                    AddFace(blockPos + float3(0, 0, 1), float3(0, 0, 1), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x, y, z - 1, world)) // Back face
                {
                    AddFace(blockPos, float3(0, 0, -1), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
            }
        }
    }
    
    m_MeshBuilt = true;
    m_Dirty = false;
}

bool Chunk::ShouldRenderFace(int x, int y, int z, int adjX, int adjY, int adjZ, VoxelWorld* world) const
{
    // Check if adjacent block is within this chunk
    if (adjX >= 0 && adjX < CHUNK_X_SIZE && adjY >= 0 && adjY < CHUNK_Y_SIZE && adjZ >= 0 && adjZ < CHUNK_Z_SIZE)
    {
        // Adjacent block is in this chunk
        Block adjacentBlock = m_Blocks[adjX][adjY][adjZ];
        return adjacentBlock.IsTransparent();
    }
    
    // Adjacent block is outside this chunk - check across chunk boundaries if world is available
    if (world != nullptr)
    {
        // Convert to world coordinates
        int worldX = m_ChunkX * CHUNK_X_SIZE + adjX;
        int worldY = m_ChunkY * CHUNK_Y_SIZE + adjY;
        int worldZ = m_ChunkZ * CHUNK_Z_SIZE + adjZ;
        
        Block adjacentBlock = world->GetBlock(worldX, worldY, worldZ);
        return adjacentBlock.IsTransparent();
    }
    
    // No world context available, assume transparent (render face) for safety
    return true;
}

void Chunk::AddFace(const float3& pos, const float3& normal, const float2& uvMin, const float2& uvMax)
{
    // Define face vertices based on normal direction with consistent counter-clockwise winding
    float3 vertices[4];
    float2 uvs[4] = {
        {uvMin.x, uvMin.y}, {uvMax.x, uvMin.y}, {uvMax.x, uvMax.y}, {uvMin.x, uvMax.y}
    };
    
    if (normal.y > 0.5f) // Top face (looking down at Y+)
    {
        vertices[0] = pos + float3(0, 0, 0);  // Bottom-left
        vertices[1] = pos + float3(1, 0, 0);  // Bottom-right
        vertices[2] = pos + float3(1, 0, 1);  // Top-right
        vertices[3] = pos + float3(0, 0, 1);  // Top-left
    }
    else if (normal.y < -0.5f) // Bottom face (looking up at Y-)
    {
        vertices[0] = pos + float3(0, 0, 1);  // Top-left
        vertices[1] = pos + float3(1, 0, 1);  // Top-right
        vertices[2] = pos + float3(1, 0, 0);  // Bottom-right
        vertices[3] = pos + float3(0, 0, 0);  // Bottom-left
    }
    else if (normal.x > 0.5f) // Right face (looking at X+)
    {
        vertices[0] = pos + float3(0, 0, 1);  // Front-bottom
        vertices[1] = pos + float3(0, 1, 1);  // Front-top
        vertices[2] = pos + float3(0, 1, 0);  // Back-top
        vertices[3] = pos + float3(0, 0, 0);  // Back-bottom
    }
    else if (normal.x < -0.5f) // Left face (looking at X-)
    {
        vertices[0] = pos + float3(0, 0, 0);  // Back-bottom
        vertices[1] = pos + float3(0, 1, 0);  // Back-top
        vertices[2] = pos + float3(0, 1, 1);  // Front-top
        vertices[3] = pos + float3(0, 0, 1);  // Front-bottom
    }
    else if (normal.z > 0.5f) // Front face (looking at Z+)
    {
        vertices[0] = pos + float3(0, 0, 0);  // Left-bottom
        vertices[1] = pos + float3(0, 1, 0);  // Left-top
        vertices[2] = pos + float3(1, 1, 0);  // Right-top
        vertices[3] = pos + float3(1, 0, 0);  // Right-bottom
    }
    else // Back face (looking at Z-)
    {
        vertices[0] = pos + float3(1, 0, 0);  // Right-bottom
        vertices[1] = pos + float3(1, 1, 0);  // Right-top
        vertices[2] = pos + float3(0, 1, 0);  // Left-top
        vertices[3] = pos + float3(0, 0, 0);  // Left-bottom
    }
    
    // Add vertices to the mesh (pos + normal + uv = 8 floats per vertex)
    for (int i = 0; i < 4; ++i)
    {
        m_Vertices.insert(m_Vertices.end(), {
            vertices[i].x, vertices[i].y, vertices[i].z,  // Position
            normal.x, normal.y, normal.z,                 // Normal
            uvs[i].x, uvs[i].y                           // UV
        });
    }
}