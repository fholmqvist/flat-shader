#pragma once

struct Camera;

struct Input {
    SDL_Event e;

    bool update(Camera&);
};
