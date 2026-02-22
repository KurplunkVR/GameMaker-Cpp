#pragma once

class IRenderer {
public:
    virtual ~IRenderer() {}
    virtual bool Init(int width, int height) = 0;
    virtual void Present() = 0;
    virtual void Clear(unsigned int color = 0xFF000000) = 0;
    
    // Drawing primitives
    virtual void DrawRect(int x, int y, int width, int height, unsigned int color, bool filled = true) = 0;
    virtual void DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int color) = 0;
    
    // State management
    virtual void SetClearColor(unsigned int color) = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
};
