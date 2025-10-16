#include "random.hpp"

#include "external/PerlinNoise.hpp"

const siv::PerlinNoise SIV_PERLIN{ 123456u };

namespace Random {
double perlin2(double x, double y) {
    return SIV_PERLIN.noise2D(x, y);
}
} // namespace Random
