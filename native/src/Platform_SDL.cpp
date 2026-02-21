#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "../include/IPlatform.h"
#include <cstdio>

class RendererSDL : public IRenderer {
public:
    bool Init(int width, int height) override {
        printf("[Renderer] Initializing SDL...\n");
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            printf("[Renderer] SDL_Init failed: %s\n", SDL_GetError());
            return false;
        }
        printf("[Renderer] Creating window %dx%d...\n", width, height);
        window = SDL_CreateWindow("GameMakerCpp", width, height, SDL_WINDOW_OPENGL);
        if (!window) {
            printf("[Renderer] SDL_CreateWindow failed: %s\n", SDL_GetError());
            return false;
        }
        printf("[Renderer] Creating GL context...\n");
        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            printf("[Renderer] SDL_GL_CreateContext failed: %s\n", SDL_GetError());
            return false;
        }
        printf("[Renderer] Initialization complete!\n");
        return true;
    }
    void Present() override { SDL_GL_SwapWindow(window); }
    void Clear() override { glClear(GL_COLOR_BUFFER_BIT); }
private:
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
};

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

class PlatformSDL : public IPlatform {
public:
    PlatformSDL() : renderer(new RendererSDL()), input(new InputSDL()) {}
    ~PlatformSDL() { delete renderer; delete input; }
    IRenderer* GetRenderer() override { return renderer; }
    IAudio* GetAudio() override { return nullptr; } // TODO: Implement
    IInput* GetInput() override { return input; }
    IFileIO* GetFileIO() override { return nullptr; } // TODO: Implement
    ITimer* GetTimer() override { return nullptr; } // TODO: Implement
private:
    RendererSDL* renderer;
    InputSDL* input;
};

extern "C" IPlatform* CreatePlatform() { return new PlatformSDL(); }
