#version 330 core

in vec3 _frag_pos;
in vec3 _normal;
in vec2 _uv;
in vec3 _sector_color;

in vec4 _light_space;

layout (location = 0) out vec3 sector_out;
layout (location = 1) out vec3 texture_out;

uniform vec3 light_dir;
uniform vec2 texel_size;

uniform sampler2D shadow_t;
uniform sampler2D palette_t;

const vec3 BLACK = vec3(0);

float shadow_calculation(vec4 light_space, vec3 dir_light) {
    vec3 coords = light_space.xyz / light_space.w;
    coords = coords * 0.5 + 0.5;

    float closest = texture(shadow_t, coords.xy).r;
    float curr = coords.z;

    // float bias = max(0.005 * (1 - dot(_normal, dir_light)), 0.0005);

    float cos_t = max(dot(_normal, dir_light), 0);
    float slope_bias = (1 - cos_t) * 0.00005;
    float bias = 0.0005 + slope_bias;

    // float shadow = 0.0;
    // int samples = 0;
    // for (int x = -2; x <= 2; ++x)
    // for (int y = -2; y <= 2; ++y) {
    //     float pcfDepth = texture(shadow_t, coords.xy + vec2(x, y) * texel_size).r;
    //     shadow += (curr - bias > pcfDepth) ? 1.0 : 0.0;
    //     samples++;
    // }
    // shadow /= samples;

    float shadow = curr - bias > closest ? 1 : 0;

    if (coords.x < 0 || coords.x > 1 ||
        coords.y < 0 || coords.y > 1 ||
        coords.z < 0 || coords.z > 1) {
        shadow = 0;
    }

    return shadow;
}

vec3 with_shadows(vec3 tex) {
    vec3 norm = normalize(_normal);
    vec3 dir_light = normalize(-light_dir);

    float diff = max(dot(norm, dir_light), 0);
    float shadow = shadow_calculation(_light_space, dir_light);
    vec3 directional_shade = vec3((1 - shadow) * diff);

    vec3 shade = tex * directional_shade;
    float avg = (shade.r + shade.g + shade.b) / 3;
    return avg > 0.04 ? tex : mix(tex, BLACK, 0.9);
}

void main() {
    sector_out  = _sector_color;
    texture_out = with_shadows(texture(palette_t, _uv).rgb);
}
