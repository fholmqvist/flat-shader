#pragma once

#include "camera.hpp"
#include "input.hpp"
#include "mesh_static.hpp"
#include "shader.hpp"
#include "timer.hpp"
#include "window.hpp"

struct Session {
    bool running = true;

    Timer timer;
    Window window;
    Input input;
    Camera camera;

    MeshStatic mesh;

    Shader geo;
    Shader result;

    void init();
    void update();
    void render();
};
