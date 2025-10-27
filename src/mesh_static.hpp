#pragma once

#include "vertex.hpp"

struct MeshStatic {
    std::vector<Vertex> verts;
    std::vector<u16> indices;

    // TODO: These should be in model.
    vec3 position = vec3(0);
    vec3 rotation = vec3(0);
    float scale = 1;

    static MeshStatic from_scene(std::string file);

    void draw(u32 shader_id);

    void apply_perlin(vec3 wpos, vec3 strength = vec3(1));

    void gl_buffer_data();
    void gl_uniforms(u32 shader_id);
};
