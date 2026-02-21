#include "Sprite.h"
#include "Graphics.h"

namespace GM {

// SpriteFrame implementation
SpriteFrame::SpriteFrame() {
}

SpriteFrame::~SpriteFrame() {
}

// Sprite implementation
Sprite::Sprite(uint32_t id, const std::string& name)
    : id(id), name(name) {
}

Sprite::~Sprite() {
    Clear();
}

void Sprite::AddFrame(const SpriteFrame& frame) {
    frames.push_back(frame);
}

SpriteFrame* Sprite::GetFrame(uint32_t index) {
    if (index < frames.size()) {
        return &frames[index];
    }
    return nullptr;
}

const SpriteFrame* Sprite::GetFrame(uint32_t index) const {
    if (index < frames.size()) {
        return &frames[index];
    }
    return nullptr;
}

void Sprite::Clear() {
    frames.clear();
}

} // namespace GM
