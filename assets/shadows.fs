#version 330 core

in vec3 _frag_pos;
in vec3 _normal;
in vec2 _uv;
in vec4 _light_space;

uniform vec3 light_dir;

uniform sampler2D sector_t;
uniform sampler2D shadow_t;

out vec4 FragColor;

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

vec4 with_shadows(vec4 tex) {
    vec3 norm = normalize(_normal);
    vec3 dir_light = normalize(-light_dir);

    float diff = max(dot(norm, dir_light), 0);
    float shadow = shadow_calculation(_light_space, dir_light);
    vec3 directional_shade = vec3((1 - shadow) * diff);

    directional_shade = directional_shade.r > 0.5 ? vec3(1) : vec3(0);

    vec4 lighting = vec4(directional_shade, 1);

    return tex * lighting;
}

void main() {
    // FragColor = with_shadows(texture(sector_t, _uv));
    FragColor = with_shadows(vec4(1, 0, 0, 1));
    // FragColor = texture(shadow_t, _uv);
}
