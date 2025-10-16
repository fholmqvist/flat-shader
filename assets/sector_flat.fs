#version 330 core

in vec2 _uv;

uniform sampler2D t;
uniform vec2 texel_size;

out vec4 FragColor;

float threshold = 0.05;
int radius = 2;

void main() {
    vec3 center = texture(t, _uv).rgb;
    float edge = 0.0;
    int samples = 0;

    for (int y = -radius; y <= radius; ++y)
    for (int x = -radius; x <= radius; ++x) {
        if (x == 0 && y == 0) continue;
        vec3 n = texture(t, _uv + vec2(x, y) * texel_size).rgb;
        edge += step(threshold, length(center - n));
        samples++;
    }

    edge = clamp(edge / float(samples), 0.0, 1.0);

    edge = edge > 0.2 ? 1 : 0;

    FragColor = vec4(vec3(edge), 1.0);
}
