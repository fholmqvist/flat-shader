#pragma once

#include "pch.hpp" // IWYU pragma: keep

extern int SCREEN_W;
extern int SCREEN_H;
extern float DELTA_TIME;

struct Window {
  private:
    SDL_Window* window;
    SDL_GLContext gl_context;

    u32 now = SDL_GetPerformanceCounter();
    u32 last = 0;

  public:
    bool init();
    void update();
    void swap();
};
