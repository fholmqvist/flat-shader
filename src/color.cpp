#include "color.hpp"

std::vector<vec3> palette = {
    hex_to_rgb("#2b2821"), hex_to_rgb("#624c3c"), hex_to_rgb("#d9ac8b"), hex_to_rgb("#e3cfb4"),
    hex_to_rgb("#243d5c"), hex_to_rgb("#5d7275"), hex_to_rgb("#5c8b93"), hex_to_rgb("#b1a58d"),
    hex_to_rgb("#b03a48"), hex_to_rgb("#d4804d"), hex_to_rgb("#e0c872"), hex_to_rgb("#3e6958"),
};

static std::random_device rd;
static std::mt19937 gen(rd());
std::uniform_int_distribution<size_t> dist(0, palette.size() - 1);

vec3 random_color() {
    return palette[dist(gen)];
}

vec3 hex_to_rgb(std::string hex) {
    std::string h = hex;
    if (h[0] == '#') {
        h = h.substr(1);
    }

    if (h.size() != 6) {
        throw std::invalid_argument("Invalid hex color");
    }

    u32 r, g, b;
    std::stringstream ss;
    ss << std::hex << h;
    u32 hexColor;
    ss >> hexColor;

    r = (hexColor >> 16) & 0xFF;
    g = (hexColor >> 8) & 0xFF;
    b = hexColor & 0xFF;

    return vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}
