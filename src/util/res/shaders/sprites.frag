#version 330 core

uniform sampler2D tex;

in vec2 uv;
in vec4 color;

out vec4 frag_color;

void main() {
    frag_color = color * texture(tex, uv);
    if (frag_color.a < 0.0001) {
        discard;
    }
}