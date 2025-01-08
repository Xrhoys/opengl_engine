#include <stdio.h>
#include <stdarg.h>
#include <GLES3/gl3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/em_math.h>
#include <pthread.h>
#include <unistd.h>
#include <emscripten/threading.h>
#include <bits/errno.h>
#endif

#define Assert(exp) \
if(!(exp)) __builtin_trap()

#include "game.h"
#include "engine.h"

#include "shader_resource.h"

#include "game.cpp"
#include "renderer.cpp"

#include "wasm_main.h"

PLATFORM_LOG(WASM_Log)
{
	va_list args;
    va_start(args, fmt);
	emscripten_console_logf(fmt, args);
	va_end(args);
}

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;

internal bool
WASM_RenderLoop(double dt, void* arg)
{
	platform_user_data* uData = (platform_user_data*)arg;
	platform_engine* engine = uData->engine;
	platform_render* render = uData->render;
	
	render_DrawScene(engine, render, 0.f);
	emscripten_webgl_commit_frame();
	
	return true;
}

internal void*
WASM_RenderThread(void* userData)
{
	platform_user_data* uData = (platform_user_data*)userData;
	platform_engine* engine = uData->engine;
	platform_render* render = uData->render;
	
	if(!engine || !render)
	{
		return NULL;
	}
	
	EmscriptenWebGLContextAttributes attrs;
	emscripten_webgl_init_context_attributes(&attrs);
#if 0
	attrs.alpha = false;
    attrs.depth = true;
    attrs.stencil = true;
    attrs.antialias = true;
    attrs.premultipliedAlpha = false;
    attrs.preserveDrawingBuffer = false;
    attrs.powerPreference = false;
    attrs.failIfMajorPerformanceCaveat = false;
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.enableExtensionsByDefault = true;
#endif
	//attrs.renderViaOffscreenBackBuffer = true;
	attrs.explicitSwapControl = true;
	//attrs.proxyContextToMainThread = EMSCRIPTEN_WEBGL_CONTEXT_PROXY_DISALLOW;
    
    ctx = emscripten_webgl_create_context("#canvas", &attrs);
    if(!ctx)
    {
        printf("Webgl ctx could not be created!\n");
        return NULL;
    }
	emscripten_webgl_make_context_current(ctx);

	emscripten_set_canvas_element_size("#canvas", 1280, 720);
	
	engine->vsSource = _shader_resource_vs;
	engine->psSource = _shader_resource_ps;
	
	// TODO: Make it dynamic for resizing
	engine->windowHeight = 800;
	engine->windowWidth = 600;
	
    render_Init(engine, render);
    
#if 1    
	char* version = (char*)glGetString(GL_VERSION);
    printf("WebGL verison: %s\n", version);
#endif
	
	emscripten_request_animation_frame_loop(WASM_RenderLoop, userData);
	
	return NULL;
}

internal void 
WASM_Loop(void* arg)
{
    platform_user_data* userData = (platform_user_data*)arg;
    
    platform_engine* engine = userData->engine;
    platform_render* render = userData->render;
    
    f32 t = emscripten_get_now();
    
    if(engine->gameUpdate)
    {
        engine->gameUpdate(engine, t);
    }
}

READ_ENTIRE_FILE(WASM_ReadEntireFile)
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
		
		res.size = length;
		res.data = writeCursor;
		fclose(fp);
	}
	else
	{
		// TODO: log
		emscripten_console_logf("Cannot open file: %s\n", filename);
	}
	
	return res;
}

WRITE_ENTIRE_FILE(WASM_WriteEntireFile)
{
    return 0;
}

int main() {
    platform_engine engine = {};
    platform_render render = {};
    platform_user_data userData = { &engine, &render };
    
	engine.running = true;
	
	platform_memory* memory = &engine.memory;
    {
		memory->permanentStorage.size = Megabyte(256);
        memory->permanentStorage.used = 0;
        memory->permanentStorage.base = (u8*)malloc(memory->permanentStorage.size);
        
        memory->transientStorage.size = Gigabytes(2);
        memory->transientStorage.used = 0;
        memory->transientStorage.base = (u8*)malloc(memory->transientStorage.size);
		
        memory->readFile  = WASM_ReadEntireFile;
        memory->writeFile = WASM_WriteEntireFile;
        
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		emscripten_pthread_attr_settransferredcanvases(&attr, "#canvas");
		
		pthread_t thread;
		int rc = pthread_create(&thread, &attr, WASM_RenderThread, (void*)&userData);
		if(rc == ENOSYS)
		{
			printf("Offscreen canvas not supported!\n");
		}
		emscripten_set_thread_name(thread, "Rendering thread");
		pthread_detach(thread);
        
		// TEMP:
		engine.gameInit = game_Init;
        engine.gameUpdate = game_Update;
        if(engine.gameInit)
        {
            engine.gameInit(&engine);
        }
    }

    emscripten_set_main_loop_arg(WASM_Loop, &userData, 0, true);
    
    //render_Cleanup(&engine, &render);
    return 1;
}
