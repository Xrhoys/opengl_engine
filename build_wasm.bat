@echo off
@set SOURCE=wasm_main.cpp
@set OUTPUT_DIR=build/wasm
@set OUTPUT=app_wasm.js
@set FLAGS=-O3 -sMIN_WEBGL_VERSION=2 -sWASM=1 -sMALLOC=emmalloc -sALLOW_MEMORY_GROWTH=1 -Wno-writable-strings

call build_env.bat
call "%EMSDK_PATH%"

rmdir /S /Q build\wasm\
xcopy /S wasm\ build\wasm\
emcc %FLAGS% %SOURCE% -o %OUTPUT_DIR%/%OUTPUT% --embed-file assets