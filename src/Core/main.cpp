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

    try
    {        
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
            CoUninitialize();
            return -1;
        }

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

        // Initialize the application
        NativeAppInitAttrib initAttrib;
        initAttrib.hWnd = hWnd;
        initAttrib.DeviceType = deviceType;

        g_pTheApp->Initialize(initAttrib);

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

        CoUninitialize();

        return static_cast<int>(msg.wParam);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return -1;
    }
    catch (...)
    {
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
