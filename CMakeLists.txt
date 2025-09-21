cmake_minimum_required(VERSION 3.20)
project(ForgedFlight)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable Diligent Engine samples, tutorials, and unnecessary components
option(DILIGENT_BUILD_SAMPLES "Build Diligent Engine samples" ON)
option(DILIGENT_BUILD_DOCS "Build Diligent Engine documentation" OFF)
option(DILIGENT_BUILD_FX "Build DiligentFX module" ON)  # Enable for advanced lighting and post-processing
option(DILIGENT_BUILD_TOOLS "Build Diligent Engine tools" ON)  # Keep ON as we need TextureLoader

# Add Diligent Engine
add_subdirectory(DiligentEngine)

# Organize source files by directory
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ForgedFlightApp.cpp
)

set(RENDERING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Rendering/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Rendering/AdvancedRenderer.cpp
)

set(WORLD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/World/VoxelWorld.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/World/Chunk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/World/ChunkManager.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${RENDERING_SOURCES}
    ${WORLD_SOURCES}
)

# Create the executable
add_executable(ForgedFlight WIN32 ${ALL_SOURCES})

# Link against Diligent Engine
target_link_libraries(ForgedFlight
    PRIVATE
    Diligent-GraphicsEngineD3D11-static
    Diligent-GraphicsEngineD3D12-static
    Diligent-GraphicsEngineOpenGL-static
    Diligent-GraphicsEngineVk-static
    Diligent-GraphicsAccessories
    Diligent-BuildSettings
    Diligent-TargetPlatform
    Diligent-Common
    Diligent-TextureLoader
    DiligentFX  # Re-enabled for samples and proper integration
)

target_include_directories(ForgedFlight
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Graphics/GraphicsEngineD3D11/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Graphics/GraphicsEngineD3D12/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Graphics/GraphicsEngineOpenGL/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Graphics/GraphicsEngineVulkan/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Common/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Primitives/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore/Platforms/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentTools
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentTools/TextureLoader/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentFX
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentFX/PBR/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentFX/PostProcess/Common/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentFX/Components/interface
)

# Set working directory for debugging
set_target_properties(ForgedFlight PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
