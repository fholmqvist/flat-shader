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
uniform mat4 light_space;

out vec3 _normal;
out vec2 _uv;
out vec3 _sector_color;

out vec3 _frag_pos;
out vec4 _light_space;

void main() {
    mat4 model     = model_matrix(world_pos, rotation, scale);
    vec4 final_pos = model * vec4(pos, 1);

    _frag_pos     = vec3(final_pos);
    _normal       = normalize(mat3(model) * normal);
    _light_space  = light_space * final_pos;
    _uv           = uv;
    _sector_color = sector_color;

    gl_Position = view_projection * final_pos;
}
