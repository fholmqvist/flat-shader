#include "light.hpp"

LightPoint::LightPoint(int _distance) {
    distance = _distance;

    float d = std::max(1.0f, static_cast<float>(distance));

    constant = 1.0f;
    linear = 1.0f / std::pow(d, 0.75f);
    quadratic = 12.0f / std::pow(d, 2.2f);
}
