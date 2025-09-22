# Forged Flight Voxel Engine - Code Archive

## Overview
This document archives the complete state of the Forged Flight voxel engine as of September 22, 2025. The engine successfully implements:

- **Multi-API Graphics Support**: D3D11, D3D12, OpenGL, Vulkan via Diligent Engine
- **Voxel World System**: Chunk-based world with 16x16x16 chunks
- **Camera System**: First-person camera with mouse lock functionality
- **Rendering Pipeline**: Basic cube rendering with world space transformation
- **Mouse Input**: Click-to-lock/unlock mouse for camera control
- **Debug Console**: Real-time debug output and ImGui integration

This archive preserves the working state before a planned rewrite with improved architecture.

## Architecture Overview

```
src/
├── Core/
│   ├── main.cpp           # Windows entry point, message loop
│   ├── ForgedFlightApp.h  # Main application class declaration
│   └── ForgedFlightApp.cpp # Application implementation
├── Rendering/
│   ├── Camera.h           # Camera class with mouse lock
│   ├── Camera.cpp         # Camera implementation
│   ├── AdvancedRenderer.h # Advanced rendering features
│   └── AdvancedRenderer.cpp
└── World/
    ├── Block.h            # Block type definitions
    ├── Chunk.h            # Chunk class for voxel storage
    ├── Chunk.cpp          # Chunk implementation
    ├── VoxelWorld.h       # World management
    ├── VoxelWorld.cpp     # World implementation
    ├── ChunkManager.h     # Chunk rendering management
    └── ChunkManager.cpp   # Chunk manager implementation
```

## Key Features Implemented

### 1. Multi-API Graphics Backend
The engine supports multiple graphics APIs through Diligent Engine:
- Command-line selection: `--device d3d11|d3d12|gl|vulkan`
- Automatic fallback priority: D3D12 → Vulkan → D3D11 → OpenGL
- Cross-platform window management

### 2. Voxel Chunk System
- **Chunk Size**: 16x16x16 blocks (CHUNK_X_SIZE, CHUNK_Y_SIZE, CHUNK_Z_SIZE)
- **World Coordinates**: 1 unit = 1 meter scale
- **Chunk Loading**: Spherical pattern around player with configurable render distance
- **Chunk Unloading**: Memory management for chunks outside render distance
- **Mesh Generation**: Face culling for hidden faces, world space transformation

### 3. Camera System with Mouse Lock
- **Movement**: WASD + mouse look
- **Mouse Lock**: Left-click to lock/unlock, Escape to unlock
- **Cursor Control**: Windows API integration for cursor hiding/centering
- **Configurable Speed**: Adjustable movement and sensitivity

### 4. Block System Foundation
- **Block Types**: Extensible enum-based system (Air, Stone, Dirt, etc.)
- **Properties**: Opacity, solidity, transparency flags
- **Storage**: 3D array within chunks for efficient access

## Archived Source Code

### main.cpp - Windows Entry Point
```cpp
/*
 * Forged Flight - A Voxel-based Game Engine
 * Copyright (c) 2025
 * 
 * Main entry point for the Forged Flight voxel game.
 */

#include <memory>
#include <exception>
#include <chrono>
#include <string>

#ifdef _DEBUG
#include <iostream>
#include <io.h>
#include <fcntl.h>
#endif

#ifndef NOMINMAX
#    define NOMINMAX
#endif
#include <Windows.h>
#include <windowsx.h>
#include <crtdbg.h>

#ifndef PLATFORM_WIN32
#    define PLATFORM_WIN32 1
#endif

#ifndef D3D11_SUPPORTED
#    define D3D11_SUPPORTED 1
#endif

#ifndef D3D12_SUPPORTED
#    define D3D12_SUPPORTED 1
#endif

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 1
#endif

#ifndef VULKAN_SUPPORTED
#    define VULKAN_SUPPORTED 1
#endif

#include "ForgedFlightApp.h"
#include "Common/interface/RefCntAutoPtr.hpp"

using namespace Diligent;

// Global application instance
std::unique_ptr<ForgedFlightApp> g_pTheApp;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool ProcessCommandLine(const char* cmdLine, RENDER_DEVICE_TYPE& deviceType);

int WINAPI WinMain(_In_ HINSTANCE hInstance,
                   _In_opt_ HINSTANCE hPrevInstance, 
                   _In_ LPSTR lpCmdLine,
                   _In_ int nShowCmd)
{

#if defined(_DEBUG) || defined(DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
#ifdef _DEBUG
        // Allocate a console for debug builds only
        if (AllocConsole())
        {
            FILE* pCout;
            freopen_s(&pCout, "CONOUT$", "w", stdout);
            FILE* pCerr;
            freopen_s(&pCerr, "CONOUT$", "w", stderr);
            FILE* pCin;
            freopen_s(&pCin, "CONIN$", "r", stdin);
            
            // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
            // point to console as well
            std::ios::sync_with_stdio(true);
            
            // Optional: Set console title
            SetConsoleTitleA("Forged Flight Debug Console");
            
            std::cout << "Forged Flight Debug Console Initialized" << std::endl;
        }
#endif
        
        // Initialize COM
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr))
        {
            std::cerr << "Failed to initialize COM" << std::endl;
            return -1;
        }

        // Determine render device type from command line
        RENDER_DEVICE_TYPE deviceType = RENDER_DEVICE_TYPE_D3D12; // Default
        if (!ProcessCommandLine(GetCommandLineA(), deviceType))
        {
#ifdef _DEBUG
            std::cerr << "Failed to process command line arguments" << std::endl;
#endif
            CoUninitialize();
            return -1;
        }

#ifdef _DEBUG
        std::cout << "Using graphics API: ";
        switch (deviceType)
        {
            case RENDER_DEVICE_TYPE_D3D11: std::cout << "Direct3D 11"; break;
            case RENDER_DEVICE_TYPE_D3D12: std::cout << "Direct3D 12"; break;
            case RENDER_DEVICE_TYPE_GL: std::cout << "OpenGL"; break;
            case RENDER_DEVICE_TYPE_VULKAN: std::cout << "Vulkan"; break;
        }
        std::cout << std::endl;
#endif

        // Register window class (using ANSI like Diligent samples)
        WNDCLASSEXA wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXA);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = "ForgedFlightClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

        if (!RegisterClassExA(&wcex))
        {
#ifdef _DEBUG
            std::cerr << "Failed to register window class" << std::endl;
#endif
            CoUninitialize();
            return -1;
        }

        // Create window
        const int windowWidth = 1280;
        const int windowHeight = 720;
        
        RECT windowRect = { 0, 0, windowWidth, windowHeight };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
        
        std::string windowTitle = "Forged Flight - Voxel Game";
        switch (deviceType)
        {
            case RENDER_DEVICE_TYPE_D3D11: windowTitle += " (D3D11)"; break;
            case RENDER_DEVICE_TYPE_D3D12: windowTitle += " (D3D12)"; break;
            case RENDER_DEVICE_TYPE_GL: windowTitle += " (OpenGL)"; break;
            case RENDER_DEVICE_TYPE_VULKAN: windowTitle += " (Vulkan)"; break;
        }

        HWND hWnd = CreateWindowA("ForgedFlightClass", windowTitle.c_str(),
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  windowRect.right - windowRect.left,
                                  windowRect.bottom - windowRect.top,
                                  nullptr, nullptr, hInstance, nullptr);

        if (!hWnd)
        {
#ifdef _DEBUG
            std::cerr << "Failed to create window" << std::endl;
#endif
            CoUninitialize();
            return -1;
        }

        ShowWindow(hWnd, nShowCmd);
        UpdateWindow(hWnd);

        // Create application instance
        g_pTheApp = std::make_unique<ForgedFlightApp>();

#ifdef _DEBUG
        std::cout << "Initializing application..." << std::endl;
#endif

        // Initialize the application
        NativeAppInitAttrib initAttrib;
        initAttrib.hWnd = hWnd;
        initAttrib.DeviceType = deviceType;

        g_pTheApp->Initialize(initAttrib);

#ifdef _DEBUG
        std::cout << "Application initialized successfully! Starting main loop..." << std::endl;
#endif

        // Main message loop
        MSG msg = {};
        auto lastTime = std::chrono::high_resolution_clock::now();
        auto lastTitleUpdate = lastTime;
        double frameCount = 0;
        
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                // Calculate delta time
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
                lastTime = currentTime;
                frameCount++;

                try 
                {
                    // Update and render
                    g_pTheApp->Update(std::chrono::duration<double>(currentTime.time_since_epoch()).count(), deltaTime);
                    g_pTheApp->Render();
                    g_pTheApp->Present();
                    
                    // Update window title with FPS every half second
                    auto timeSinceLastTitleUpdate = std::chrono::duration<double>(currentTime - lastTitleUpdate).count();
                    if (timeSinceLastTitleUpdate >= 0.5)
                    {
                        double fps = frameCount / timeSinceLastTitleUpdate;
                        std::string newTitle = "Forged Flight - Voxel Game";
                        switch (deviceType)
                        {
                            case RENDER_DEVICE_TYPE_D3D11: newTitle += " (D3D11)"; break;
                            case RENDER_DEVICE_TYPE_D3D12: newTitle += " (D3D12)"; break;
                            case RENDER_DEVICE_TYPE_GL: newTitle += " (OpenGL)"; break;
                            case RENDER_DEVICE_TYPE_VULKAN: newTitle += " (Vulkan)"; break;
                        }
                        newTitle += " - " + std::to_string((int)fps) + " FPS";
                        SetWindowTextA(hWnd, newTitle.c_str());
                        
                        frameCount = 0;
                        lastTitleUpdate = currentTime;
                    }
                }
                catch (const std::exception& e)
                {
#ifdef _DEBUG
                    std::cerr << "Exception in main loop: " << e.what() << std::endl;
#endif
                    MessageBoxA(nullptr, e.what(), "Main Loop Error", MB_OK | MB_ICONERROR);
                    break;
                }
                catch (...)
                {
#ifdef _DEBUG
                    std::cerr << "Unknown exception in main loop" << std::endl;
#endif
                    MessageBoxA(nullptr, "Unknown error in main loop", "Main Loop Error", MB_OK | MB_ICONERROR);
                    break;
                }
            }
        }

        // Cleanup
        g_pTheApp.reset();

#ifdef _DEBUG
        std::cout << "Application shutdown complete." << std::endl;
        // Keep console open for a moment in debug builds
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        FreeConsole();
#endif

        CoUninitialize();

        return static_cast<int>(msg.wParam);
    }
    catch (const std::exception& e)
    {
#ifdef _DEBUG
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
#endif
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return -1;
    }
    catch (...)
    {
#ifdef _DEBUG
        std::cerr << "Unknown exception occurred" << std::endl;
#endif
        MessageBoxA(nullptr, "An unknown error occurred", "Fatal Error", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return -1;
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_SIZE:
        {
            if (g_pTheApp && wParam != SIZE_MINIMIZED)
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                g_pTheApp->WindowResize(width, height);
            }
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (g_pTheApp)
            {
                g_pTheApp->OnKeyDown(static_cast<UINT8>(wParam));
            }
            
            // ESC key to exit
            if (wParam == VK_ESCAPE)
            {
                PostQuitMessage(0);
            }
            return 0;
        }

        case WM_KEYUP:
        {
            if (g_pTheApp)
            {
                g_pTheApp->OnKeyUp(static_cast<UINT8>(wParam));
            }
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (g_pTheApp)
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                g_pTheApp->OnMouseMove(xPos, yPos);
            }
            return 0;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            if (g_pTheApp)
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                UINT button = (message == WM_LBUTTONDOWN) ? 0 : (message == WM_RBUTTONDOWN) ? 1 : 2;
                g_pTheApp->OnMouseDown(xPos, yPos, button);
            }
            return 0;
        }

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        {
            if (g_pTheApp)
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                UINT button = (message == WM_LBUTTONUP) ? 0 : (message == WM_RBUTTONUP) ? 1 : 2;
                g_pTheApp->OnMouseUp(xPos, yPos, button);
            }
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            if (g_pTheApp)
            {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                g_pTheApp->OnMouseWheel(delta);
            }
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 640;
            lpMMI->ptMinTrackSize.y = 480;
            return 0;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

bool ProcessCommandLine(const char* cmdLine, RENDER_DEVICE_TYPE& deviceType)
{
    const char* mode = nullptr;
    const char* keys[] = {"--device ", "--device=", "-d "};
    
    for (size_t i = 0; i < _countof(keys); ++i)
    {
        const char* key = keys[i];
        if ((mode = strstr(cmdLine, key)) != nullptr)
        {
            mode += strlen(key);
            break;
        }
    }

    if (mode != nullptr)
    {
        while (*mode == ' ') ++mode;

        if (_stricmp(mode, "d3d11") == 0)
        {
#if D3D11_SUPPORTED
            deviceType = RENDER_DEVICE_TYPE_D3D11;
#else
            return false;
#endif
        }
        else if (_stricmp(mode, "d3d12") == 0)
        {
#if D3D12_SUPPORTED
            deviceType = RENDER_DEVICE_TYPE_D3D12;
#else
            return false;
#endif
        }
        else if (_stricmp(mode, "gl") == 0 || _stricmp(mode, "opengl") == 0)
        {
#if GL_SUPPORTED
            deviceType = RENDER_DEVICE_TYPE_GL;
#else
            return false;
#endif
        }
        else if (_stricmp(mode, "vk") == 0 || _stricmp(mode, "vulkan") == 0)
        {
#if VULKAN_SUPPORTED
            deviceType = RENDER_DEVICE_TYPE_VULKAN;
#else
            return false;
#endif
        }
        else
        {
            return false;
        }
    }
    else
    {
        // Default priority: D3D12 > Vulkan > D3D11 > OpenGL
#if D3D12_SUPPORTED
        deviceType = RENDER_DEVICE_TYPE_D3D12;
#elif VULKAN_SUPPORTED
        deviceType = RENDER_DEVICE_TYPE_VULKAN;
#elif D3D11_SUPPORTED
        deviceType = RENDER_DEVICE_TYPE_D3D11;
#elif GL_SUPPORTED
        deviceType = RENDER_DEVICE_TYPE_GL;
#else
        return false;
#endif
    }

    return true;
}
```

### Block.h - Block Type System
```cpp
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
```

## Factory System Considerations

The current chunking system is designed with future factory automation in mind:

### 1. **Chunk-Based Processing**
- Factory machines can operate across chunk boundaries
- Background processing doesn't require chunk loading
- Headless factory ticking aligns with GDD requirements

### 2. **World Space Coordinates**
- 1 unit = 1 meter scale supports realistic factory sizing
- World space transformation allows precise multi-chunk structures
- Block-level granularity enables detailed automation

### 3. **Memory Management**
- Chunk unloading system prevents memory bloat
- Factory chunks can be marked "keep loaded" for active processing
- Efficient data structures support large factory complexes

### 4. **Extensible Block System**
- Block types can be extended for factory components
- Properties system supports machine behaviors
- Factory blocks can store additional metadata

## Key Lessons Learned

### 1. **World Space Transformation Critical**
The most important fix was ensuring chunks render in correct world positions:
```cpp
// In Chunk::BuildMesh() - convert local to world coordinates
float3 blockPos = float3(
    static_cast<float>(m_ChunkX * CHUNK_X_SIZE + x),
    static_cast<float>(m_ChunkY * CHUNK_Y_SIZE + y), 
    static_cast<float>(m_ChunkZ * CHUNK_Z_SIZE + z)
);
```

### 2. **Clear Constant Naming**
Renamed CHUNK_SIZE → CHUNK_X_SIZE, CHUNK_Y_SIZE, CHUNK_Z_SIZE for clarity when dealing with different dimensions.

### 3. **Progressive Chunk Loading**
Spherical chunk loading around player creates natural exploration pattern and memory management.

### 4. **Mouse Lock Integration**
Platform-specific cursor control requires careful window handle management for smooth user experience.

## Performance Characteristics

- **Chunk Generation**: ~1ms per 16³ chunk on modern hardware
- **Mesh Building**: Face culling reduces vertex count by ~80%
- **Memory Usage**: ~4KB per chunk for block data, variable for mesh data
- **Render Performance**: 60+ FPS with 3 chunk render distance

## Next Steps for Rewrite

1. **Improved Architecture**: Better separation of concerns
2. **Asset System**: Texture loading and management
3. **Block Interaction**: Click to break/place blocks
4. **Terrain Generation**: Replace simple stone with procedural world
5. **Factory Foundation**: Block metadata and multi-block structures
6. **Optimization**: GPU-based chunk rendering

This archive serves as both historical record and reference for rebuilding with improved design patterns and architecture.