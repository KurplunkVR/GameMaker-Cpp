#include <SDL3/SDL.h>
#include "../include/IPlatform.h"
#include <cstdio>

class RendererSDL : public IRenderer {
public:
    RendererSDL() = default;
    ~RendererSDL() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    bool Init(int width, int height) override {
        printf("[Renderer] Initializing SDL...\n");
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            printf("[Renderer] SDL_Init failed: %s\n", SDL_GetError());
            return false;
        }
        printf("[Renderer] Creating window %dx%d...\n", width, height);
        window = SDL_CreateWindow("GameMakerCpp", width, height, 0);  // 0 = default visible window
        if (!window) {
            printf("[Renderer] SDL_CreateWindow failed: %s\n", SDL_GetError());
            return false;
        }
        
        printf("[Renderer] Creating SDL renderer...\n");
        renderer = SDL_CreateRenderer(window, NULL);
        if (!renderer) {
            printf("[Renderer] SDL_CreateRenderer failed: %s\n", SDL_GetError());
            return false;
        }
        
        // Enable vsync
        SDL_SetRenderVSync(renderer, 1);
        
        screen_width = width;
        screen_height = height;
        
        printf("[Renderer] Initialization complete!\n");
        return true;
    }
    
    void BeginFrame() override {
        // Clear will be done with SetClearColor + Clear
    }
    
    void EndFrame() override {
        SDL_RenderPresent(renderer);
    }
    
    void Present() override { 
        SDL_RenderPresent(renderer); 
    }
    
    void Clear(unsigned int color) override { 
        SetClearColor(color);
        if (renderer) {
            SDL_RenderClear(renderer);
        }
    }
    
    void SetClearColor(unsigned int color) override {
        if (!renderer) return;
        unsigned char r = (color >> 16) & 0xFF;
        unsigned char g = (color >> 8) & 0xFF;
        unsigned char b = color & 0xFF;
        unsigned char a = (color >> 24) & 0xFF;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }
    
    void DrawRect(int x, int y, int width, int height, unsigned int color, bool filled) override {
        if (!renderer) return;
        
        unsigned char r = (color >> 16) & 0xFF;
        unsigned char g = (color >> 8) & 0xFF;
        unsigned char b = color & 0xFF;
        unsigned char a = (color >> 24) & 0xFF;
        
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_FRect rect;
        rect.x = (float)x;
        rect.y = (float)y;
        rect.w = (float)width;
        rect.h = (float)height;
        
        if (filled) {
            SDL_RenderFillRect(renderer, &rect);
        } else {
            SDL_RenderRect(renderer, &rect);
        }
    }
    
    void DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int color) override {
        if (!renderer) return;
        
        unsigned char r = (color >> 16) & 0xFF;
        unsigned char g = (color >> 8) & 0xFF;
        unsigned char b = color & 0xFF;
        unsigned char a = (color >> 24) & 0xFF;
        
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        
        // Draw quad as two triangles using lines (SDL doesn't have triangle fill natively)
        SDL_RenderLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
        SDL_RenderLine(renderer, (int)x2, (int)y2, (int)x3, (int)y3);
        SDL_RenderLine(renderer, (int)x3, (int)y3, (int)x4, (int)y4);
        SDL_RenderLine(renderer, (int)x4, (int)y4, (int)x1, (int)y1);
    }
    
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int screen_width = 0;
    int screen_height = 0;
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
