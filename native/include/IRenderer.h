#pragma once

class IRenderer {
public:
    virtual ~IRenderer() {}
    virtual bool Init(int width, int height) = 0;
    virtual void Present() = 0;
    virtual void Clear() = 0;
    // Add more: createTexture, drawSprite, etc.
};
