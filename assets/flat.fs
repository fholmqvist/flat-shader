#version 330 core

in vec2 _uv;

uniform sampler2D depth_t;
uniform sampler2D normal_t;
uniform vec2 texel_size;

float depth_sens  = 1;
float normal_sens = 1;

vec2 step = texel_size * 2;

out vec4 FragColor;

float depth_edge() {
    float center = texture(depth_t, _uv).r;
    float dx = abs(texture(depth_t, _uv + vec2(step.x, 0)).r - center);
    float dy = abs(texture(depth_t, _uv + vec2(0, step.y)).r - center);
    return smoothstep(0, depth_sens * 0.002, dx + dy);
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
const float threshold = 0.99999;

void main() {
    float dEdge = depth_edge(); // > threshold ? 1 : 0;
    float nEdge = normal_edge();
    float edge = clamp(dEdge + nEdge, 0, 1);

    FragColor = vec4(mix(YELLOW, BLACK, edge), 1);
}
