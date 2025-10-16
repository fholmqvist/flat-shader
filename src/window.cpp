#include "window.hpp"

#include "base.hpp"
#include "pch.hpp"
#include "timer.hpp"
#include <SDL3/SDL_video.h>

int SCREEN_W = 1280; // 640;
int SCREEN_H = 960;  // 480;
float DELTA_TIME = 0.0166f;

bool Window::init() {
    GLOBAL_TIMER.reset();

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "1");

    const auto ar = aspect_ratio(SCREEN_W, SCREEN_H);
    if (ar.first != 4 || ar.second != 3) {
        throw std::runtime_error(frmt("Illegal screen resolution %d:%d", ar.first, ar.second));
    }

    window = SDL_CreateWindow("GOBS", SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        SDL_Log("SDL_GL_CreateContext Error: %s\n", SDL_GetError());
        return false;
    }

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW init failed: %s\n", glewGetErrorString(err));
        return false;
    }

    glClearColor(0, 0, 0, 1);

    // VSYNC.
    SDL_GL_SetSwapInterval(0);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    log_info("Initialized window in %s", time_to_string(GLOBAL_TIMER.stop()).c_str());

    return true;
}

void Window::update() {
    last = now;
    now = SDL_GetPerformanceCounter();
    DELTA_TIME = (float)((now - last) / (float)SDL_GetPerformanceFrequency());

    auto fps = 1.0 / DELTA_TIME;
    if (fps < 59 || fps > 61) {
        log_warn("Erratic framerate: %.0f", 1 / DELTA_TIME);
    }
}

void Window::swap() {
    SDL_GL_SwapWindow(window);
}
