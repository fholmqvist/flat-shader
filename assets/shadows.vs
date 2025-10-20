#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

#include "model.glsl"

uniform vec3  world_pos;
uniform vec3  rotation;
uniform float scale = 1;

uniform mat4 view_projection;
uniform mat4 light_space;

out vec3 _frag_pos;
out vec3 _normal;
out vec2 _uv;
out vec4 _light_space;

void main() {
    mat4 model = model_matrix(world_pos, rotation, scale);

    _frag_pos     = vec3(model * vec4(pos, 1));
    _normal       = mat3(transpose(inverse(model))) * normal;
    _light_space  = light_space * vec4(_frag_pos, 1);
    _uv           = uv;

    gl_Position   = view_projection * vec4(_frag_pos, 1);
}
