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

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
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
	
    HWND dummyWND = CreateWindow(wc.lpszClassName, "Fake Window",
								 WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								 0, 0,
								 1, 1,
								 NULL, NULL,         
								 Instance, NULL);
	
    HDC dummy = GetDC(dummyWND);
	
    PIXELFORMATDESCRIPTOR dummyPFD = {};
    dummyPFD.nSize = sizeof(dummyPFD);
    dummyPFD.nVersion = 1;
    dummyPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    dummyPFD.iPixelType = PFD_TYPE_RGBA;
    dummyPFD.cColorBits = 32;
    dummyPFD.cAlphaBits = 8;
    dummyPFD.cDepthBits = 24;
	
    i32 dummyPFDID = ChoosePixelFormat(dummy, &dummyPFD);
    if(dummyPFDID == 0) 
    {
        return 1;
    }
	
    if(!SetPixelFormat(dummy, dummyPFDID, &dummyPFD))
    {
        return 1;
    }
	
    HGLRC dummyRC = wglCreateContext(dummy);
	
    if(!dummyRC)
    {
        return 1;
    }
    
    if(!wglMakeCurrent(dummy, dummyRC))
    {
        return 1;
    }
	
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));
    if (wglChoosePixelFormatARB == NULL) 
    {
        return 1;
    }
	
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)(wglGetProcAddress("wglCreateContextAttribsARB"));
    if (wglCreateContextAttribsARB == NULL) 
    {
        return 1;
    }
	
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
	
    i32 pixelAttribs[] = 
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
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
	
    i32 major_min = 4, minor_min = 5;
    i32 contextAttribs[] = 
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
	
    HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
    if(!rc)
    {
        return 1;
    }
    
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(dummyRC);
    ReleaseDC(dummyWND, dummy);
    DestroyWindow(dummyWND);
	if(!wglMakeCurrent(dc, rc))
    {
        return 1;
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
        if (vsynch != 0) glFinish();
    }
    
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    
    return 0;
}
