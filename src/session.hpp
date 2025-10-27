#pragma once

#include "pch.hpp" // IWYU pragma: keep

#include "camera.hpp"
#include "input.hpp"
#include "mesh_static.hpp"
#include "shader.hpp"
#include "textures.hpp"
#include "timer.hpp"
#include "window.hpp"

struct Session {
    bool running = true;

    Timer timer;
    Window window;
    Input input;
    Camera camera;

    Textures textures;

    MeshStatic sofa;
    MeshStatic chair;
    MeshStatic table;
    MeshStatic desk;
    MeshStatic bookshelf;
    MeshStatic room;

    MeshStatic pipe;
    MeshStatic pipe_down;
    MeshStatic pipe_right;

    MeshStatic monorail;

    Shader depth;
    Shader shadows;
    Shader lines;
    Shader fxaa;

    Session();

    void update();
    void render();
    void draw_objects(u32 shader_id);

  private:
    void load_glsl_helpers();
    void generate_buffers();
    void load_objects();
};
