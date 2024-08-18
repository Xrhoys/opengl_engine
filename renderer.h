/* date = August 15th 2024 0:56 pm */

#ifndef RENDERER_H
#define RENDERER_H

// Forward declaration
struct render_state;
struct render_group;
struct render_command;

struct render_command
{
    u32 bufferId;
    u32 textureId;
    u32 vertexOffset;
    u32 quadCount;
};

struct render_group
{
    render_command* commands;
    u32 count;
    u32 size;
};

struct platform_render
{
    u32 program;
    render_group testGroup;
    
    u32 buffer;
    u32 vao;
	u32 texture;
};

struct vertex
{
    f32 p0[2];
	f32 p1[2];
	f32 t0[2];
	f32 t1[2];
};

#endif //RENDERER_H
