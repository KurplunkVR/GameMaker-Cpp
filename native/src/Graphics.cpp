#include "Graphics.h"
#include <cmath>
#include <cstdio>
#include <cstring>

// Note: GL headers will be included indirectly through SDL if needed
// For now, we use minimal OpenGL calls through wgl functions

namespace GM {

// Texture implementation
Texture::Texture(uint32_t width, uint32_t height)
    : width(width), height(height) {
    pixel_data = new uint8_t[width * height * 4];
}

Texture::~Texture() {
    if (pixel_data) {
        delete[] pixel_data;
        pixel_data = nullptr;
    }
}

void Texture::LoadFromPixels(const uint8_t* data, uint32_t pitch, bool rgba) {
    if (!data || !pixel_data) return;
    
    uint32_t copy_pitch = width * (rgba ? 4 : 3);
    for (uint32_t y = 0; y < height; y++) {
        const uint8_t* src = data + (y * pitch);
        uint8_t* dst = pixel_data + (y * copy_pitch);
        std::memcpy(dst, src, copy_pitch);
    }
}

// Graphics implementation
Graphics::Graphics() {
}

Graphics::~Graphics() {
}

void Graphics::DrawSprite(const Sprite* sprite, uint32_t frame_index, double x, double y,
                          double xscale, double yscale, double angle,
                          Color blend, double alpha) {
    DrawSpriteExt(sprite, frame_index, x, y, xscale, yscale, angle, blend, alpha);
}

void Graphics::DrawSpriteExt(const Sprite* sprite, uint32_t frame_index, double x, double y,
                             double xscale, double yscale, double angle,
                             Color blend, double alpha) {
    if (!sprite) return;
    
    const SpriteFrame* frame = sprite->GetFrame(frame_index % sprite->GetFrameCount());
    if (!frame) return;

    // TODO: Implement actual sprite drawing using OpenGL or SDL
    // For now, this is a placeholder
}

void Graphics::DrawRect(double x1, double y1, double x2, double y2, Color color, bool filled) {
    // TODO: Implement rectangle drawing
}

void Graphics::DrawCircle(double x, double y, double radius, Color color, bool filled) {
    // TODO: Implement circle drawing
}

void Graphics::DrawLine(double x1, double y1, double x2, double y2, Color color) {
    // TODO: Implement line drawing
}

void Graphics::DrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3,
                            Color color, bool filled) {
    // TODO: Implement triangle drawing
}

void Graphics::Clear(Color color) {
    // TODO: Clear through SDL renderer instead of direct GL
    // For now, just a placeholder that does nothing
}

void Graphics::ClearAlpha(Color color) {
    // TODO: Clear through SDL renderer
}

void Graphics::SetViewport(double x, double y, double width, double height) {
    // TODO: Set viewport through SDL renderer
}

void Graphics::ResetViewport() {
    // TODO: Reset to default viewport
}

void Graphics::Push() {
    transform_stack.push_back(Vector2(0, 0));
    rotation_stack.push_back(0);
    scale_stack.push_back(Vector2(1, 1));
    // TODO: Push matrix through SDL
}

void Graphics::Pop() {
    if (!transform_stack.empty()) {
        transform_stack.pop_back();
        rotation_stack.pop_back();
        scale_stack.pop_back();
        // TODO: Pop matrix through SDL
    }
}

void Graphics::Translate(double x, double y) {
    // TODO: Translate through SDL
}

void Graphics::Rotate(double angle) {
    // TODO: Rotate through SDL
}

void Graphics::Scale(double x, double y) {
    // TODO: Scale through SDL
}

void Graphics::SetBlendMode(BlendMode mode) {
    current_blend_mode = mode;
    
    // TODO: Set blend mode through SDL
    switch (mode) {
        case BlendMode::Normal:
            // Use standard alpha blending
            break;
        case BlendMode::Add:
            // Use additive blending
            break;
        case BlendMode::Subtract:
            // Use subtractive blending
            break;
        case BlendMode::Multiply:
            // Use multiplicative blending
            break;
    }
}

void Graphics::ResetBlendMode() {
    SetBlendMode(BlendMode::Normal);
}

void Graphics::SetAlphaTest(bool enabled, float threshold) {
    alpha_test_enabled = enabled;
    alpha_test_threshold = threshold;
    
    // TODO: Set alpha test through SDL
}

void Graphics::DrawText(const std::string& text, double x, double y, Color color) {
    // TODO: Implement text rendering using a font system
}

} // namespace GM
