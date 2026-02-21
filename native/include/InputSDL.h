#pragma once
#include "IInput.h"
#include <SDL3/SDL.h>

class InputSDL : public IInput {
public:
    void PollEvents() override {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) quit = true;
            if (e.type == SDL_EVENT_KEY_DOWN) state.keyDown[e.key.scancode] = true;
            if (e.type == SDL_EVENT_KEY_UP) state.keyDown[e.key.scancode] = false;
            if (e.type == SDL_EVENT_MOUSE_MOTION) {
                state.mouseX = e.motion.x;
                state.mouseY = e.motion.y;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) state.mouseButtons[e.button.button] = true;
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) state.mouseButtons[e.button.button] = false;
        }
    }
    InputState GetState() override { return state; }
    bool ShouldQuit() const { return quit; }
private:
    InputState state{};
    bool quit = false;
};
