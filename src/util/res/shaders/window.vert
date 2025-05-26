#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord0;

uniform vs_params {
	mat4 model;
	mat4 view;
	mat4 proj;
};

out	vec2 uv;

void main() {
	gl_Position = proj * view * model * vec4(a_position, 0, 1);
	uv = a_texcoord0;
}
