/* date = November 18th 2024 2:48 am */

#ifndef AUDIO_H
#define AUDIO_H

struct audio_sequence;
struct audio_mix;

struct audio_track
{
	
};

enum audio_decoder_state
{
	AUDIO_DECODER_START,
	AUDIO_DECODER_IDLE,
	AUDIO_DECODER_RUNNING,
	AUDIO_DECODER_PAUSE,
	AUDIO_DECODER_STOP,
	
	AUDIO_DECODER_COUNT,
};

enum audio_decoder_command
{
	
};

enum audio_encoder_type
{
	AUDIO_ENCODER_OPUS,
};

union audio_encoding
{
	OggOpusFile* ogg;
};

// NOTE: used per file
struct audio_decoder_context
{
	ring_buf buffer;
	
	audio_decoder_state state;
	audio_encoder_type  encoderType;
	audio_encoding      encoder;
	
	// + resource reference ptr
	
	f64 readTime;
};

struct audio_sequence
{
	
};

// DECODE AND UPDATE INTERNAL RING BUFFER

#endif //AUDIO_H
