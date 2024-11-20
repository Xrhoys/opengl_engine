#include <jni.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#include <string.h>

#define Assert(exp) \
if(!(exp)) {*(int*)0 = 0;}

#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Engine", __VA_ARGS__))

#include "game.h"
#include "engine.h"

#include "shader_resource.h"

#include "game.cpp"
#include "renderer.cpp"

#include "android_main.h"

PLATFORM_LOG(android_Log)
{
	va_list args;
    va_start(args, fmt);
	LOG(fmt, args);
	va_end(args);
}

READ_ENTIRE_FILE(android_ReadEntireFile)
{
	read_file_res res = {};
    FILE *fp = fopen(filename, "rb");
	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		u64 size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		
		u8* writeCursor = arena_PushArray(memory, size, u8);
		u64 length = fread(writeCursor, 1, size, fp);
		
		res.size = size;
		res.data = writeCursor;
		fclose(fp);
	}
	else
	{
		// TODO: log
		LOG("Cannot open file: %s\n", filename);
	}
	
	return res;
}

internal i32 
android_InitDisplay(platform_user_data* userData)
{
	platform_engine* engine = userData->engine;
	platform_render* render = userData->render;
    const EGLint attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE,
    };
	
    EGLDisplay display;
    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
    {
        LOG("error with eglGetDisplay");
        return -1;
    }
	
    if (!eglInitialize(display, 0, 0))
    {
        LOG("error with eglInitialize");
        return -1;
    }
	
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
    {
        LOG("error with eglChooseConfig");
        return -1;
    }
	
    EGLint format;
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        LOG("error with eglGetConfigAttrib");
        return -1;
    }
	
    ANativeWindow_setBuffersGeometry(userData->app->window, 0, 0, format);
	
    EGLSurface surface;
    if (!(surface = eglCreateWindowSurface(display, config, userData->app->window, NULL)))
    {
        LOG("error with eglCreateWindowSurface");
        return -1;
    }
	
    const EGLint ctx_attrib[] = { 
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 2,
#ifdef DEBUG
		EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
		EGL_NONE,
	};
    EGLContext context;
    if (!(context = eglCreateContext(display, config, NULL, ctx_attrib)))
    {
        LOG("error with eglCreateContext");
        return -1;
    }
	
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        LOG("error with eglMakeCurrent");
        return -1;
    }
	
    LOG("GL_VENDOR = %s", glGetString(GL_VENDOR));
    LOG("GL_RENDERER = %s", glGetString(GL_RENDERER));
    LOG("GL_VERSION = %s", glGetString(GL_VERSION));
	
    EGLint w, h;
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	
    userData->display = display;
    userData->context = context;
    userData->surface = surface;
    engine->windowWidth = w;
    engine->windowHeight = h;
	
	platform_memory* memory = &engine->memory;
	memory->permanentStorage.size = Megabyte(256);
	memory->permanentStorage.used = 0;
	memory->permanentStorage.base = (u8*)malloc(memory->permanentStorage.size);
	
	memory->transientStorage.size = Gigabytes(2);
	memory->transientStorage.used = 0;
	memory->transientStorage.base = (u8*)malloc(memory->transientStorage.size);
	
	memory->readFile  = android_ReadEntireFile;
	//memory->writeFile = Win32WriteEntireFile;
	
	engine->gameInit = game_Init;
	engine->gameUpdate = game_Update;
	if(engine->gameInit)
	{
		engine->gameInit(engine);
	}
	engine->log = android_Log;
	engine->vsSource = _shader_resource_vs;
	engine->psSource = _shader_resource_ps;
	
	render_Init(engine, render);
	
	// Test extensions
	{
		GLint n = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		for(u32 index = 0;
			index < n;
			++index)
		{
			char* extension = (char*)glGetStringi(GL_EXTENSIONS, index);
			LOG("%s\n", extension);
		}
		
		void* glGetTextureHandleARB = (void*)eglGetProcAddress("glGetTextureHandleARB");
		if(glGetTextureHandleARB)
		{
			LOG("FOUND POINTER\n");
		}
		else
		{
			LOG("NOT FOUND\n");
		}
	}
	
    return 0;
}

internal void
android_DrawFrame(platform_user_data* userData)
{
	LOG("Start frame");
	if(userData->display == NULL)
	{
		return;
	}
	
	platform_engine* engine = userData->engine;
	platform_render* render = userData->render;
	
	// TODO: replace delta timer
	render_DrawScene(engine, render, 0.0f);
    
	eglSwapBuffers(userData->display, userData->surface);
}

internal void 
android_TerminateDisplay(platform_user_data* userData)
{
	platform_engine* engine = userData->engine;
    if (userData->display != EGL_NO_DISPLAY)
    {
        //glDeleteProgram(engine->shader);
        //glDeleteBuffers(1, &engine->buffer);
		
        eglMakeCurrent(userData->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (userData->context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(userData->display, userData->context);
        }
        if (userData->surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(userData->display, userData->surface);
        }
        eglTerminate(userData->display);
    }
    userData->active = false;
    userData->display = EGL_NO_DISPLAY;
    userData->context = EGL_NO_CONTEXT;
    userData->surface = EGL_NO_SURFACE;
}

internal i32
android_HandleInput(android_app* app, AInputEvent* event)
{
    return 0;
}

internal void 
android_HandleMessage(android_app* app, i32 cmd)
{
    platform_user_data* userData = (platform_user_data*)app->userData;
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW:
		{
			if (userData->app->window != NULL)
			{
				android_InitDisplay(userData);
				android_DrawFrame(userData);
			}	
		}break;
		
        case APP_CMD_TERM_WINDOW:
		{
			android_TerminateDisplay(userData);
		}break;
        
        case APP_CMD_GAINED_FOCUS:
		{
			userData->active = true;
		}
		
        case APP_CMD_LOST_FOCUS:
		{
			userData->active = false;
			android_DrawFrame(userData);
		}break;
    }
}

void 
android_main(struct android_app* state)
{
	platform_user_data userData = {};
    platform_engine engine = {};
	platform_render render = {};
	userData.engine = &engine;
	userData.render = &render;
	
    state->userData = &userData;
    state->onAppCmd = android_HandleMessage;
    state->onInputEvent = android_HandleInput;
    userData.app = state;
	
    while (1)
    {
        i32 ident;
        i32 events;
        android_poll_source* source;
		
        while ((ident=ALooper_pollAll(userData.active ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
        {
            if (source != NULL)
            {
                source->process(state, source);
            }
			
            if (state->destroyRequested != 0)
            {
                android_TerminateDisplay(&userData);
                return;
            }
        }
		
        if (userData.active)
        {
			
            android_DrawFrame(&userData);
        }
    }
}
