# Chapter 1: Project Foundation and Setup

## Overview
This chapter covers setting up the basic project structure, understanding Diligent Engine integration, and creating a Windows application framework that can host a voxel engine.

## Learning Objectives
By the end of this chapter, you will understand:
- How to structure a modern C++ game engine project
- Diligent Engine basics and multi-API graphics support
- Windows application lifecycle and message handling
- Memory management patterns for game engines
- Debug console integration for development

## Prerequisites
- **C++ Knowledge**: Intermediate level (C++17/20 features)
- **Graphics Concepts**: Basic understanding of 3D graphics, vertices, matrices
- **Development Tools**: Visual Studio 2019+, CMake, Git
- **Platform**: Windows 10/11 (primary target)

## Key Concepts

### 1. Engine Architecture Overview

The Forged Flight engine follows a modular architecture:

```
ForgedFlightApp (Application Core)
├── Graphics Backend (Diligent Engine)
│   ├── Device & Context Management
│   ├── Pipeline State Objects
│   └── Resource Management
├── World System
│   ├── Chunk Management
│   ├── Block System
│   └── World Generation
├── Rendering System
│   ├── Camera Management
│   ├── Mesh Building
│   └── Scene Rendering
└── Input System
    ├── Keyboard Handling
    ├── Mouse Input
    └── Camera Controls
```

### 2. Diligent Engine Integration

**Why Diligent Engine?**
- Cross-platform graphics abstraction (D3D11, D3D12, OpenGL, Vulkan, Metal)
- Modern C++ design with RAII and smart pointers
- Performance-focused with minimal overhead
- Excellent documentation and examples

**Key Classes:**
- `IRenderDevice`: Hardware abstraction layer
- `IDeviceContext`: Command recording and execution
- `ISwapChain`: Presentation and backbuffer management
- `IPipelineState`: Shader programs and render state

### 3. Windows Application Framework

Our application follows the traditional Windows message loop pattern but with modern C++ practices:

**Core Components:**
- **WinMain**: Entry point with COM initialization and cleanup
- **WindowProc**: Message handling for input and window events
- **Application Class**: Game logic container with clean lifecycle
- **Exception Handling**: Proper error reporting and cleanup

## Implementation Walkthrough

### Step 1: Project Structure Setup

Create the following directory structure:
```
ForgedFlight/
├── src/
│   ├── Core/
│   │   ├── main.cpp
│   │   ├── ForgedFlightApp.h
│   │   └── ForgedFlightApp.cpp
│   ├── Rendering/
│   └── World/
├── DiligentEngine/ (git submodule)
├── CMakeLists.txt
└── README.md
```

### Step 2: CMake Configuration

Key considerations for CMake setup:
- Diligent Engine as git submodule
- Proper include paths and linking
- Debug/Release configurations
- Platform-specific settings

### Step 3: Main Application Entry Point

**Essential Elements of main.cpp:**

1. **Debug Console Allocation**
   ```cpp
   #ifdef _DEBUG
   if (AllocConsole()) {
       // Redirect stdout, stderr, stdin to console
       // Essential for debugging voxel generation
   }
   #endif
   ```

2. **Graphics API Selection**
   ```cpp
   // Command line parsing for graphics API
   // --device d3d11|d3d12|gl|vulkan
   // Fallback priority: D3D12 → Vulkan → D3D11 → OpenGL
   ```

3. **Window Creation**
   ```cpp
   // Standard Win32 window with proper sizing
   // Message handling for resize, input, etc.
   ```

4. **Application Lifecycle**
   ```cpp
   // Initialize → Update/Render Loop → Cleanup
   // Proper exception handling throughout
   ```

### Step 4: Application Class Design

**ForgedFlightApp Responsibilities:**
- Graphics engine initialization and management
- Component coordination (Camera, World, Renderer)
- Input processing and routing
- Frame timing and performance monitoring

**Key Design Patterns:**
- **RAII**: Automatic resource cleanup via destructors
- **Smart Pointers**: RefCntAutoPtr for Diligent objects, unique_ptr for game objects
- **Exception Safety**: Strong exception guarantees in initialization

### Step 5: Graphics Engine Integration

**Multi-API Support Pattern:**
```cpp
switch (deviceType) {
    case RENDER_DEVICE_TYPE_D3D11:
        // D3D11 factory and device creation
        break;
    case RENDER_DEVICE_TYPE_D3D12:
        // D3D12 factory and device creation
        break;
    // etc.
}
```

**Resource Management:**
- Use RefCntAutoPtr for all Diligent objects
- Proper cleanup order in destructors
- Context flushing before destruction

## Factory System Considerations

Even at this early stage, consider how the application framework will support factory automation:

### 1. **Headless Processing**
The application can run without rendering for server-side factory simulation:
```cpp
// Future: Headless mode flag
bool m_HeadlessMode = false;
if (!m_HeadlessMode) {
    // Only create graphics resources if needed
}
```

### 2. **Background Processing**
The update loop can handle factory ticking while rendering continues:
```cpp
// Separate update frequencies
UpdateFactories(deltaTime);  // 20 TPS for factories
UpdateRendering(deltaTime);  // 60+ FPS for visuals
```

### 3. **Save/Load Framework**
Application state management for factory persistence:
```cpp
// Future: Serialization hooks
virtual void SaveState() = 0;
virtual void LoadState() = 0;
```

## Common Issues and Solutions

### Issue 1: Graphics Device Creation Fails
**Symptoms:** Black screen, no rendering output
**Solution:** Check graphics drivers, try different APIs via command line

### Issue 2: Memory Leaks in Debug
**Symptoms:** _CrtSetDbgFlag reports leaks
**Solution:** Ensure proper RefCntAutoPtr usage, flush context before cleanup

### Issue 3: Console Output Missing
**Symptoms:** std::cout doesn't show debug information
**Solution:** Verify console allocation and stream redirection in debug builds

### Issue 4: Window Events Not Processed
**Symptoms:** Unresponsive window, no input
**Solution:** Check WindowProc message routing to application instance

## Testing and Validation

### Basic Functionality Tests
1. **Application Startup**: Window appears with correct title
2. **Graphics API Selection**: Command line switches work correctly
3. **Debug Console**: Output appears in debug builds
4. **Clean Shutdown**: No crashes or leaks on exit
5. **Window Resize**: Proper handling of resize events

### Performance Baselines
- **Startup Time**: < 2 seconds on typical hardware
- **Memory Usage**: < 100MB at startup
- **CPU Usage**: < 5% when idle

## Next Steps

With the foundation complete, Chapter 2 will cover:
- Basic rendering pipeline setup
- Shader creation and compilation
- Vertex/index buffer management
- Drawing your first triangle

## Key Resources

- [Diligent Engine Tutorials](https://github.com/DiligentGraphics/DiligentEngine/tree/master/Tutorials)
- [Windows Programming Reference](https://docs.microsoft.com/en-us/windows/win32/apiindex/windows-api-list)
- [Modern C++ Best Practices](https://github.com/cpp-best-practices/cppbestpractices)
- [CMake Documentation](https://cmake.org/cmake/help/latest/)

## Chapter Summary

This chapter established the core application framework that will host your voxel engine. The design emphasizes:
- **Clean Architecture**: Separation of concerns between platform code and game logic
- **Resource Safety**: Proper RAII and smart pointer usage
- **Extensibility**: Framework ready for complex systems like factory automation
- **Debugging Support**: Rich debug output and error handling

The foundation is now ready for building graphics rendering capabilities in the next chapter.