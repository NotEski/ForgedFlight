#pragma once

#include "Block.h"
#include "Common/interface/BasicMath.hpp"
#include <array>
#include <vector>

using namespace Diligent;

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 16;  // Now cubic: 16x16x16

class Chunk
{
public:
    Chunk(int x, int y, int z);
    ~Chunk() = default;

    // Block access
    Block GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, BlockType type);
    
    // Position helpers
    int3 GetWorldPosition() const { return int3(m_ChunkX * CHUNK_SIZE, m_ChunkY * CHUNK_HEIGHT, m_ChunkZ * CHUNK_SIZE); }
    int GetChunkX() const { return m_ChunkX; }
    int GetChunkY() const { return m_ChunkY; }
    int GetChunkZ() const { return m_ChunkZ; }
    
    // Generation and mesh building
    void Generate();
    void BuildMesh();
    bool IsMeshBuilt() const { return m_MeshBuilt; }
    bool IsDirty() const { return m_Dirty; }
    void MarkDirty() { m_Dirty = true; }
    
    // Mesh data access
    const std::vector<float>& GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
    size_t GetVertexCount() const { return m_Vertices.size() / 8; } // 8 floats per vertex (pos + normal + uv)
    size_t GetIndexCount() const { return m_Indices.size(); }

private:
    // Block storage
    std::array<std::array<std::array<Block, CHUNK_HEIGHT>, CHUNK_SIZE>, CHUNK_SIZE> m_Blocks;
    
    // Chunk position
    int m_ChunkX;
    int m_ChunkY;
    int m_ChunkZ;
    
    // Mesh data
    std::vector<float> m_Vertices;
    std::vector<uint32_t> m_Indices;
    bool m_MeshBuilt = false;
    bool m_Dirty = true;
    
    // Helper methods
    bool IsBlockVisible(int x, int y, int z) const;
    void AddFace(const float3& pos, const float3& normal, const float2& uvMin, const float2& uvMax);
    bool ShouldRenderFace(int x, int y, int z, int adjX, int adjY, int adjZ) const;
};