#version 330 core

in vec2 _uv;
in vec3 _normal;
in vec3 _frag_pos;
in vec3 _sector_color;

layout (location = 0) out vec3 sector_out;
layout (location = 1) out vec3 texture_out;

uniform sampler2D sector_t;
uniform vec3 color;
uniform vec3 view_pos;
uniform float rand;

const vec3 BLACK = vec3(0);

#include "lights.glsl"
#include "lights_calc.glsl"

void main() {
    vec3 N = normalize(_normal);

    sector_out = _sector_color;
    texture_out = _sector_color;
    return;

    vec3 view_dir = normalize(view_pos - _frag_pos);
    vec3 result = dir_light_calc(dir_light, N, view_dir, color);

    float avg = (result.r + result.g + result.b) / 3;
    texture_out = avg > 0.25 ? color : mix(color, BLACK, 0.9);
}
