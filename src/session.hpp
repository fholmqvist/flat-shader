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
    MeshStatic desk;
    MeshStatic bookshelf;
    MeshStatic room;


    Shader depth;
    Shader shadows;
    Shader lines;
    Shader fxaa;

    Session();

    void update();
    void render();

  private:
    void load_glsl_helpers();
    void generate_buffers();
    void load_objects();
};
