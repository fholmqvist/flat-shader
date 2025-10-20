#version 330 core

in vec2 _uv;

uniform sampler2D input_t;
uniform vec2 texel_size;

out vec4 FragColor;

void main() {
    // FragColor = texture(input_t, _uv);
    // return;

    vec3 rgb_nw = texture(input_t, _uv + vec2(-1.0, -1.0) * texel_size).rgb;
    vec3 rgb_ne = texture(input_t, _uv + vec2( 1.0, -1.0) * texel_size).rgb;
    vec3 rgb_sw = texture(input_t, _uv + vec2(-1.0,  1.0) * texel_size).rgb;
    vec3 rgb_se = texture(input_t, _uv + vec2( 1.0,  1.0) * texel_size).rgb;
    vec3 rgb_m  = texture(input_t, _uv).rgb;

    const vec3 luma = vec3(0.299, 0.587, 0.114);
    float luma_nw = dot(rgb_nw, luma);
    float luma_ne = dot(rgb_ne, luma);
    float luma_sw = dot(rgb_sw, luma);
    float luma_se = dot(rgb_se, luma);
    float lumaM  = dot(rgb_m,  luma);

    float luma_min = min(lumaM, min(min(luma_nw, luma_ne), min(luma_sw, luma_se)));
    float luma_max = max(lumaM, max(max(luma_nw, luma_ne), max(luma_sw, luma_se)));

    float contrast = luma_max - luma_min;

    if (contrast < 0.031) {
        FragColor = vec4(rgb_m, 1.0);
        return;
    }

    vec2 dir;
    dir.x = -((luma_nw + luma_ne) - (luma_sw + luma_se));
    dir.y =  ((luma_nw + luma_sw) - (luma_ne + luma_se));

    float dir_reduce = max((luma_nw + luma_ne + luma_sw + luma_se) * 0.25 * 0.5, 1e-4);
    float rcp_dir_min = 1.0 / (min(abs(dir.x), abs(dir.y)) + dir_reduce);
    dir = clamp(dir * rcp_dir_min, -8.0, 8.0) * texel_size;

    vec3 rgb_a = 0.5 * (
        texture(input_t, _uv + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture(input_t, _uv + dir * (2.0 / 3.0 - 0.5)).rgb
    );

    vec3 rgb_b = rgb_a * 0.5 + 0.25 * (
        texture(input_t, _uv + dir * -0.5).rgb +
        texture(input_t, _uv + dir * 0.5).rgb
    );

    float luma_b = dot(rgb_b, luma);
    if (luma_b < luma_min || luma_b > luma_max) {
        FragColor = vec4(rgb_a, 1.0);
    } else {
        FragColor = vec4(rgb_b, 1.0);
    }
}
