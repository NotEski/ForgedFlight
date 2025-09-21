#include "Chunk.h"
#include <random>
#include <cmath>

Chunk::Chunk(int x, int y, int z)
    : m_ChunkX(x), m_ChunkY(y), m_ChunkZ(z)
{
    // Initialize all blocks to air
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                m_Blocks[x][y][z].type = BlockType::Air;
            }
        }
    }
}

Block Chunk::GetBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)
        return Block{}; // Return air for out-of-bounds
    
    return m_Blocks[x][y][z];
}

void Chunk::SetBlock(int x, int y, int z, BlockType type)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)
        return;
    
    m_Blocks[x][y][z].type = type;
    m_Dirty = true;
}

void Chunk::Generate()
{
    // Simple terrain generation for cubic chunks
    std::mt19937 gen(m_ChunkX * 1000 + m_ChunkY * 100 + m_ChunkZ);
    std::uniform_real_distribution<float> noise(0.0f, 1.0f);
    
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                // Calculate world coordinates
                int worldX = m_ChunkX * CHUNK_SIZE + x;
                int worldY = m_ChunkY * CHUNK_HEIGHT + y;
                int worldZ = m_ChunkZ * CHUNK_SIZE + z;
                
                // Generate height based on simple noise
                float height = 32.0f + 16.0f * sin(worldX * 0.1f) * cos(worldZ * 0.1f);
                height += 8.0f * sin(worldX * 0.2f) * cos(worldZ * 0.2f);
                int terrainHeight = static_cast<int>(height);
                
                // Determine block type based on world Y position
                if (worldY < terrainHeight - 5)
                {
                    m_Blocks[x][y][z].type = BlockType::Stone;
                }
                else if (worldY < terrainHeight - 1)
                {
                    m_Blocks[x][y][z].type = BlockType::Dirt;
                }
                else if (worldY < terrainHeight)
                {
                    m_Blocks[x][y][z].type = BlockType::Grass;
                }
                else
                {
                    m_Blocks[x][y][z].type = BlockType::Air;
                }
            }
        }
    }
    
    m_Dirty = true;
}

void Chunk::BuildMesh()
{
    if (!m_Dirty)
        return;
    
    m_Vertices.clear();
    m_Indices.clear();
    
    uint32_t indexOffset = 0;
    
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                Block block = m_Blocks[x][y][z];
                if (block.type == BlockType::Air)
                    continue;
                
                float3 blockPos = float3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                
                // Check each face of the block
                if (ShouldRenderFace(x, y, z, x, y + 1, z)) // Top face
                {
                    AddFace(blockPos + float3(0, 1, 0), float3(0, 1, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x, y - 1, z)) // Bottom face
                {
                    AddFace(blockPos, float3(0, -1, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset + 2, indexOffset + 1, indexOffset,
                        indexOffset, indexOffset + 3, indexOffset + 2
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x + 1, y, z)) // Right face
                {
                    AddFace(blockPos + float3(1, 0, 0), float3(1, 0, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x - 1, y, z)) // Left face
                {
                    AddFace(blockPos, float3(-1, 0, 0), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset + 2, indexOffset + 1, indexOffset,
                        indexOffset, indexOffset + 3, indexOffset + 2
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x, y, z + 1)) // Front face
                {
                    AddFace(blockPos + float3(0, 0, 1), float3(0, 0, 1), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset, indexOffset + 1, indexOffset + 2,
                        indexOffset + 2, indexOffset + 3, indexOffset
                    });
                    indexOffset += 4;
                }
                
                if (ShouldRenderFace(x, y, z, x, y, z - 1)) // Back face
                {
                    AddFace(blockPos, float3(0, 0, -1), float2(0, 0), float2(1, 1));
                    m_Indices.insert(m_Indices.end(), {
                        indexOffset + 2, indexOffset + 1, indexOffset,
                        indexOffset, indexOffset + 3, indexOffset + 2
                    });
                    indexOffset += 4;
                }
            }
        }
    }
    
    m_MeshBuilt = true;
    m_Dirty = false;
}

bool Chunk::ShouldRenderFace(int x, int y, int z, int adjX, int adjY, int adjZ) const
{
    // Check if adjacent block is transparent or out of bounds
    if (adjX < 0 || adjX >= CHUNK_SIZE || adjY < 0 || adjY >= CHUNK_HEIGHT || adjZ < 0 || adjZ >= CHUNK_SIZE)
        return true; // Render face if adjacent is out of bounds
    
    Block adjacentBlock = m_Blocks[adjX][adjY][adjZ];
    return adjacentBlock.IsTransparent();
}

void Chunk::AddFace(const float3& pos, const float3& normal, const float2& uvMin, const float2& uvMax)
{
    // Define face vertices based on normal direction
    float3 vertices[4];
    float2 uvs[4] = {
        {uvMin.x, uvMin.y}, {uvMax.x, uvMin.y}, {uvMax.x, uvMax.y}, {uvMin.x, uvMax.y}
    };
    
    if (normal.y > 0.5f) // Top face
    {
        vertices[0] = pos + float3(0, 0, 0);
        vertices[1] = pos + float3(1, 0, 0);
        vertices[2] = pos + float3(1, 0, 1);
        vertices[3] = pos + float3(0, 0, 1);
    }
    else if (normal.y < -0.5f) // Bottom face
    {
        vertices[0] = pos + float3(0, 0, 1);
        vertices[1] = pos + float3(1, 0, 1);
        vertices[2] = pos + float3(1, 0, 0);
        vertices[3] = pos + float3(0, 0, 0);
    }
    else if (normal.x > 0.5f) // Right face
    {
        vertices[0] = pos + float3(0, 0, 0);
        vertices[1] = pos + float3(0, 1, 0);
        vertices[2] = pos + float3(0, 1, 1);
        vertices[3] = pos + float3(0, 0, 1);
    }
    else if (normal.x < -0.5f) // Left face
    {
        vertices[0] = pos + float3(0, 0, 1);
        vertices[1] = pos + float3(0, 1, 1);
        vertices[2] = pos + float3(0, 1, 0);
        vertices[3] = pos + float3(0, 0, 0);
    }
    else if (normal.z > 0.5f) // Front face
    {
        vertices[0] = pos + float3(1, 0, 0);
        vertices[1] = pos + float3(1, 1, 0);
        vertices[2] = pos + float3(0, 1, 0);
        vertices[3] = pos + float3(0, 0, 0);
    }
    else // Back face
    {
        vertices[0] = pos + float3(0, 0, 0);
        vertices[1] = pos + float3(0, 1, 0);
        vertices[2] = pos + float3(1, 1, 0);
        vertices[3] = pos + float3(1, 0, 0);
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