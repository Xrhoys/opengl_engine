# Multiplatform OpenGL Engine
Portable OpenGL based game engine.

Currently supported target:
- Win32
- Android
- Wasm WebGL

## Build

Copy `build_env.bat.sample` and name it `build_env.bat`.
Fill path to various local SDKs or .bat environment loading files locations.

### Windows
MSVC: `vcvars64.bat`  
Launch `build_win32.bat`

### Android
Paths to NDK and SDK + Java  
Launch `build_android.bat`

### WASM
Emscripten: `emsdk_env.bat`  
Launch `build_wasm.bat`
