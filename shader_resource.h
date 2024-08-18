/* date = August 18th 2024 8:23 pm */

#ifndef SHADER_RESOURCE_H
#define SHADER_RESOURCE_H

/*
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
*/
global_variable char* _shader_resource_vs =
"#version 300 es\n"
"precision mediump float;\n"
"\n"
"in vec2 p0;\n"
"in vec2 p1;\n"
"in vec2 t0;\n"
"in vec2 t1;\n"
"\n"
"uniform mat4 viewProjection;\n"
"uniform sampler2D tex;\n"
"\n"
"const vec2 pos[] = vec2[4](\n"
"\tvec2(-1.0,  1.0),\n"
"\tvec2(-1.0, -1.0),\n"
"\tvec2( 1.0,  1.0),\n"
"\tvec2( 1.0, -1.0)\n"
");\n"
"\n"
"out vec2 texCoords;\n"
"\n"
"void main()\n"
"{\n"
"\tivec2 texSize      = textureSize(tex, 0);\n"
"\t\n"
"\tvec2 dst_half_size = (p1 - p0) / 2.0;\n"
"\tvec2 dst_center    = (p1 + p0) / 2.0;\n"
"\tvec2 dst_pos       = (pos[gl_VertexID] * dst_half_size + dst_center);\n"
"\n"
"\tvec2 src_half_size = (t1 - t0) / 2.0;\n"
"\tvec2 src_center    = (t1 + t0) / 2.0;\n"
"\tvec2 src_pos       = (pos[gl_VertexID] * src_half_size + src_center);\n"
"\n"
"\tgl_Position = viewProjection * vec4(dst_pos, 0.0, 1.0);\n"
"\ttexCoords   = vec2(src_pos.x / float(texSize.x), src_pos.y / float(texSize.y));\n"
"}\n"
"";

/*
#version 300 es
precision mediump float;

uniform sampler2D tex;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
	FragColor = texture(tex, texCoords);
}
*/
global_variable char* _shader_resource_ps =
"#version 300 es\n"
"precision mediump float;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"in vec2 texCoords;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"\tFragColor = texture(tex, texCoords);\n"
"}\n"
"";

global_variable char* _shader_resource_vs46 =
"#version 460 core\n"
"\n"
"in vec2 p0;\n"
"in vec2 p1;\n"
"in vec2 t0;\n"
"in vec2 t1;\n"
"\n"
"uniform mat4 viewProjection;\n"
"uniform sampler2D tex;\n"
"\n"
"const vec2 pos[] = vec2[4](\n"
"\tvec2(-1.0,  1.0),\n"
"\tvec2(-1.0, -1.0),\n"
"\tvec2( 1.0,  1.0),\n"
"\tvec2( 1.0, -1.0)\n"
");\n"
"\n"
"out vec2 texCoords;\n"
"\n"
"void main()\n"
"{\n"
"\tivec2 texSize      = textureSize(tex, 0);\n"
"\t\n"
"\tvec2 dst_half_size = (p1 - p0) / 2;\n"
"\tvec2 dst_center    = (p1 + p0) / 2;\n"
"\tvec2 dst_pos       = (pos[gl_VertexID] * dst_half_size + dst_center);\n"
"\n"
"\tvec2 src_half_size = (t1 - t0) / 2;\n"
"\tvec2 src_center    = (t1 + t0) / 2;\n"
"\tvec2 src_pos       = (pos[gl_VertexID] * src_half_size + src_center);\n"
"\n"
"\tgl_Position = viewProjection * vec4(dst_pos, 0.0, 1.0);\n"
"\ttexCoords   = vec2(src_pos.x / float(texSize.x), src_pos.y / float(texSize.y));\n"
"}\n"
"";

global_variable char* _shader_resource_ps46 = 
"#version 460 core\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"in vec2 texCoords;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"\tFragColor = texture(tex, texCoords);\n"
"}\n"
"";

#endif //SHADER_RESOURCE_H
