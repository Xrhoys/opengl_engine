@echo off
@set SOURCE=wasm_main.cpp
@set OUTPUT_DIR=build/wasm
@set OUTPUT=app_wasm.js
@set INCLUDES=-I..\..\Lib\opus-1.5.2\src\ -I..\..\Lib\opus-1.5.2\include\ -I..\..\Lib\opus-1.5.2\celt\ -I..\..\Lib\opusfile-0.12\include\ -I..\..\Lib\opusfile-0.12\src\ -I..\..\Lib\libogg-1.3.5\include\ 
@set FLAGS=-sMIN_WEBGL_VERSION=2 -sWASM=1 -sINITIAL_MEMORY=536870912 -sMALLOC=emmalloc -Wno-writable-strings -sAUDIO_WORKLET=1 -sWASM_WORKERS=1 -g -gseparate-dwarf=%OUTPUT_DIR%/app_wasm.debug.wasm -sSEPARATE_DWARF_URL=http://localhost:8000/%OUTPUT_DIR%/app_wasm.debug.wasm -pthread -sPTHREAD_POOL_SIZE=2

call build_env.bat
call "%EMSDK_PATH%"

rmdir /S /Q build\wasm\
xcopy /S wasm\ build\wasm\
emcc %FLAGS% %INCLUDES% %SOURCE% -L..\..\Lib\opus-1.5.2\lib -lopus -L..\..\Lib\libogg-1.3.5\lib -logg -o %OUTPUT_DIR%/%OUTPUT% --embed-file assets 