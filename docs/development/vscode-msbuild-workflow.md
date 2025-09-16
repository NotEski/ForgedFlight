# VS Code + Visual Studio CLI Development Setup

## 🚀 Perfect Hybrid Workflow

**Edit in VS Code** → **Build with Visual Studio CLI** → **Debug in Visual Studio**

## 🔧 Command Line Build Setup

### Method 1: MSBuild (Recommended)
```bash
# Build Debug configuration
msbuild build\ForgedFlight.sln /p:Configuration=Debug /p:Platform=x64

# Build Release configuration  
msbuild build\ForgedFlight.sln /p:Configuration=Release /p:Platform=x64

# Build specific project only
msbuild build\apps\client\ff_client.vcxproj /p:Configuration=Debug /p:Platform=x64

# Clean and rebuild
msbuild build\ForgedFlight.sln /t:Clean /p:Configuration=Debug /p:Platform=x64
msbuild build\ForgedFlight.sln /t:Rebuild /p:Configuration=Debug /p:Platform=x64
```

### Method 2: Developer Command Prompt
```bash
# Initialize Visual Studio environment (run once per session)
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

# Then use simpler commands
msbuild build\ForgedFlight.sln /p:Configuration=Debug
```

### Method 3: DevEnv (Visual Studio IDE automation)
```bash
# Build solution
devenv build\ForgedFlight.sln /build Debug

# Build and run
devenv build\ForgedFlight.sln /build Debug /project ff_client /projectconfig Debug
```

## 📋 VS Code Integration

### Add to tasks.json
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Debug",
            "type": "shell",
            "command": "msbuild",
            "args": [
                "build/ForgedFlight.sln",
                "/p:Configuration=Debug",
                "/p:Platform=x64",
                "/m"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared",
                "showReuseMessage": true,
                "clear": false
            },
            "problemMatcher": "$msCompile"
        },
        {
            "label": "Build Release",
            "type": "shell",
            "command": "msbuild",
            "args": [
                "build/ForgedFlight.sln",
                "/p:Configuration=Release",
                "/p:Platform=x64",
                "/m"
            ],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": "$msCompile"
        },
        {
            "label": "Clean",
            "type": "shell",
            "command": "msbuild",
            "args": [
                "build/ForgedFlight.sln",
                "/t:Clean",
                "/p:Configuration=Debug",
                "/p:Platform=x64"
            ],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": "$msCompile"
        },
        {
            "label": "Rebuild",
            "type": "shell",
            "command": "msbuild",
            "args": [
                "build/ForgedFlight.sln",
                "/t:Rebuild",
                "/p:Configuration=Debug",
                "/p:Platform=x64",
                "/m"
            ],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": "$msCompile"
        },
        {
            "label": "Run Debug",
            "type": "shell",
            "command": "${workspaceFolder}/build/apps/client/Debug/ForgedFlight.exe",
            "group": "test",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": true,
                "panel": "shared"
            },
            "dependsOn": "Build Debug"
        }
    ]
}
```

### Add to launch.json (for debugging)
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug ForgedFlight",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/apps/client/Debug/ForgedFlight.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "console": "externalTerminal",
            "preLaunchTask": "Build Debug"
        }
    ]
}
```

## ⚡ Quick Commands

### Build Commands
- **Ctrl+Shift+P** → "Tasks: Run Task" → "Build Debug"
- **Ctrl+Shift+B** → Run default build task
- **F5** → Build and debug

### Terminal Commands
```bash
# Quick build
msbuild build\ForgedFlight.sln /p:Configuration=Debug /p:Platform=x64 /m

# Quick run
.\build\apps\client\Debug\ForgedFlight.exe

# Build and run in one command
msbuild build\ForgedFlight.sln /p:Configuration=Debug /p:Platform=x64 /m && .\build\apps\client\Debug\ForgedFlight.exe
```

## 🎯 Development Workflow

### Daily Development
1. **Edit in VS Code** - Best editor experience
2. **Build with Ctrl+Shift+B** - Uses MSBuild via VS Code tasks
3. **Run with F5** - Automatic build and debug
4. **Complex debugging** - Open in Visual Studio when needed

### Performance Benefits
- **Faster than CMake** - Direct MSBuild is more efficient
- **Better error parsing** - VS Code understands MSBuild output
- **Incremental builds** - Only rebuilds changed files
- **Parallel compilation** - `/m` flag uses all CPU cores

## 🛠️ MSBuild Tips

### Faster Builds
```bash
# Use all CPU cores
msbuild build\ForgedFlight.sln /m

# Minimal rebuild (only changed files)
msbuild build\ForgedFlight.sln /p:Configuration=Debug /m:4

# Verbose output (for debugging build issues)
msbuild build\ForgedFlight.sln /v:detailed
```

### Build Specific Projects
```bash
# Build only TheForge library
msbuild build\apps\client\TheForge.vcxproj /p:Configuration=Debug

# Build only client app
msbuild build\apps\client\ff_client.vcxproj /p:Configuration=Debug
```

## 🚀 Advantages of This Setup

### VS Code Benefits
- **Superior editing** - Better for large codebases
- **Git integration** - Excellent source control
- **Extensions** - C++ IntelliSense, GitLens, etc.
- **Terminal integration** - Built-in PowerShell

### Visual Studio Build Benefits  
- **Native TheForge support** - Optimized for DirectX development
- **Better error messages** - More detailed compilation errors
- **Graphics debugging** - When you need to debug rendering issues
- **Professional toolchain** - Industry standard for game development

---

**Result:** Best of both worlds - VS Code's editing experience with Visual Studio's build power!
