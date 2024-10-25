#define COBJMACROS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN_32_EXTRA_LEAN

#include <windows.h>
#include <glad/glad.h>
#include <glad/glad.c>
#include <hidusage.h>
#include <xaudio2.h>

#define Assert(exp) \
if(!(exp)) {*(int*)0 = 0;}

#include "game.h"
#include "engine.h"

#include "shader_resource.h"

#include "game.cpp"
#include "renderer.cpp"

#include "win32_main.h"

global_variable PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
global_variable PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
global_variable PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

internal void 
FatalError(const char* message)
{
    MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
    ExitProcess(0);
}

PLATFORM_LOG(Win32Log)
{
	OutputDebugStringA(fmt);
}

READ_ENTIRE_FILE(Win32ReadEntireFile)
{
	read_file_res res = {};
	
    u32 bytesRead;
    u32 fileSize32;
    HANDLE fileHandle = CreateFileA(filename,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    0,
                                    OPEN_EXISTING,
                                    0,
                                    0);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if(GetFileSizeEx(fileHandle, &fileSize))
        {
            // NOTE: Will cause problem for 64bits
            fileSize32 = (u32)fileSize.QuadPart;
			u8* ptr = memory->base + memory->used;
			arena_PushSize_(memory, fileSize32);
            
            if(ptr)
            {
                if(ReadFile(fileHandle, ptr, fileSize32, (LPDWORD)&bytesRead, 0))
                {
					res.data = ptr;
					res.size = fileSize32;
                }
				else
				{
                    // Release arena
				}
            }
            else
            {
                // TODO: Logging
            }
        }
        else
        {
            // TODO: Logging
        }
		
		CloseHandle(fileHandle);
    }
    else
    {
        // TODO: Logging
    }
	
	return res;
}

WRITE_ENTIRE_FILE(Win32WriteEntireFile)
{
    return 0;
}

inline f64
Win32UpdateClock(LARGE_INTEGER* lastTimer, u64 freq)
{
    LARGE_INTEGER c2;
	QueryPerformanceCounter(&c2);
	f64 delta = (f64)(c2.QuadPart - lastTimer->QuadPart) / (f64)freq;
    *lastTimer = c2;
    
    return delta;
}

LRESULT WINAPI
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
		case WM_MOVING:
		{
			
		}break;
		
		case WM_MOVE:
		{
			
		}break;
		
        case WM_SYSCOMMAND:
        {
            if ((wParam & 0xfff0) == SC_KEYMENU) // NOTE: Disable ALT application menu
                return 0;
        } break;
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        } break;
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

internal void
Win32GetWglFunctions()
{
	// to get WGL functions we need valid GL context, so create dummy window for dummy GL context
    HWND dummy = CreateWindowExW(
								 0, L"STATIC", L"DummyWindow", WS_OVERLAPPED,
								 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								 NULL, NULL, NULL, NULL);
    Assert(dummy && "Failed to create dummy window");
	
    HDC dc = GetDC(dummy);
    Assert(dc && "Failed to get device context for dummy window");
	
    PIXELFORMATDESCRIPTOR desc = {};
	desc.nSize = sizeof(desc);
	desc.nVersion = 1;
	desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	desc.iPixelType = PFD_TYPE_RGBA;
	desc.cColorBits = 24;
	
    int format = ChoosePixelFormat(dc, &desc);
    if (!format)
    {
        FatalError("Cannot choose OpenGL pixel format for dummy window!");
    }
	
    int ok = DescribePixelFormat(dc, format, sizeof(desc), &desc);
    Assert(ok && "Failed to describe OpenGL pixel format");
	
    // reason to create dummy window is that SetPixelFormat can be called only once for the window
    if (!SetPixelFormat(dc, format, &desc))
    {
        FatalError("Cannot set OpenGL pixel format for dummy window!");
    }
	
    HGLRC rc = wglCreateContext(dc);
    Assert(rc && "Failed to create OpenGL context for dummy window");
	
    ok = wglMakeCurrent(dc, rc);
    Assert(ok && "Failed to make current OpenGL context for dummy window");
	
    // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_extensions_string.txt
    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
	(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
    if (!wglGetExtensionsStringARB)
    {
        FatalError("OpenGL does not support WGL_ARB_extensions_string extension!");
    }
	
    const char* ext = wglGetExtensionsStringARB(dc);
    Assert(ext && "Failed to get OpenGL WGL extension string");
	
    const char* start = ext;
    for (;;)
    {
        while (*ext != 0 && *ext != ' ')
        {
            ext++;
        }
		
        size_t length = ext - start;
        if (StringsAreEqual("WGL_ARB_pixel_format", start, length))
        {
            // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
            wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        }
        else if (StringsAreEqual("WGL_ARB_create_context", start, length))
        {
            // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
            wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        }
        else if (StringsAreEqual("WGL_EXT_swap_control", start, length))
        {
            // https://www.khronos.org/registry/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt
            wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        }
		
        if (*ext == 0)
        {
            break;
        }
		
        ext++;
        start = ext;
    }
	
    if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB || !wglSwapIntervalEXT)
    {
        FatalError("OpenGL does not support required WGL extensions for modern context!");
    }
	
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(rc);
    ReleaseDC(dummy, dc);
    DestroyWindow(dummy);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
	Win32GetWglFunctions();
	
	WNDCLASSEX wc = 
    { 
        sizeof(WNDCLASSEX), 
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, 
        WndProc, 
        0L, 0L, 
        GetModuleHandle(NULL), 
        NULL, NULL, NULL, NULL, 
        "Engine", 
        NULL 
    };
	
	RegisterClassEx(&wc);
	
    HWND hwnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP,
                               wc.lpszClassName, "Engine",
                               WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL, NULL, wc.hInstance, NULL);
    
    i32 width = CW_USEDEFAULT;
    i32 height = CW_USEDEFAULT;
    
    platform_engine engine = {};
    platform_render render = {};
    
    engine.running = true;
    
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    
    {
        platform_memory* memory = &engine.memory;
        memory->permanentStorage.size = Megabyte(256);
        memory->permanentStorage.used = 0;
        memory->permanentStorage.base = (u8*)VirtualAlloc(0, memory->permanentStorage.size, 
														  MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        
        memory->transientStorage.size = Gigabytes(2);
        memory->transientStorage.used = 0;
        memory->transientStorage.base = (u8*)VirtualAlloc(0, memory->transientStorage.size, 
														  MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        
        memory->readFile  = Win32ReadEntireFile;
        memory->writeFile = Win32WriteEntireFile;
        
        // TEMP:
        engine.gameInit = game_Init;
        engine.gameUpdate = game_Update;
        if(engine.gameInit)
        {
            engine.gameInit(&engine);
        }
        
        engine.log = Win32Log;
        engine.vsSource = _shader_resource_vs46;
        engine.psSource = _shader_resource_ps46;
    }
	
    HDC dc = GetDC(hwnd);
	{
		i32 pixelAttribs[] = 
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     24,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
			0,
		};
		
		i32 pixelFormatID;
		u32 numFormats;
		b32 status = wglChoosePixelFormatARB(dc, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
		
		if(status == false || numFormats == 0)
		{
			return 1;
		}
		
		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(dc, pixelFormatID, sizeof(pfd), &pfd);
		SetPixelFormat(dc, pixelFormatID, &pfd);
	}
	
	{
		i32 major_min = 4, minor_min = 5;
		i32 contextAttribs[] = 
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef DEBUG
            // ask for debug context for non "Release" builds
            // this is so we can enable debug callback
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
            0,
		};
		
		HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
		if(!rc)
		{
			return 1;
		}
		
		if(!wglMakeCurrent(dc, rc))
		{
			return 1;
		}
	}
    
    if(!gladLoadGL())
    {
        return -1;
    }
    
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = 
	(PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
    b32 swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;
    i32 vsynch = 0;
    
    if (swapControlSupported) {
		PFNWGLSWAPINTERVALEXTPROC wglSwapInternalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = 
		(PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
		if (wglSwapInternalEXT(1))
		{
			OutputDebugStringA("VSynch enabled \n");
		}
		else
		{
			OutputDebugStringA("Could not enable VSynch\n");
		}
	}
	else
	{
		OutputDebugStringA("WGL_EXT_swap_control not supported \n");
	}
    
    render_Init(&engine, &render);
    
    LARGE_INTEGER clock;
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&clock);
    
    WINDOWPLACEMENT plac;
    GetWindowPlacement(hwnd, &plac);
    engine.posX = plac.rcNormalPosition.left;
    engine.posY = plac.rcNormalPosition.top;
    
    while(engine.running)
    {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            switch (msg.message)
            {
                case WM_LBUTTONDOWN:
                {
                }break;
                case WM_LBUTTONUP:
                {
                }break;
                
                case WM_QUIT:
                {
                    engine.running = false;
                }break;
            }
            
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        
        RECT rect;
        GetClientRect(hwnd, &rect);
        width = rect.right - rect.left;
		height = rect.bottom - rect.top;
        
        f64 delta = Win32UpdateClock(&clock, freq.QuadPart);
        engine.time += delta;
        
        if(width != engine.windowWidth || height != engine.windowHeight)
        {
            render_Resize(&engine, width, height);
        }
        
        render_DrawScene(&engine, &render, delta);
        
        //render_EndFrame(&engine);
        SwapBuffers(dc);
        //if (vsynch != 0) glFinish();
    }
    
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    
    return 0;
}
