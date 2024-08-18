#version 300 es
precision mediump float;

in vec2 p0;
in vec2 p1;
in vec2 t0;
in vec2 t1;

uniform mat4 viewProjection;
uniform sampler2D tex;

const vec2 pos[] = vec2[4](
	vec2(-1.0,  1.0),
	vec2(-1.0, -1.0),
	vec2( 1.0,  1.0),
	vec2( 1.0, -1.0)
);

out vec2 texCoords;

void main()
{
	ivec2 texSize      = textureSize(tex, 0);
	
	vec2 dst_half_size = (p1 - p0) / 2;
	vec2 dst_center    = (p1 + p0) / 2;
	vec2 dst_pos       = (pos[gl_VertexID] * dst_half_size + dst_center);

	vec2 src_half_size = (t1 - t0) / 2;
	vec2 src_center    = (t1 + t0) / 2;
	vec2 src_pos       = (pos[gl_VertexID] * src_half_size + src_center);

	gl_Position = viewProjection * vec4(dst_pos, 0.0, 1.0);
	texCoords   = vec2(src_pos.x / float(texSize.x), src_pos.y / float(texSize.y));
}
