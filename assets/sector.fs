#version 330 core

in vec2 _uv;

layout (location = 0) out vec3 sector_out;
layout (location = 1) out vec3 texture_out;

uniform sampler2D sector_t;
uniform vec3 color;

void main() {
    sector_out = texture(sector_t, _uv).rgb;
    texture_out = vec3(color);
}
