#pragma once

#include "vertex.hpp"

struct MeshStatic {
    std::vector<Vertex> verts;
    std::vector<u16> indices;

    // TODO: These should be in model.
    vec3 world_pos = vec3(0);
    vec3 rotation = vec3(0);
    float scale = 1;

    u32 diffuse_id = 0;
    u32 specular_id = 0;
    u32 normal_map_id = 0;

    static MeshStatic from_scene(std::string file, u32 _diffuse_id, u32 _spectral_id,
                                 u32 _normal_map_id);

    void gl_buffer_data();
    void gl_uniforms(u32 &shader_id);
    void print_verts();
};
