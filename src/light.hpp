#pragma once

#define LIGHT_COLOR_DEFAULT vec3(1, 0.8, 0.5)

struct LightDirectional {
    float intensity = 1;
    vec3 dir;
    vec3 color = LIGHT_COLOR_DEFAULT;
};

struct LightPoint {
    float intensity = 1;
    vec3 pos;
    vec3 color = LIGHT_COLOR_DEFAULT;

    int distance;
    float constant = 1;
    float linear = 0.09;
    float quadratic = 0.032;

    LightPoint(int _distance);
};
