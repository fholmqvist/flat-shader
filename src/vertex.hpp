#pragma once

struct Vertex {
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec2 uv;
    float face_offset;

    void print(int index);
};
