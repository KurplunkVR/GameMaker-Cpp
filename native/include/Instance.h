#pragma once

#include "GMLTypes.h"
#include <memory>
#include <vector>
#include <map>

namespace GM {

class Object;
class Room;
class Sprite;

class Instance {
public:
    Instance(double x, double y, uint32_t id, std::shared_ptr<Object> object);
    ~Instance();

    // Position
    double GetX() const { return x; }
    double GetY() const { return y; }
    void SetX(double val) { xprevious = x; x = val; }
    void SetY(double val) { yprevious = y; y = val; }
    
    double GetXPrevious() const { return xprevious; }
    double GetYPrevious() const { return yprevious; }
    double GetXStart() const { return xstart; }
    double GetYStart() const { return ystart; }

    // Motion
    double GetHSpeed() const { return hspeed; }
    double GetVSpeed() const { return vspeed; }
    void SetHSpeed(double val) { hspeed = val; }
    void SetVSpeed(double val) { vspeed = val; }
    
    double GetSpeed() const { return speed; }
    double GetDirection() const { return direction; }
    void SetSpeed(double val) { speed = val; }
    void SetDirection(double val) { direction = val; }
    
    double GetFriction() const { return friction; }
    double GetGravity() const { return gravity; }
    double GetGravityDirection() const { return gravity_direction; }
    
    void SetFriction(double val) { friction = val; }
    void SetGravity(double val) { gravity = val; }
    void SetGravityDirection(double val) { gravity_direction = val; }

    // Visibility and drawing
    bool GetVisible() const { return visible; }
    void SetVisible(bool val) { visible = val; }
    
    bool GetActive() const { return active; }
    void SetActive(bool val) { active = val; }
    
    bool GetSolid() const { return solid; }
    void SetSolid(bool val) { solid = val; }
    
    bool GetPersistent() const { return persistent; }
    void SetPersistent(bool val) { persistent = val; }
    
    double GetDepth() const { return depth; }
    void SetDepth(double val) { depth = val; }

    // Sprite
    uint32_t GetSpriteIndex() const { return sprite_index; }
    void SetSpriteIndex(uint32_t val);
    
    double GetImageIndex() const { return image_index; }
    void SetImageIndex(double val) { image_index = val; }
    
    double GetImageXScale() const { return image_xscale; }
    double GetImageYScale() const { return image_yscale; }
    void SetImageXScale(double val) { image_xscale = val; }
    void SetImageYScale(double val) { image_yscale = val; }
    
    double GetImageAngle() const { return image_angle; }
    void SetImageAngle(double val) { image_angle = val; }
    
    double GetImageAlpha() const { return image_alpha; }
    void SetImageAlpha(double val) { image_alpha = val; }
    
    Color GetImageBlend() const { return image_blend; }
    void SetImageBlend(Color val) { image_blend = val; }
    
    double GetImageSpeed() const { return image_speed; }
    void SetImageSpeed(double val) { image_speed = val; }

    // Object reference
    std::shared_ptr<Object> GetObject() const { return object; }
    uint32_t GetObjectIndex() const { return object_index; }

    // BBox
    const Rect& GetBBox() const { return bbox; }
    void UpdateBBox();

    // Alarms
    int GetAlarm(int index) const {
        if (index >= 0 && index < 12) return alarm[index];
        return -1;
    }
    void SetAlarm(int index, int val) {
        if (index >= 0 && index < 12) alarm[index] = val;
    }

    // Events
    void TriggerEvent(EventType type, int subType);
    void CreateEvent();
    void DestroyEvent();
    void StepEvent(StepEventType stepType);
    void DrawEvent();

    // Animation
    void Animate();
    void UpdateAnimation();

    // Update
    void Update();

    // Variables (GML variables)
    Variant GetVariable(const std::string& name);
    void SetVariable(const std::string& name, const Variant& value);

    uint32_t GetID() const { return id; }
    bool IsMarked() const { return marked; }
    void Mark() { marked = true; }

private:
    uint32_t id;
    std::shared_ptr<Object> object;
    uint32_t object_index = 0;

    // Position
    double x = 0, y = 0;
    double xprevious = 0, yprevious = 0;
    double xstart = 0, ystart = 0;

    // Motion
    double hspeed = 0, vspeed = 0;
    double speed = 0, direction = 0;
    double friction = 0, gravity = 0, gravity_direction = 270;

    // Visibility
    bool visible = true;
    bool active = true;
    bool solid = true;
    bool persistent = false;
    double depth = 0;

    // Sprite
    uint32_t sprite_index = 0;
    double image_index = 0;
    double image_xscale = 1.0, image_yscale = 1.0;
    double image_angle = 0;
    double image_alpha = 1.0;
    Color image_blend = 0xFFFFFFFF;
    double image_speed = 1.0;
    uint32_t mask_index = 0;

    // BBox
    Rect bbox;
    bool bbox_dirty = true;

    // Alarms
    int alarm[12] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    // State
    bool marked = false;
    
    // Variables
    std::map<std::string, Variant> variables;
};

} // namespace GM
