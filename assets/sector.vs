#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 sector_color;

#include "model.glsl"

uniform vec3  world_pos;
uniform vec3  rotation;
uniform float scale = 1;

uniform mat4 view_projection;

out vec3 _normal;
out vec2 _uv;
out vec3 _frag_pos;
out vec3 _sector_color;

void main() {
    mat4 model = model_matrix(world_pos, rotation, scale);
    gl_Position = view_projection * model * vec4(pos, 1);

    _normal = mat3(transpose(inverse(model))) * normal;
    _frag_pos = vec3(model * vec4(pos, 1));
    _uv = uv;
    _sector_color = sector_color;
}
