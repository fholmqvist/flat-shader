#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 3) in vec2 uv;

#include "model.glsl"

uniform vec3  world_pos;
uniform vec3  rotation;
uniform float scale = 1;

uniform mat4 view_projection;

out vec2 _uv;

void main() {
    mat4 model = model_matrix(world_pos, rotation, scale);
    gl_Position = view_projection * model * vec4(pos, 1);

    _uv = uv;
}
