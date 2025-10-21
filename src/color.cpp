#include "color.hpp"

std::vector<vec3> palette = {
    hex_to_rgb("#5d7557"), //
    hex_to_rgb("#8e9257"), //
    hex_to_rgb("#e55d4d"), //
    hex_to_rgb("#f1866c"), //
    hex_to_rgb("#d26730"), //
    hex_to_rgb("#de9a28"), //
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
