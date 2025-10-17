#version 330 core

in vec2 _uv;
in vec3 _normal;

layout (location = 0) out vec3 sector_out;
layout (location = 1) out vec3 texture_out;

uniform sampler2D sector_t;
uniform vec3 color;

uniform vec3  tintDirection = vec3(0, 0, 1);
uniform vec3  tint          = vec3(0.8, 0, 0);
uniform float tintStrength  = 0.5;

void main() {
    sector_out = texture(sector_t, _uv).rgb;

    vec3 N = normalize(_normal);
    vec3 L = normalize(tintDirection);

    float facing = dot(N, L);
    float tintAmount = smoothstep(0, 1, facing) * tintStrength;

    vec3 final = mix(color, tint, tintAmount);

    texture_out = vec3(final);
}
