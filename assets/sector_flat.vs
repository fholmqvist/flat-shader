#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out vec2 _uv;

void main() {
    gl_Position = vec4(pos.xy, 0, 1);
    _uv = uv;
}
