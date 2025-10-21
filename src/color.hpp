#pragma once

// https://lospec.com/palette-list/japanese-woodblock
extern std::vector<vec3> palette;

vec3 random_color();

vec3 hex_to_rgb(std::string);
