#include "input.hpp"
#include "camera.hpp"

bool Input::update(Camera &c) {
    bool running = true;

    const bool* keystate = SDL_GetKeyboardState(NULL);
    const auto mousestate = SDL_GetMouseState(NULL, NULL);

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (e.key.key) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_SPACE:
                        c.print_debug_information();
                        break;
                    case SDLK_M:
                        c.reset_view();
                        break;
                }
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                if (abs(e.wheel.y) > 0) {
                    c.mouse_scroll(e.wheel.y);
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                c.mpos.x = e.motion.x;
                c.mpos.y = e.motion.y;

                if (mousestate & SDL_BUTTON_MMASK) {
                    int dx = e.motion.xrel;
                    int dy = e.motion.yrel;
                    c.mouse_movement((float)dx, (float)-dy);
                }
                break;
        }
    }

    const bool UP = keystate[SDL_SCANCODE_I];
    const bool DOWN = keystate[SDL_SCANCODE_K];
    const bool LEFT = keystate[SDL_SCANCODE_J];
    const bool RIGHT = keystate[SDL_SCANCODE_L];

    if (UP) {
        c.movement(vec3(0, 0, -1));
    } else if (DOWN) {
        c.movement(vec3(0, 0, 1));
    }
    if (LEFT) {
        c.movement(vec3(-1, 0, 0));
    } else if (RIGHT) {
        c.movement(vec3(1, 0, 0));
    }

    return running;
}
