# Forged Flight - Project Structure

This document outlines the organized folder structure for the Forged Flight voxel game project.

## Project Overview

Forged Flight is a modern C++ voxel game built using the Diligent Engine graphics framework. The project follows a clean, modular architecture with separation of concerns.

## Folder Structure

```
forgedflight/
├── assets/                     # Game assets and resources
│   ├── shaders/               # HLSL/GLSL shader files
│   ├── textures/              # Texture files (PNG, DDS, etc.)
│   └── models/                # 3D model files (if any)
│
├── build/                     # CMake build output directory
│   └── Debug/                 # Debug build artifacts
│       └── ForgedFlight.exe   # Main executable
│
├── DiligentEngine/            # Diligent Engine submodule
│   └── ...                    # Graphics engine source code
│
├── docs/                      # Documentation files
│   ├── development/           # Development guidelines
│   ├── GDD/                   # Game Design Documents
│   ├── learning/              # Learning resources
│   └── pm/                    # Project management docs
│
├── include/                   # Public header files
│   ├── Core/                  # Core application headers
│   │   └── ForgedFlightApp.h  # Main application class
│   ├── Rendering/             # Rendering system headers
│   │   └── Camera.h           # Camera system
│   ├── World/                 # Voxel world headers
│   │   ├── Block.h            # Block definitions
│   │   ├── Chunk.h            # Chunk management
│   │   ├── ChunkManager.h     # Chunk rendering/management
│   │   └── VoxelWorld.h       # World management
│   ├── Input/                 # Input handling headers (future)
│   └── Utils/                 # Utility headers (future)
│
├── scripts/                   # Build and utility scripts
│   ├── build.bat              # Windows build script
│   └── setup.bat              # Environment setup script
│
├── src/                       # Source implementation files
│   ├── Core/                  # Core application source
│   │   ├── main.cpp           # Application entry point
│   │   └── ForgedFlightApp.cpp # Main application implementation
│   ├── Rendering/             # Rendering system source
│   │   └── Camera.cpp         # Camera implementation
│   ├── World/                 # Voxel world source
│   │   ├── Chunk.cpp          # Chunk implementation
│   │   ├── ChunkManager.cpp   # Chunk rendering/management
│   │   └── VoxelWorld.cpp     # World management
│   ├── Input/                 # Input handling source (future)
│   └── Utils/                 # Utility source (future)
│
├── CMakeLists.txt             # Main CMake configuration
├── STRUCTURE.md              # This file - project structure documentation
└── README.md                 # Project overview and build instructions
```

## Architecture Overview

### Core Systems
- **Core/**: Application lifecycle, main loop, and core functionality
- **Rendering/**: Graphics rendering, camera systems, and visual effects
- **World/**: Voxel world management, chunk systems, and block handling
- **Input/**: User input processing and control systems (future expansion)
- **Utils/**: Common utilities and helper functions (future expansion)

### Key Components

#### Core System
- `ForgedFlightApp`: Main application class managing initialization, update, and rendering
- `main.cpp`: Win32 application entry point with window management

#### Rendering System
- `Camera`: First-person camera with movement and rotation controls
- Graphics pipeline management and shader handling

#### World System
- `VoxelWorld`: High-level world management and chunk coordination
- `ChunkManager`: Chunk rendering, buffer management, and GPU resource handling
- `Chunk`: Individual chunk data structure and mesh generation
- `Block`: Block type definitions and properties

## Build System

The project uses CMake with organized source file groups:
- Explicit source file listing (no GLOB patterns)
- Separate include directories for each module
- Proper dependency management
- Support for multiple graphics APIs (D3D11, D3D12, OpenGL, Vulkan)

## Development Guidelines

1. **Header Placement**: Public headers go in `include/`, private headers stay with source files
2. **Include Paths**: Use module-prefixed includes (e.g., `#include "World/Chunk.h"`)
3. **Modularity**: Keep systems loosely coupled with clear interfaces
4. **Forward Declarations**: Use forward declarations in headers when possible
5. **Naming Conventions**: PascalCase for classes, camelCase for variables

## Future Expansions

The structure is designed to accommodate future systems:
- **Input/**: Dedicated input handling system
- **Audio/**: Sound and music systems
- **Networking/**: Multiplayer support
- **Physics/**: Physics simulation
- **AI/**: Game AI systems
- **UI/**: User interface systems

This modular structure ensures the codebase remains maintainable and scalable as the project grows.