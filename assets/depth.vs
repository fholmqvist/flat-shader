#version 330 core

#include "model.glsl"

layout (location = 0) in vec3 pos;

uniform vec3  world_pos;
uniform vec3  rotation = vec3(0.0);
uniform float scale = 1.0;
uniform mat4  light_space;

void main() {
    mat4 model = model_matrix(world_pos, rotation, scale);
    gl_Position = light_space * model * vec4(pos, 1.0);
}
