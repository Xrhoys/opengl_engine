@echo off
@set OUT_DIR=build\win32
@set OUT_EXE=game
@set INCLUDES=/Iinclude\
@set SOURCES=win32_main.cpp
@set LIBS=User32.lib gdi32.lib Xaudio2.lib opengl32.lib
@set FLAGS=-wd4091 -wd4293 /Fd%OUT_DIR%\ -DDEBUG
IF NOT EXIST %OUT_DIR%\ MKDIR %OUT_DIR%

if not defined DevEnvDir (
	call build_env.bat
	call "%MSVC_VARS_PATH%"
)

cl /nologo /Zi /EHsc %FLAGS% %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS% 