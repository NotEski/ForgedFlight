#pragma once

#include "Block.h"
#include "Common/interface/BasicMath.hpp"
#include <array>
#include <vector>

using namespace Diligent;

constexpr int CHUNK_X_SIZE = 16;
constexpr int CHUNK_Y_SIZE = 16;
constexpr int CHUNK_Z_SIZE = 16;

// Forward declaration for VoxelWorld
class VoxelWorld;

class Chunk
{
public:
    Chunk(int x, int y, int z);
    ~Chunk() = default;

    // Block access
    Block GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, BlockType type);
    
    // Position helpers
    int3 GetWorldPosition() const { return int3(m_ChunkX * CHUNK_X_SIZE, m_ChunkY * CHUNK_Y_SIZE, m_ChunkZ * CHUNK_Z_SIZE); }
    int GetChunkX() const { return m_ChunkX; }
    int GetChunkY() const { return m_ChunkY; }
    int GetChunkZ() const { return m_ChunkZ; }
    
    // Generation and mesh building
    void Generate();
    void BuildMesh(VoxelWorld* world = nullptr);
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
    std::array<std::array<std::array<Block, CHUNK_Y_SIZE>, CHUNK_X_SIZE>, CHUNK_Z_SIZE> m_Blocks;
    
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
    bool ShouldRenderFace(int x, int y, int z, int adjX, int adjY, int adjZ, VoxelWorld* world = nullptr) const;
};