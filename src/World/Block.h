#pragma once

#include <cstdint>

// Block types for the voxel world
enum class BlockType : uint8_t
{
    Air = 0,
    Stone,
    Dirt,
    Grass,
    Wood,
    Leaves,
    Water,
    Sand,
    Count
};

struct Block
{
    BlockType type = BlockType::Air;
    
    bool IsOpaque() const 
    { 
        return type != BlockType::Air && type != BlockType::Water; 
    }
    
    bool IsSolid() const 
    { 
        return type != BlockType::Air; 
    }
    
    bool IsTransparent() const 
    { 
        return type == BlockType::Air || type == BlockType::Water; 
    }
};