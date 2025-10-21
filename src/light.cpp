#include "camera.hpp"
#include "pch.hpp" // IWYU pragma: keep

#include "light.hpp"

int SHADOW_SIZE = 4096;

vec3 LIGHT_DIR = vec3(0.25, -1, 1);
mat4 LIGHT_SPACE;

std::vector<vec3> get_camera_frustum_corners(mat4 proj, mat4 view) {
    mat4 inv = inverse(proj * view);

    std::vector<vec3> corners;
    corners.reserve(8);

    for (int x = 0; x < 2; ++x) {
        for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
                vec4 p = vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                vec4 world = inv * p;
                corners.push_back(vec3(world) / world.w);
            }
        }
    }

    return corners;
}

void update_light_space(Camera &camera) {
    const float FRUSTUM_SIZE = 3;
    const float NEAR = -16;
    const float FAR = 16;

    vec3 center = { camera.pos.x, 0, camera.pos.z };
    mat4 light_view = lookAt(center - normalize(LIGHT_DIR) * 10.0f, center, vec3(0, 1, 0));

    mat4 light_proj = ortho(-FRUSTUM_SIZE, FRUSTUM_SIZE, -FRUSTUM_SIZE, FRUSTUM_SIZE, NEAR, FAR);

    float texel_size = (FRUSTUM_SIZE * 2.0f) / float(SHADOW_SIZE);
    vec4 origin = light_proj * light_view * vec4(0, 0, 0, 1);
    origin *= 0.5f;
    origin += 0.5f;
    origin *= float(SHADOW_SIZE);
    vec2 rounded = floor(vec2(origin.x, origin.y)) - vec2(origin.x, origin.y);
    rounded *= texel_size;
    mat4 texel_align = translate(mat4(1.0), vec3(rounded, 0.0));

    LIGHT_SPACE = texel_align * light_proj * light_view;
}
