#ifndef WASM_MAIN_H
#define WASM_MAIN_H

// TODO(Ecy): rename struct with WASM prefix
struct wasm_audio
{
	EMSCRIPTEN_WEBAUDIO_T context;
	
	// Sample
	OggOpusFile* sample;
	
	// buffer
	f32* pcmBuffer;
	u32 bufferSize;
	u32 sampleSize;
	u32 readIndex;
	u32 writeIndex;
};

struct platform_user_data
{
    platform_engine* engine;
    platform_render* render;
	wasm_audio   audio;
};

#endif //WASM_MAIN_H
