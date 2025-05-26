#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord0;

layout (location = 2) in vec2 a_offset;
layout (location = 3) in vec2 a_scale;
layout (location = 4) in vec2 a_uvmin;
layout (location = 5) in vec2 a_uvmax;
layout (location = 6) in vec4 a_color;
layout (location = 7) in float a_z;
layout (location = 8) in float a_flags;

uniform vs_params {
	mat4 model;
    mat4 view;
    mat4 proj;
};

out vec2 uv;
out vec4 color;

void main() {
	uv = a_uvmin + (a_texcoord0 * (a_uvmax - a_uvmin));
    color = a_color;

    vec2 ipos = a_offset + (a_scale * a_position);
    gl_Position = proj * view * model * vec4(ipos, a_z, 1.0);
}
