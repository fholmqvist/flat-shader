#version 330 core

in vec3 _frag_pos;
in vec3 _normal;
in vec4 _light_space;

uniform vec3 light_pos;
uniform vec3 view_pos;

uniform sampler2D shadow_t;

out vec4 FragColor;

float calculate_shadow(vec4 light_space) {
    vec3 coords = light_space.xyz / light_space.w;
    coords = coords * 0.5 + 0.5;
    float closest_depth = texture(shadow_t, coords.xy).r;
    float curr_depth = coords.z;
    float shadow = curr_depth > closest_depth  ? 1 : 0;
    return shadow;
}

void main() {
    vec3 light_dir = normalize(light_pos - _frag_pos);
    float diff = max(dot(light_dir, _normal), 0);
    vec3 diffuse = diff * vec3(1);

    float shadow = calculate_shadow(_light_space);

    vec3 final = (1 - shadow) * diffuse;

    FragColor = vec4(final, 1);
}
