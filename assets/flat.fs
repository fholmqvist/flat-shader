#version 330 core

in vec2 _uv;

uniform sampler2D depth_t;
uniform sampler2D normal_t;
uniform vec2 texel_size;

uniform mat4 inv_proj;

float depth_sens  = 1.5;
float normal_sens = 1.0;

float d_n_threshold = 0.3;
float d_n_threshold_scale = 1.0;

vec2 step = texel_size * 2;

out vec4 FragColor;

vec3 get_view_dir(float depth, vec2 uv) {
    vec2 ndc = uv * 2 - 1;
    vec4 clip = vec4(ndc, depth * 2 - 1, 1);
    vec4 view = inv_proj * clip;
    view /= view.w;
    return normalize(view.xyz);
}

// float depth_edge() {
//     float center = texture(depth_t, _uv).r;
//     float dx = abs(texture(depth_t, _uv + vec2(step.x, 0)).r - center);
//     float dy = abs(texture(depth_t, _uv + vec2(0, step.y)).r - center);
//     return smoothstep(0, depth_sens * 0.002, dx + dy);
// }

float depth_edge(vec3 view_dir, vec3 normal_center) {
    float center = texture(depth_t, _uv).r;
    float dx = abs(texture(depth_t, _uv + vec2(step.x, 0)).r - center);
    float dy = abs(texture(depth_t, _uv + vec2(0, step.y)).r - center);
    float diff = dx + dy;

    vec3 n = normalize(normal_center * 2 - 1);

    float ndotv = 1 - dot(n, -view_dir);
    ndotv = clamp(ndotv, 0, 1);

    float nv_mod = smoothstep(d_n_threshold, 1, ndotv);

    float mod_scale = 1 + nv_mod * d_n_threshold_scale;
    mod_scale = clamp(mod_scale, 1, 2);

    float effective_threshold = depth_sens * 0.002 * mod_scale;

    return smoothstep(0, effective_threshold, diff);
}

float normal_edge() {
    vec3 center = texture(normal_t, _uv).xyz;
    vec3 nx = texture(normal_t, _uv + vec2(step.x, 0)).xyz;
    vec3 ny = texture(normal_t, _uv + vec2(0, step.y)).xyz;
    float diff = (1 - dot(center, nx)) + (1 - dot(center, ny));
    return smoothstep(0, normal_sens * 0.5, diff);
}

const vec3  YELLOW    = vec3(0.99, 0.67, 0.12);
const vec3  BLACK     = vec3(0);
const float threshold = 0.5;

void main() {
    // float d_edge = depth_edge() > threshold ? 1 : 0;
    // float n_edge = normal_edge();
    // float edge = d_edge + n_edge;
    // FragColor = vec4(vec3(edge), 1);

    float depth = texture(depth_t, _uv).r;
    vec3 view_dir = get_view_dir(depth, _uv);
    vec3 normal_center = texture(normal_t, _uv).xyz;

    float d_edge = depth_edge(view_dir, normal_center);
    float n_edge = normal_edge();

    float edge = d_edge + n_edge;

    FragColor = vec4(mix(YELLOW, BLACK, edge), 1);
}
