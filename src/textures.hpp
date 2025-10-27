#pragma once

#include "pch.hpp" // IWYU pragma: keep

struct Texture {
    u32 id;
};

struct Textures {
    std::unordered_map<std::string, Texture> textures;

    void add(std::string name, std::string path, u32 color_format = GL_RGBA, u32 wrap = GL_REPEAT);
    Texture &get(std::string name);
};
