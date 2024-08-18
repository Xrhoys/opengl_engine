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
	
	HWND hwnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP,
							   wc.lpszClassName, "Engine",
							   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
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
        memory->permanentStorage.base = (u8*)malloc(memory->permanentStorage.size);
        
        memory->transientStorage.size = Gigabytes(2);
        memory->transientStorage.used = 0;
        memory->transientStorage.base = (u8*)malloc(memory->transientStorage.size);
        
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
	
    // Init OPENGL
    HDC dc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    
    i32 pixelFormat = ChoosePixelFormat(dc, &pfd);
    i32 res = SetPixelFormat(dc, pixelFormat, &pfd);
    Assert(res);
    
    HGLRC tempRC = wglCreateContext(dc);
	wglMakeCurrent(dc, tempRC);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = 
    (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    i32 attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_COREPROFILE_BIT_ARB, 0, 
	};
    HGLRC glContext = wglCreateContextAttribsARB(dc, 0, attribList);
    
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempRC);
	wglMakeCurrent(dc, glContext);
    
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
			printf("VSynch enabled \n");
		}
		else
		{
			printf("Could not enable VSynch\n");
		}
	}
	else
	{
		printf("WGL_EXT_swap_control not supported \n");
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
        if (vsynch != 0) glFinish();
    }
    
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    
    return 0;
}
