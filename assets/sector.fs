#version 330 core

in vec2 _uv;
in vec3 _normal;
in vec3 _frag_pos;

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

    vec3 sector_rgb = texture(sector_t, _uv).rgb + vec3(rand, rand / 2, rand / 3);
    sector_out = sector_rgb;

    vec3 view_dir = normalize(view_pos - _frag_pos);
    vec3 result = dir_light_calc(dir_light, N, view_dir, color);

    texture_out = result.r > 0.25 ? color : mix(color, BLACK, 0.95);
}
