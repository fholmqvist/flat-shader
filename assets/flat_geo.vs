#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

#include "model.glsl"

uniform vec3  world_pos;
uniform vec3  rotation;
uniform float scale = 1;

uniform mat4 view;
uniform mat4 projection;

out vec3 normal_;
out vec3 frag_pos_;

void main() {
    mat4 model = model_matrix(world_pos, rotation, scale);
    mat3 normal_matrix = transpose(inverse(mat3(model)));

    normal_ = normalize(normal_matrix * normal);
    frag_pos_ = vec3(model * vec4(pos, 1));

    gl_Position = projection * view * vec4(frag_pos_, 1);
}
