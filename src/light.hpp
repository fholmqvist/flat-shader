#pragma once

struct Camera;

extern int SHADOW_SIZE;

extern vec3 LIGHT_DIR;
extern mat4 LIGHT_SPACE;

void update_light_space(Camera &);
