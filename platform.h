#include <stdint.h>

#ifndef PLATFORM_H
#define PLATFORM_H

#define Kilobytes(n) 1024LL * n
#define Megabyte(n)  1024LL * 1024LL * n 
#define Gigabytes(n) 1024LL * 1024LL * 1024LL * n

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))
#define Clamp(value, low, high) ((value) < (high)) ? (((value) > (low)) ? (value) : (low)) : (high) 
#define Abs(x) (x) > 0 ? (x) : -(x)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define CheckFlag(s, f) (((s) & (f)) == (f))
#define AddFlag(s, f) (s) |= (f)
#define RemoveFlag(s, f) (s) &= ~(f)

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float f32;
typedef float f64;

typedef uint32_t b32;
typedef uint8_t  b8;

#define global_variable static
#define internal        static
#define local_persist   static 

struct platform_engine;
struct arena;
struct thread_context;
struct read_file_result;

struct read_file_res
{
	u64 size;
	u8* data;
};

struct thread_context
{
    u64 threadId;
	u32 threadIndex;
};

#define READ_ENTIRE_FILE(name) \
read_file_res name(thread_context* thread, const char* filename, arena* memory)
typedef READ_ENTIRE_FILE(read_entire_file);
#define WRITE_ENTIRE_FILE(name) \
u64 name(thread_context* thread, arena* memory)
typedef WRITE_ENTIRE_FILE(write_entire_file);

#define GAME_INIT(name) \
void name(platform_engine* engine)
typedef GAME_INIT(game_init);
#define GAME_UPDATE(name) \
void name(platform_engine* engine, f32 dt)
typedef GAME_UPDATE(game_update);

#define PLATFORM_LOG(name) \
void name(const char* fmt, ...)
typedef PLATFORM_LOG(platform_log);
struct arena
{
    u8* base;
    u64 size;
    u64 used;
};

#define arena_PushStruct(arena, type) \
(type*)arena_PushSize_(arena, sizeof(type))
#define arena_PushArray(arena, size, type) \
(type*)arena_PushSize_(arena, sizeof(type) * size)

inline void*
arena_PushSize_(arena* a, u64 size)
{
    Assert(a->used + size < a->size);
    void* ptr = a->base + a->used;
    a->used += size;
    
    return ptr;
}

inline void
arena_Reset(arena* a, b32 zero = false)
{
    if(zero)
    {
        // ZeroMemory()
    }
    a->used = 0;
}

inline void
arena_Pop(arena* a, u64 size)
{
	Assert(a->used - size >= 0);
	a->used -= size;
}

////////////////////////////////
// RING BUFFER API
////////////////////////////////
struct ring_buf
{
	u8* data;
	u32 size;
	u32 read;
	u32 write;
};

// Read
#define ring_ReadStruct(buffer, type) *(type*)ring_Read_(buffer, sizeof(type))
#define ring_ReadArray(buffer, size, type) (type*)ring_Read_(buffer, size * sizeof(type), type)
inline u8*
ring_Read_(ring_buf* buffer, u32 size)
{
	Assert(size < buffer->size);
	u8* cursor = buffer->data + buffer->read;
	if(buffer->read + size > buffer->size)
	{
		buffer->read += size;
	}
	else
	{
		buffer->read = buffer->read + size - buffer->size;
	}
	
	return cursor;
}

// Write
#define ring_WriteStruct(buffer, type) (type*)ring_Write_(buffer, sizeof(type))
#define ring_WriteArray(buffer, size, type) (type*)ring_Write_(buffer, size * sizeof(type));
inline u8*
ring_Write_(ring_buf* buffer, u32 size)
{
	Assert(size < buffer->size);
	u8* cursor = buffer->data + buffer->write;
	if(buffer->write + size > buffer->size)
	{
		buffer->write += size;
	}
	else
	{
		buffer->write = buffer->write + size - buffer->size;
	}
	
	return cursor;
}

// Readable size
inline u32
ring_ReadableLeft(ring_buf* buffer)
{
	if(buffer->read < buffer->write)
	{
		return buffer->write - buffer->read;
	}
	else
	{
		return buffer->size - buffer->read + buffer->write;
	}
}

// Writable space
inline u32
ring_WritableLeft(ring_buf* buffer)
{
	if(buffer->read < buffer->write)
	{
		return buffer->size - buffer->write + buffer->read;
	}
	else
	{
		return buffer->read - buffer->write;
	}
}

struct platform_memory
{
    arena transientStorage;
    arena permanentStorage;
    
    // DISK read/write layer
    read_entire_file*  readFile;
    write_entire_file* writeFile;
};

////////////////////////////////
// AUDIO LAYER
////////////////////////////////
struct platform_audio;

#define AUDIO_INIT(name) void name(thread_context* thread, platform_audio* context)
typedef AUDIO_INIT(audio_init);
#define AUDIO_TOOGLE(name) void name(platform_audio* context, b32 state)
typedef AUDIO_TOOGLE(audio_toogle);
struct platform_audio
{
	// Output buffer
	audio_init*   platformInit;
	audio_toogle* playToogle;
	
	ring_buf buffer;
	b32      generalPlaystate;
	i32      flags;
	f64      time;
};

#endif //PLATFORM_H
