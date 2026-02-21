#pragma once

#include "GMLTypes.h"
#include <vector>
#include <memory>
#include <cstdint>

namespace GM {

// Sprite collision types
enum class SpriteCollisionType {
    AxisAlignedRect = 0,
    Precise = 1,
    RotatedRect = 2,
    SpineMesh = 3,
};

// Playback speed type
enum class PlaybackSpeedType {
    FramesPerSecond = 0,
    FramesPerGameFrame = 1,
};

// Forward declarations
class Texture;

// Represents a single frame of a sprite
class SpriteFrame {
public:
    SpriteFrame();
    ~SpriteFrame();

    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }
    void SetWidth(uint32_t w) { width = w; }
    void SetHeight(uint32_t h) { height = h; }

    const std::shared_ptr<Texture>& GetTexture() const { return texture; }
    void SetTexture(std::shared_ptr<Texture> tex) { texture = tex; }

    double GetDuration() const { return duration; }
    void SetDuration(double d) { duration = d; }

private:
    uint32_t width = 0;
    uint32_t height = 0;
    std::shared_ptr<Texture> texture;
    double duration = 1.0;  // Duration in frames or seconds depending on speed type
};

// Represents a complete sprite with multiple frames
class Sprite {
public:
    Sprite(uint32_t id, const std::string& name);
    ~Sprite();

    uint32_t GetID() const { return id; }
    const std::string& GetName() const { return name; }

    // Frame management
    uint32_t GetFrameCount() const { return (uint32_t)frames.size(); }
    void AddFrame(const SpriteFrame& frame);
    SpriteFrame* GetFrame(uint32_t index);
    const SpriteFrame* GetFrame(uint32_t index) const;

    // Origin (hotspot)
    uint32_t GetXOrigin() const { return x_origin; }
    uint32_t GetYOrigin() const { return y_origin; }
    void SetXOrigin(uint32_t x) { x_origin = x; }
    void SetYOrigin(uint32_t y) { y_origin = y; }

    // Properties
    bool GetTransparent() const { return transparent; }
    void SetTransparent(bool val) { transparent = val; }

    bool GetSmooth() const { return smooth; }
    void SetSmooth(bool val) { smooth = val; }

    bool GetPreload() const { return preload; }
    void SetPreload(bool val) { preload = val; }

    // Bounding box
    const Rect& GetBBox() const { return bbox; }
    void SetBBox(const Rect& bb) { bbox = bb; }

    // Collision
    SpriteCollisionType GetCollisionType() const { return collision_type; }
    void SetCollisionType(SpriteCollisionType type) { collision_type = type; }

    // Speed
    double GetPlaybackSpeed() const { return playback_speed; }
    void SetPlaybackSpeed(double speed) { playback_speed = speed; }

    PlaybackSpeedType GetPlaybackSpeedType() const { return playback_speed_type; }
    void SetPlaybackSpeedType(PlaybackSpeedType type) { playback_speed_type = type; }

    // Bounding box mode (0=automatic, 1=full, 2=manual)
    int GetBBoxMode() const { return bbox_mode; }
    void SetBBoxMode(int mode) { bbox_mode = mode; }

    // Culling
    double GetCullRadius() const { return cull_radius; }
    void SetCullRadius(double radius) { cull_radius = radius; }

    // Clear all frames
    void Clear();

private:
    uint32_t id;
    std::string name;
    std::vector<SpriteFrame> frames;

    uint32_t x_origin = 0;
    uint32_t y_origin = 0;
    bool transparent = true;
    bool smooth = true;
    bool preload = true;
    Rect bbox;
    SpriteCollisionType collision_type = SpriteCollisionType::AxisAlignedRect;
    double playback_speed = 1.0;
    PlaybackSpeedType playback_speed_type = PlaybackSpeedType::FramesPerSecond;
    int bbox_mode = 0;
    double cull_radius = 8.0;
};

} // namespace GM
