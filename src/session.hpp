#pragma once

#include "pch.hpp" // IWYU pragma: keep

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

    MeshStatic sofa;
    MeshStatic chair;
    MeshStatic table;
    MeshStatic wall;

    Shader geo;
    Shader result;

    Session();

    void update();
    void render();
};
