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

    MeshStatic monorail;
    MeshStatic rail;
    MeshStatic rail_turn;
    MeshStatic wall;
    MeshStatic floor;
    MeshStatic seagull;

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
    void init_shaders();
    void load_objects();
};
