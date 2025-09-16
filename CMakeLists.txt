cmake_minimum_required(VERSION 3.20)
project(ForgedFlight)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Diligent Engine
add_subdirectory(DiligentEngine)

# Create the executable
add_executable(ForgedFlight
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# Link against Diligent Engine
target_link_libraries(ForgedFlight
    PRIVATE
    Diligent-TextureLoader
    Diligent-ImGui
)

target_include_directories(ForgedFlight
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentCore
    ${CMAKE_CURRENT_SOURCE_DIR}/DiligentEngine/DiligentTools
)
