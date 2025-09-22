# Chapter 4: Block System and Voxel Data Structures

## Overview
This chapter covers designing and implementing a robust block system that serves as the foundation for both basic voxel gameplay and complex factory automation. We'll create extensible block types, efficient storage, and the infrastructure for machine blocks.

## Learning Objectives
By the end of this chapter, you will understand:
- Block type enumeration and property systems
- Efficient voxel data storage and access patterns
- Block metadata for complex machine states
- Memory-efficient block storage in chunks
- Extensible design for future factory automation

## Key Concepts

### 1. Block Type System Architecture

A robust block system requires multiple layers:

```cpp
// Basic block identification
enum class BlockType : uint16_t {
    Air = 0,           // Special: completely empty
    Stone,             // Basic solid block
    Dirt,              // Diggable terrain
    Grass,             // Surface terrain
    Wood,              // Building material
    // Factory blocks start at 1000
    Conveyor = 1000,   // Automation transport
    Assembler,         // Manufacturing machine
    PowerPole,         // Electrical infrastructure
    // ... more factory types
};

// Block properties for gameplay
struct BlockProperties {
    bool    isOpaque;      // Blocks light
    bool    isSolid;       // Collision detection
    bool    isDiggable;    // Can be broken
    float   hardness;      // Time to break
    uint8_t lightLevel;    // Emitted light
    bool    needsSupport;  // Must be placed on solid surface
    bool    isFactory;     // Has machine logic
};
```

### 2. Block Data Storage

Different blocks need different amounts of data:

**Simple Blocks**: Only need type ID (2 bytes)
**Machine Blocks**: Need additional state data (orientation, power, inventory, etc.)

```cpp
// Base block storage (minimal memory)
struct Block {
    BlockType type;
    uint8_t   metadata;  // Basic state (orientation, etc.)
};

// Extended block data for machines (stored separately)
struct BlockEntity {
    BlockType type;
    float3    position;
    // Machine-specific data
    uint32_t  power;
    uint8_t   orientation;
    std::vector<ItemStack> inventory;
    nlohmann::json customData;
};
```

### 3. Memory Organization Strategy

Optimize for both access speed and memory efficiency:

```cpp
// Chunk storage optimized for cache performance
class Chunk {
    // Packed array for simple blocks (most common)
    std::array<Block, CHUNK_SIZE³> m_Blocks;
    
    // Sparse storage for complex blocks (machines)
    std::unordered_map<uint32_t, std::unique_ptr<BlockEntity>> m_BlockEntities;
    
    // Spatial index for fast queries
    std::bitset<CHUNK_SIZE³> m_HasBlockEntity;
};
```

## Implementation Walkthrough

### Step 1: Enhanced Block Type Definition

**Block.h - Modern Block System:**
```cpp
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

// Primary block type enumeration
enum class BlockType : uint16_t
{
    // Special blocks
    Air = 0,
    Unknown = 1,
    
    // Natural terrain blocks (1-99)
    Stone = 10,
    Dirt = 11,
    Grass = 12,
    Sand = 13,
    Gravel = 14,
    Water = 15,
    
    // Building materials (100-199)
    Wood = 100,
    Planks = 101,
    Brick = 102,
    Glass = 103,
    Metal = 104,
    
    // Resource blocks (200-299)
    CoalOre = 200,
    IronOre = 201,
    CopperOre = 202,
    GoldOre = 203,
    
    // Factory infrastructure (1000+)
    ConveyorBelt = 1000,
    Assembler = 1001,
    PowerPole = 1002,
    ElectricMotor = 1003,
    SteamEngine = 1004,
    
    Count = 2000  // Reserve space for expansion
};

// Block properties for efficient queries
struct BlockProperties
{
    bool isOpaque = true;      // Blocks light/view
    bool isSolid = true;       // Has collision
    bool isDiggable = true;    // Can be broken by player
    float hardness = 1.0f;     // Time to break (seconds)
    uint8_t lightLevel = 0;    // Emitted light (0-15)
    bool needsSupport = false; // Must be placed on solid surface
    bool isFactory = false;    // Has machine logic
    bool isFluid = false;      // Water-like behavior
    
    // Rendering hints
    bool useTransparency = false;
    bool animateTexture = false;
    
    // Factory-specific properties
    bool conductsPower = false;
    bool consumesPower = false;
    bool needsOrientation = false;
};

// Basic block storage (optimized for memory)
struct Block
{
    BlockType type = BlockType::Air;
    uint8_t metadata = 0;  // Orientation, state flags, etc.
    
    // Quick property queries
    bool IsOpaque() const;
    bool IsSolid() const;
    bool IsAir() const { return type == BlockType::Air; }
    bool IsFactory() const;
    
    // Metadata helpers
    uint8_t GetOrientation() const { return metadata & 0x07; }  // 3 bits
    void SetOrientation(uint8_t orientation) { metadata = (metadata & 0xF8) | (orientation & 0x07); }
    
    bool GetFlag(uint8_t bitIndex) const { return (metadata >> bitIndex) & 1; }
    void SetFlag(uint8_t bitIndex, bool value);
};

// Block registry for property lookups
class BlockRegistry
{
public:
    static void Initialize();
    static const BlockProperties& GetProperties(BlockType type);
    static std::string GetBlockName(BlockType type);
    static BlockType GetBlockByName(const std::string& name);
    
private:
    static std::unordered_map<BlockType, BlockProperties> s_Properties;
    static std::unordered_map<std::string, BlockType> s_NameToType;
    static std::unordered_map<BlockType, std::string> s_TypeToName;
};
```

### Step 2: Block Registry Implementation

**BlockRegistry.cpp - Centralized Block Definitions:**
```cpp
#include "Block.h"

std::unordered_map<BlockType, BlockProperties> BlockRegistry::s_Properties;
std::unordered_map<std::string, BlockType> BlockRegistry::s_NameToType;
std::unordered_map<BlockType, std::string> BlockRegistry::s_TypeToName;

void BlockRegistry::Initialize()
{
    // Helper lambda for registration
    auto registerBlock = [](BlockType type, const std::string& name, const BlockProperties& props)
    {
        s_Properties[type] = props;
        s_NameToType[name] = type;
        s_TypeToName[type] = name;
    };
    
    // Air (special case)
    registerBlock(BlockType::Air, "air", BlockProperties{
        .isOpaque = false,
        .isSolid = false,
        .isDiggable = false,
        .hardness = 0.0f
    });
    
    // Basic terrain blocks
    registerBlock(BlockType::Stone, "stone", BlockProperties{
        .isOpaque = true,
        .isSolid = true,
        .isDiggable = true,
        .hardness = 2.0f
    });
    
    registerBlock(BlockType::Dirt, "dirt", BlockProperties{
        .isOpaque = true,
        .isSolid = true,
        .isDiggable = true,
        .hardness = 0.5f
    });
    
    // Factory blocks
    registerBlock(BlockType::ConveyorBelt, "conveyor_belt", BlockProperties{
        .isOpaque = false,
        .isSolid = true,
        .isDiggable = true,
        .hardness = 1.0f,
        .isFactory = true,
        .needsOrientation = true,
        .animateTexture = true
    });
    
    registerBlock(BlockType::Assembler, "assembler", BlockProperties{
        .isOpaque = true,
        .isSolid = true,
        .isDiggable = true,
        .hardness = 3.0f,
        .isFactory = true,
        .needsOrientation = true,
        .consumesPower = true
    });
    
    // Add more blocks as needed...
}

const BlockProperties& BlockRegistry::GetProperties(BlockType type)
{
    static BlockProperties defaultProps;
    auto it = s_Properties.find(type);
    return (it != s_Properties.end()) ? it->second : defaultProps;
}

bool Block::IsOpaque() const
{
    return BlockRegistry::GetProperties(type).isOpaque;
}

bool Block::IsSolid() const
{
    return BlockRegistry::GetProperties(type).isSolid;
}

bool Block::IsFactory() const
{
    return BlockRegistry::GetProperties(type).isFactory;
}
```

### Step 3: Extended Block Entity System

**BlockEntity.h - Complex Block Data:**
```cpp
#pragma once

#include "Block.h"
#include "Common/interface/BasicMath.hpp"
#include <memory>
#include <vector>

// Forward declarations for factory systems
class ItemStack;
class PowerNetwork;

// Base class for blocks that need extra data
class BlockEntity
{
public:
    BlockEntity(BlockType type, const int3& position);
    virtual ~BlockEntity() = default;
    
    // Core properties
    BlockType GetType() const { return m_Type; }
    const int3& GetPosition() const { return m_Position; }
    uint8_t GetOrientation() const { return m_Orientation; }
    void SetOrientation(uint8_t orientation) { m_Orientation = orientation; }
    
    // Serialization
    virtual void Serialize(nlohmann::json& json) const;
    virtual void Deserialize(const nlohmann::json& json);
    
    // Update logic (for factory automation)
    virtual void Update(float deltaTime) {}
    
    // Power system integration
    virtual bool ConsumePower() const { return false; }
    virtual uint32_t GetPowerConsumption() const { return 0; }
    virtual void SetPowerNetwork(PowerNetwork* network) { m_PowerNetwork = network; }
    
protected:
    BlockType m_Type;
    int3 m_Position;
    uint8_t m_Orientation = 0;
    PowerNetwork* m_PowerNetwork = nullptr;
};

// Conveyor belt with item transport
class ConveyorBlockEntity : public BlockEntity
{
public:
    ConveyorBlockEntity(const int3& position);
    
    void Update(float deltaTime) override;
    void Serialize(nlohmann::json& json) const override;
    void Deserialize(const nlohmann::json& json) override;
    
    // Item transport
    bool CanAcceptItem() const;
    void ReceiveItem(const ItemStack& item);
    ItemStack* GetItemAtPosition(float position); // 0.0 = input, 1.0 = output
    
private:
    struct TransportItem {
        ItemStack item;
        float position; // 0.0 to 1.0 along belt
        float speed = 1.0f; // units per second
    };
    
    std::vector<TransportItem> m_Items;
    float m_BeltSpeed = 2.0f; // blocks per second
};

// Assembler machine with recipes and power
class AssemblerBlockEntity : public BlockEntity
{
public:
    AssemblerBlockEntity(const int3& position);
    
    void Update(float deltaTime) override;
    bool ConsumePower() const override { return true; }
    uint32_t GetPowerConsumption() const override { return 100; } // 100W
    
    // Manufacturing
    void SetRecipe(const std::string& recipeId);
    bool HasValidRecipe() const;
    float GetCraftingProgress() const { return m_CraftingProgress; }
    
    // Inventory access
    std::vector<ItemStack>& GetInputInventory() { return m_InputInventory; }
    std::vector<ItemStack>& GetOutputInventory() { return m_OutputInventory; }
    
private:
    std::string m_CurrentRecipe;
    float m_CraftingProgress = 0.0f;
    float m_CraftingTime = 5.0f; // seconds per craft
    
    std::vector<ItemStack> m_InputInventory;
    std::vector<ItemStack> m_OutputInventory;
    
    bool HasRequiredItems() const;
    void ProduceOutput();
};
```

### Step 4: Chunk Integration

**Enhanced Chunk.h - BlockEntity Support:**
```cpp
class Chunk
{
public:
    // ... existing methods ...
    
    // Block entity management
    BlockEntity* GetBlockEntity(int x, int y, int z) const;
    void SetBlockEntity(int x, int y, int z, std::unique_ptr<BlockEntity> entity);
    void RemoveBlockEntity(int x, int y, int z);
    
    // Factory-specific queries
    std::vector<BlockEntity*> GetFactoryBlocks() const;
    void UpdateFactoryBlocks(float deltaTime);
    
    // Efficient serialization
    void SerializeBlockEntities(nlohmann::json& json) const;
    void DeserializeBlockEntities(const nlohmann::json& json);
    
private:
    // ... existing members ...
    
    // Sparse storage for complex blocks
    std::unordered_map<uint32_t, std::unique_ptr<BlockEntity>> m_BlockEntities;
    
    // Fast query helpers
    uint32_t GetBlockIndex(int x, int y, int z) const {
        return x + y * CHUNK_X_SIZE + z * CHUNK_X_SIZE * CHUNK_Y_SIZE;
    }
};
```

### Step 5: Block Placement and Interaction

**BlockInteraction.h - Player Block Interaction:**
```cpp
#pragma once

#include "Block.h"
#include "Common/interface/BasicMath.hpp"

struct BlockHitResult
{
    bool hit = false;
    int3 blockPos;
    int3 adjacentPos;  // For block placement
    float3 hitPoint;
    float3 normal;
    float distance;
};

class BlockInteractionSystem
{
public:
    // Raycast to find block under cursor
    BlockHitResult RaycastBlock(const float3& origin, const float3& direction, float maxDistance);
    
    // Block manipulation
    bool CanPlaceBlock(const int3& position, BlockType type) const;
    bool PlaceBlock(const int3& position, BlockType type, uint8_t orientation = 0);
    bool BreakBlock(const int3& position);
    
    // Factory-specific placement
    bool PlaceFactoryBlock(const int3& position, BlockType type, uint8_t orientation);
    bool ValidateFactoryPlacement(const int3& position, BlockType type) const;
    
    // Block information
    Block GetBlock(const int3& position) const;
    BlockEntity* GetBlockEntity(const int3& position) const;
    
private:
    VoxelWorld* m_World = nullptr;
    
    // Helper methods
    bool IsPositionValid(const int3& position) const;
    bool CheckSupport(const int3& position, BlockType type) const;
    void NotifyNeighborsOfChange(const int3& position);
};
```

## Factory System Integration

### 1. **Power Network Foundation**
Blocks can participate in electrical networks:
```cpp
class PowerNetwork {
    std::vector<BlockEntity*> m_Producers;  // Generators
    std::vector<BlockEntity*> m_Consumers;  // Machines
    std::vector<BlockEntity*> m_Conductors; // Power poles, cables
    
    uint32_t m_TotalProduction = 0;
    uint32_t m_TotalConsumption = 0;
};
```

### 2. **Item Transport Systems**
Conveyor belts and pipes use block entities:
```cpp
class TransportNetwork {
    std::vector<ConveyorBlockEntity*> m_Conveyors;
    
    void UpdateItemFlow(float deltaTime);
    void RouteItem(const ItemStack& item, const int3& destination);
};
```

### 3. **Recipe and Manufacturing**
Assemblers process items according to recipes:
```cpp
struct Recipe {
    std::string id;
    std::vector<ItemStack> inputs;
    std::vector<ItemStack> outputs;
    float craftingTime;
    uint32_t powerRequired;
};
```

## Performance Considerations

### Memory Usage
- **Basic Blocks**: 3 bytes each (type + metadata)
- **Block Entities**: ~100-500 bytes each (only for complex blocks)
- **Chunk Storage**: ~49KB for blocks + sparse entity storage

### Access Patterns
- **Block Type Queries**: O(1) direct array access
- **Property Lookups**: O(1) hash table lookup (cached)
- **Block Entity Access**: O(1) hash table lookup
- **Neighbor Queries**: O(1) with proper indexing

### Factory Performance
- **Update Batching**: Process factory blocks in groups
- **Network Optimization**: Only update connected components
- **Threading**: Background processing for complex factories

## Testing and Validation

### Unit Tests
1. **Block Registry**: All block types have valid properties
2. **Memory Layout**: Block storage uses expected memory
3. **Serialization**: Block entities save/load correctly
4. **Factory Logic**: Conveyors transport items, assemblers craft recipes

### Integration Tests
1. **Chunk Integration**: Block entities work within chunks
2. **World Persistence**: Complex blocks survive save/load
3. **Performance**: Large factories maintain target frame rate

## Next Steps

Chapter 5 will cover:
- Chunk management and streaming
- World coordinate systems
- Cross-chunk structure support
- Memory optimization for large worlds

## Key Resources

- [Minecraft Block System Analysis](https://minecraft.wiki/w/Block)
- [Entity Component System Patterns](https://github.com/SanderMertens/ecs-faq)
- [Game Programming Memory Management](https://gameprogrammingpatterns.com/object-pool.html)
- [Factory Game Design Patterns](https://www.factorio.com/blog/post/fff-176)

## Chapter Summary

This chapter established a robust block system foundation:
- **Extensible Block Types**: Registry-based system supporting simple terrain and complex factory blocks
- **Memory Efficiency**: Optimized storage with sparse block entities for complex data
- **Factory Ready**: Infrastructure for power networks, item transport, and manufacturing
- **Performance Focused**: Cache-friendly access patterns and efficient queries

The block system now provides the foundation for both basic voxel gameplay and sophisticated factory automation systems described in the Game Design Document.