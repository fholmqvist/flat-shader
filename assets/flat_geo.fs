#version 330 core

layout (location = 0) out vec3 out_color;
layout (location = 1) out vec3 out_normal;

in vec3 normal_;
in vec3 frag_pos_;

uniform vec3 base_color;

void main() {
    out_color = base_color;
    out_normal = normalize(normal_);
}
