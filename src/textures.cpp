#include "textures.hpp"
#include "base.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

void Textures::add(std::string name, std::string path, u32 color_format, u32 wrap) {
    int width, height, nr_channels;
    auto texture_data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
    if (!texture_data) {
        throw std::runtime_error("Failed to load texture " + path);
        return;
    }

    Texture t;

    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE,
                 texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    textures[name] = t;
}

Texture &Textures::get(std::string name) {
    if (!textures.contains(name)) {
        log_dang("Missing texture \"%s\"", name.c_str());
    }
    return textures[name];
}
