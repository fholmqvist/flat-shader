#version 330 core

in vec3 _frag_pos;
in vec3 _normal;
in vec2 _uv;
in vec3 _sector_color;

in vec4 _light_space;

layout (location = 0) out vec3 sector_out;
layout (location = 1) out vec3 texture_out;

uniform vec3 light_dir;

uniform sampler2D shadow_t;

float shadow_calculation(vec4 light_space, vec3 dir_light) {
    vec3 coords = light_space.xyz / light_space.w;
    coords = coords * 0.5 + 0.5;

    float closest = texture(shadow_t, coords.xy).r;
    float curr = coords.z;
    float bias = max(0.005 * (1 - dot(_normal, dir_light)), 0.0005);

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

    directional_shade = directional_shade.r > 0.5 ? vec3(1) : vec3(0);

    return tex * directional_shade;
}

void main() {
    // FragColor = with_shadows(texture(sector_t, _uv));
    texture_out = with_shadows(_sector_color);
    // FragColor = texture(shadow_t, _uv);
}
