#include <stdio.h>
#include <stdarg.h>
#include <GLES3/gl3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/webaudio.h>
#include <emscripten/em_math.h>
#endif

#define Assert(exp) \
if(!(exp)) __builtin_trap()

#include "game.h"
#include "engine.h"

#include "shader_resource.h"

#include "game.cpp"
#include "renderer.cpp"

// OPUS test stuff
#include <opus.h>
#include <opusfile.h>
#include <opusfile.c>
#include <info.c>
#include <internal.c>
#include <stream.c>

#include "wasm_main.h"

PLATFORM_LOG(WASM_Log)
{
	va_list args;
    va_start(args, fmt);
	emscripten_console_logf(fmt, args);
	va_end(args);
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
    
    render_DrawScene(engine, render, t);
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

internal bool
AudioRenderTrack(i32 numInputs, const AudioSampleFrame *inputs,
				 i32 numOutputs, AudioSampleFrame *outputs,
				 i32 numParams, const AudioParamFrame *params,
				 void *userData)
{
	platform_user_data* engine = (platform_user_data*)userData;
	wasm_audio* audio = &engine->audio;
	
	if(audio->readIndex > audio->sampleSize)
		return false;
	
	u32 sampleCount = Minimum(audio->sampleSize - audio->readIndex, 128);
	f32* cursor = &engine->audio.pcmBuffer[audio->readIndex];
	for(u32 outputIndex = 0;
		outputIndex < numOutputs;
		++outputIndex)
	{
		for(u32 index = 0;
			index < sampleCount;
			++index)
		{
			outputs[outputIndex].data[index] = *cursor++;
			outputs[outputIndex].data[index + 128] = *cursor++;
		}
	}
		
	audio->readIndex += sampleCount * 2;
	
	return true;
}

internal bool
GenerateNoise(i32 numInputs, const AudioSampleFrame *inputs,
			  i32 numOutputs, AudioSampleFrame *outputs,
			  i32 numParams, const AudioParamFrame *params,
			  void *userData)
{
	// Warning: scale down audio volume by factor of 0.2, raw noise can be really loud otherwise
	for(u32 i = 0; i < numOutputs; ++i)
		for(u32 j = 0; j < outputs[i].samplesPerChannel*outputs[i].numberOfChannels; ++j)
		outputs[i].data[j] = emscripten_random() * 0.2 - 0.1; 
	
	return true; // Keep the graph output going
}

internal bool
OnCanvasClick(i32 eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
	EMSCRIPTEN_WEBAUDIO_T audioContext = (EMSCRIPTEN_WEBAUDIO_T)userData;
	if (emscripten_audio_context_state(audioContext) != AUDIO_CONTEXT_STATE_RUNNING) {
		emscripten_resume_audio_context_sync(audioContext);
	}
	
	return false;
}

internal void
AudioWorkletProcessorCreated(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData)
{
	if(!success) return;
	
	platform_user_data* engine = (platform_user_data*)userData;
	
	i32 outputChannelCounts[] = { 2 }; 
	EmscriptenAudioWorkletNodeCreateOptions options =
	{
		.numberOfInputs = 0,
		.numberOfOutputs = 1,
		.outputChannelCounts = outputChannelCounts
	};
	
	if(!engine->audio.sample) return;
	
	// Decode the whole track
	i32 ret = 0;
	u64 offset = 0;
	do 
	{
		ret = op_read_float_stereo(engine->audio.sample, engine->audio.pcmBuffer + offset, 
								   engine->audio.bufferSize - offset);
		offset += ret * 2;
		engine->audio.sampleSize += ret;
	}while(ret > 0);
	
	
	EMSCRIPTEN_AUDIO_WORKLET_NODE_T wasmAudioWorklet = 
		emscripten_create_wasm_audio_worklet_node(audioContext, "noise-generator", &options, &AudioRenderTrack, userData);
	
	// Connect it to audio context destination
	emscripten_audio_node_connect(wasmAudioWorklet, audioContext, 0, 0);
	
	// Resume context on mouse click
	emscripten_set_click_callback("canvas", (void*)audioContext, 0, OnCanvasClick);
}

internal void 
AudioThreadInitialized(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData)
{
	if(!success) return;
	
	platform_user_data* engine = (platform_user_data*)userData;
	platform_memory* memory = &engine->engine->memory;
	wasm_audio* audio = &engine->audio;
	
	read_file_res opusFile = memory->readFile(NULL, "assets/sample.opus", &memory->permanentStorage);
	if(opusFile.size > 0)
	{
		printf("FILE LOADED");
	}
	
	i32 error;
	audio->sample = op_open_memory(opusFile.data, opusFile.size,& error);
	OggOpusFile* oggRef = audio->sample;
	if(oggRef)
	{
		printf("LOADED");
	}
	
	i64 size = 0, duration = 0;
	if(op_seekable(oggRef))
	{
		size = op_raw_total(oggRef, -1); 
		duration = op_pcm_total(oggRef, -1);
	}
	
	{
		// Stereo max sample size
		audio->bufferSize = Megabyte(50);
		audio->pcmBuffer = arena_PushArray(&memory->permanentStorage, audio->bufferSize, f32);
	}
	
	WebAudioWorkletProcessorCreateOptions opts = {
		.name = "noise-generator",
	};
	
	emscripten_create_wasm_audio_worklet_processor_async(audioContext, &opts, &AudioWorkletProcessorCreated, userData);
}

int main() {
	EmscriptenWebGLContextAttributes attrs;
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
    
    platform_engine engine = {};
    platform_render render = {};
    platform_user_data userData = { &engine, &render };
	wasm_audio* audio = &userData.audio;
    
    int ctx = emscripten_webgl_create_context("canvas", &attrs);
    if(!ctx)
    {
        printf("Webgl ctx could not be created!\n");
        return -1;
    }
    emscripten_webgl_make_context_current(ctx);
    
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
        
        // TEMP:
        engine.gameInit = game_Init;
        engine.gameUpdate = game_Update;
        if(engine.gameInit)
        {
            engine.gameInit(&engine);
        }
		
		engine.log = WASM_Log;
		engine.vsSource = _shader_resource_vs;
		engine.psSource = _shader_resource_ps;
		
		// TODO: Make it dynamic for resizing
		engine.windowHeight = 800;
		engine.windowWidth = 600;
    }

    u8* audioThreadStack[4096];
    // AUDIO
    {
        EMSCRIPTEN_WEBAUDIO_T context = emscripten_create_audio_context(0);
		audio->context = context;

        emscripten_start_wasm_audio_worklet_thread_async(context, audioThreadStack, sizeof(audioThreadStack), 
														 &AudioThreadInitialized, &userData);
    }

#if 1    
	char* version = (char*)glGetString(GL_VERSION);
    printf("WebGL verison: %s\n", version);
#endif
	
    render_Init(&engine, &render);
    
    emscripten_set_main_loop_arg(WASM_Loop, &userData, 0, true);
    
    render_Cleanup(&engine, &render);
    return 1;
}
