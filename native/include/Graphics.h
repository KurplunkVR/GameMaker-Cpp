#pragma once

#include "GMLTypes.h"
#include "Sprite.h"
#include <memory>
#include <cstdint>

namespace GM {

// Represents a texture (image) that can be rendered
class Texture {
public:
    Texture(uint32_t width, uint32_t height);
    virtual ~Texture();

    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }

    // Get texture handle for rendering
    uint32_t GetHandle() const { return handle; }
    void SetHandle(uint32_t h) { handle = h; }

    // Get raw pixel data (if applicable)
    uint8_t* GetPixelData() { return pixel_data; }
    const uint8_t* GetPixelData() const { return pixel_data; }

    void LoadFromPixels(const uint8_t* data, uint32_t pitch, bool rgba);

private:
    uint32_t width;
    uint32_t height;
    uint32_t handle = 0;  // OpenGL texture ID or platform-specific handle
    uint8_t* pixel_data = nullptr;
};

// Graphics context - manages drawing operations
class Graphics {
public:
    Graphics();
    ~Graphics();

    // Drawing primitives
    void DrawSprite(const Sprite* sprite, uint32_t frame_index, double x, double y,
                    double xscale = 1.0, double yscale = 1.0, double angle = 0.0,
                    Color blend = 0xFFFFFFFF, double alpha = 1.0);

    void DrawSpriteExt(const Sprite* sprite, uint32_t frame_index, double x, double y,
                       double xscale, double yscale, double angle,
                       Color blend, double alpha);

    void DrawRect(double x1, double y1, double x2, double y2, Color color, bool filled = true);
    void DrawCircle(double x, double y, double radius, Color color, bool filled = true);
    void DrawLine(double x1, double y1, double x2, double y2, Color color);
    void DrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3,
                      Color color, bool filled = true);

    // Clear screen
    void Clear(Color color = 0xFF000000);
    void ClearAlpha(Color color = 0xFF000000);

    // View/Camera
    void SetViewport(double x, double y, double width, double height);
    void ResetViewport();

    // Transform stack
    void Push();
    void Pop();
    void Translate(double x, double y);
    void Rotate(double angle);
    void Scale(double x, double y);

    // Blending
    enum class BlendMode {
        Normal = 0,
        Add = 1,
        Subtract = 2,
        Multiply = 3,
    };
    void SetBlendMode(BlendMode mode);
    void ResetBlendMode();

    // Alpha testing
    void SetAlphaTest(bool enabled, float threshold = 0.5f);

    // Text rendering (placeholder)
    void DrawText(const std::string& text, double x, double y, Color color);

    // State
    bool IsInitialized() const { return initialized; }
    void SetInitialized(bool val) { initialized = val; }

protected:
    bool initialized = false;
    BlendMode current_blend_mode = BlendMode::Normal;
    bool alpha_test_enabled = false;
    float alpha_test_threshold = 0.5f;

    // Transform state
    std::vector<Vector2> transform_stack;
    std::vector<double> rotation_stack;
    std::vector<Vector2> scale_stack;
};

} // namespace GM
