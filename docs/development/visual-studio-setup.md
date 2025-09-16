# Visual Studio Development Setup

**Date:** September 11, 2025  
**Status:** Transitioning from CMake to Visual Studio for better TheForge integration

## 🎯 Why Visual Studio?

Visual Studio provides superior support for TheForge development:
- **Native TheForge Support** - All examples use VS projects
- **Better Graphics Debugging** - DirectX 12 debugging tools
- **Integrated IntelliSense** - Better C++ code completion
- **Simpler Build Process** - Less configuration needed

## 🚀 Quick Start

### 1. Open Solution
```bash
# From project root
start build\ForgedFlight.sln
```

### 2. Build Configuration
- **Configuration:** Debug
- **Platform:** x64
- **Startup Project:** ff_client

### 3. Build and Run
- **Build:** Ctrl+Shift+B or Build → Build Solution
- **Run:** F5 or Debug → Start Debugging
- **Run without Debug:** Ctrl+F5

## 📁 Project Structure in Visual Studio

```
ForgedFlight.sln
├── ff_client (Startup Project)
│   ├── main.cpp - ForgedFlightApp implementation
│   └── Dependencies
│       └── TheForge - Static library
├── ff_engine
│   └── engine source files
└── TheForge
    ├── OS/Windows - Platform layer
    ├── Graphics/Direct3D12 - Renderer
    └── Utilities - Core utilities
```

## 🔧 Key Visual Studio Features for TheForge

### Graphics Debugging
- **PIX Integration** - GPU profiling and debugging
- **DirectX Debug Layer** - Runtime validation
- **Shader Debugging** - Step through shader code

### IntelliSense Benefits
- **TheForge API Discovery** - Autocomplete for all interfaces
- **Parameter Hints** - Function signatures and documentation
- **Go to Definition** - Navigate TheForge source easily

### Build Advantages
- **Parallel Compilation** - Faster builds than CMake
- **Incremental Builds** - Only rebuild changed files
- **Error Integration** - Click errors to jump to code

## 🎯 Current Status

### ✅ Ready Components
- **IApp Skeleton** - Complete ForgedFlightApp class
- **Build System** - Visual Studio solution configured
- **TheForge Integration** - Source files linked properly
- **Platform Setup** - Windows/DirectX 12 configured

### 🚧 Next Steps in Visual Studio
1. **Build the Solution** - Test current skeleton compilation
2. **Add TheForge Headers** - Include graphics interfaces properly
3. **Implement Init()** - Initialize renderer following 01_Transformations
4. **Add Clear Color** - Get purple screen rendering

### 🔍 Development Workflow
1. **Edit in VS Code** - Continue using VS Code for editing (better for this project structure)
2. **Build in Visual Studio** - Use VS for compilation and debugging
3. **Debug in Visual Studio** - Use VS debugging tools for graphics
4. **Version Control** - Continue using Git through VS Code

## 🛠️ Troubleshooting

### Build Errors
- **Clean Solution** - Build → Clean Solution
- **Rebuild All** - Build → Rebuild Solution
- **Check Platform** - Ensure x64 is selected

### TheForge Issues
- **Missing Headers** - Check include paths in project properties
- **Link Errors** - Verify TheForge static library is building
- **Runtime Errors** - Use Visual Studio debugger to diagnose

### Performance
- **Release Build** - Switch to Release for performance testing
- **Profiling** - Use Visual Studio Performance Profiler
- **GPU Debugging** - Use PIX for Windows for graphics profiling

## 📚 Resources

### Visual Studio Extensions
- **Graphics Debugger** - Built into VS 2019/2022
- **PIX for Windows** - Download separately for advanced GPU debugging
- **HLSL Tools** - Enhanced shader editing

### TheForge References
- **Examples Location:** `external/TheForge/Examples_3/Unit_Tests/`
- **Key Example:** `01_Transformations` - Basic renderer setup
- **Documentation:** TheForge GitHub wiki and source comments

---

**Next Action:** Open Visual Studio solution and test basic compilation
