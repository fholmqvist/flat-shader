#version 330 core

in vec2 _uv;

uniform sampler2D sector_t;
uniform sampler2D texture_t;
uniform sampler2D depth_t;

uniform vec2 texel_size;

out vec4 FragColor;

float threshold = 0.05;
const int base_radius = 1;

const vec3 BLACK = vec3(0);

const float near = 0.2;
const float far  = 1;

float linearize_depth(float depth) {
    float z = depth * 2 - 1;
    return (2 * near * far) / (far + near - z * (far - near));
}

void main() {
    float depth = texture(depth_t, _uv).r;
    float linear_depth = linearize_depth(depth);

    float scale = (1 - linear_depth) * 8;

    int radius = int(float(base_radius) * scale);
    radius = clamp(radius, 1, 10);

    vec3 center = texture(sector_t, _uv).rgb;
    float edge = 0.0;
    int samples = 0;

    for (int y = -radius; y <= radius; ++y)
    for (int x = -radius; x <= radius; ++x) {
        if (x == 0 && y == 0) continue;
        vec3 n = texture(sector_t, _uv + vec2(x, y) * texel_size).rgb;
        edge += step(threshold, length(center - n));
        samples++;
    }

    edge = clamp(edge / float(samples), 0, 1);

    edge = smoothstep(0.1, 0.2, edge);

    vec3 final = texture(texture_t, _uv).rgb;

    float avg = (final.r + final.g + final.b) / 3.0;

    if (avg < 0.1) {
        FragColor = vec4(mix(BLACK, final * 4, edge), 1);
    } else {
        FragColor = vec4(mix(final, BLACK, edge), 1);
    }
}
