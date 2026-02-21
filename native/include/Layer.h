#pragma once

#include "GMLTypes.h"
#include <memory>
#include <vector>
#include <string>

namespace GM {

class Instance;
class Sprite;

// Layer types
enum class LayerType {
    Instances = 0,
    Tiles = 1,
    Background = 2,
    Sprites = 3,
    Text = 4,
    Effects = 5,
    Parallax = 6,
};

// Represents a camera/view
class Camera {
public:
    Camera();
    ~Camera();

    uint32_t GetID() const { return id; }
    void SetID(uint32_t i) { id = i; }

    // Position and size
    double GetX() const { return x; }
    double GetY() const { return y; }
    void SetX(double val) { x = val; }
    void SetY(double val) { y = val; }

    double GetWidth() const { return width; }
    double GetHeight() const { return height; }
    void SetWidth(double w) { width = w; }
    void SetHeight(double h) { height = h; }

    // Viewport (where to draw on screen)
    double GetViewportX() const { return viewport_x; }
    double GetViewportY() const { return viewport_y; }
    void SetViewportX(double x) { viewport_x = x; }
    void SetViewportY(double y) { viewport_y = y; }

    double GetViewportWidth() const { return viewport_width; }
    double GetViewportHeight() const { return viewport_height; }
    void SetViewportWidth(double w) { viewport_width = w; }
    void SetViewportHeight(double h) { viewport_height = h; }

    // Properties
    bool GetVisible() const { return visible; }
    void SetVisible(bool val) { visible = val; }

    double GetZoom() const { return zoom; }
    void SetZoom(double z) { zoom = z; }

    double GetAngle() const { return angle; }
    void SetAngle(double a) { angle = a; }

private:
    uint32_t id = 0;
    double x = 0, y = 0;
    double width = 1024, height = 768;
    double viewport_x = 0, viewport_y = 0;
    double viewport_width = 1024, viewport_height = 768;
    bool visible = true;
    double zoom = 1.0;
    double angle = 0.0;
};

// Represents a layer in a room
class Layer {
public:
    Layer(uint32_t id, const std::string& name, LayerType type);
    ~Layer();

    uint32_t GetID() const { return id; }
    const std::string& GetName() const { return name; }
    LayerType GetType() const { return type; }

    // Depth (lower values drawn first)
    double GetDepth() const { return depth; }
    void SetDepth(double d) { depth = d; }

    // Visibility
    bool GetVisible() const { return visible; }
    void SetVisible(bool val) { visible = val; }

    // Opacity
    double GetAlpha() const { return alpha; }
    void SetAlpha(double a) { alpha = a; }

    // Content
    void AddInstance(std::shared_ptr<Instance> inst);
    void RemoveInstance(std::shared_ptr<Instance> inst);
    const std::vector<std::shared_ptr<Instance>>& GetInstances() const { return instances; }

    // Parallax
    double GetParallaxX() const { return parallax_x; }
    double GetParallaxY() const { return parallax_y; }
    void SetParallax(double x, double y) { parallax_x = x; parallax_y = y; }

    // Offset
    double GetOffsetX() const { return offset_x; }
    double GetOffsetY() const { return offset_y; }
    void SetOffset(double x, double y) { offset_x = x; offset_y = y; }

private:
    uint32_t id;
    std::string name;
    LayerType type;
    std::vector<std::shared_ptr<Instance>> instances;

    double depth = 0;
    bool visible = true;
    double alpha = 1.0;
    double parallax_x = 1.0;
    double parallax_y = 1.0;
    double offset_x = 0;
    double offset_y = 0;
};

} // namespace GM
