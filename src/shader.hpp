#pragma once

#include "pch.hpp" // IWYU pragma: keep

struct Session;

struct Shader {
    u32 ID = 0;
    u32 VAO = 0;
    u32 VBO = 0;
    u32 EBO = 0;

    std::string vertex_source;
    std::string fragment_source;

    std::function<void(Shader &)> init_func;
    std::function<void(Shader &, Session &)> draw_func;

    Shader(){}

    Shader(std::string vertex_source, std::string fragment_source,
           std::function<void(Shader &)> init_func,
           std::function<void(Shader &, Session &)> draw_func)
        : vertex_source(vertex_source), fragment_source(fragment_source), init_func(init_func),
          draw_func(draw_func) {
    }

    void init();

    void render(Session &l);

    void bind();
    void unbind();
};

void store_glsl_helper(std::string source);

void compile_shaders(u32 shader_id, std::string vertex_source, std::string fragment_source);
