#version 330 core

in vec2 _uv;

uniform sampler2D sector_t;

out vec4 FragColor;

void main() {
    FragColor = texture(sector_t, _uv);
}
