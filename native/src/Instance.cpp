#include "Instance.h"
#include "Object.h"
#include "Graphics.h"
#include <cmath>
#include <algorithm>

namespace GM {

Instance::Instance(double x, double y, uint32_t id, std::shared_ptr<Object> object)
    : x(x), y(y), xprevious(x), yprevious(y), xstart(x), ystart(y),
      id(id), object(object), object_index(object ? object->GetID() : 0) {
    
    if (object) {
        sprite_index = object->GetSpriteIndex();
        solid = object->GetSolid();
        visible = object->GetVisible();
        depth = object->GetDepth();
    }
    
    UpdateBBox();
}

Instance::~Instance() {
}

void Instance::SetSpriteIndex(uint32_t val) {
    sprite_index = val;
    bbox_dirty = true;
}

void Instance::UpdateBBox() {
    // TODO: Get actual sprite dimensions
    bbox.x1 = x;
    bbox.y1 = y;
    bbox.x2 = x + 32;  // Default size
    bbox.y2 = y + 32;
    bbox_dirty = false;
}

void Instance::TriggerEvent(EventType type, int subType) {
    if (object) {
        object->TriggerEvent(this, type, subType);
    }
}

void Instance::CreateEvent() {
    TriggerEvent(EventType::Create, 0);
}

void Instance::DestroyEvent() {
    TriggerEvent(EventType::Destroy, 0);
}

void Instance::StepEvent(StepEventType stepType) {
    // Update alarms
    for (int i = 0; i < 12; i++) {
        if (alarm[i] > -1) {
            alarm[i]--;
            if (alarm[i] == -1) {
                TriggerEvent(EventType::Alarm, i);
            }
        }
    }

    // Apply motion
    x += hspeed;
    y += vspeed;

    // Apply gravity
    if (gravity != 0) {
        double angle_rad = gravity_direction * 3.14159265359 / 180.0;
        vspeed += gravity * std::sin(angle_rad);
        hspeed += gravity * std::cos(angle_rad);
    }

    // Apply friction
    if (friction > 0) {
        double speed_mag = std::sqrt(hspeed * hspeed + vspeed * vspeed);
        if (speed_mag > friction) {
            double factor = 1.0 - (friction / speed_mag);
            hspeed *= factor;
            vspeed *= factor;
        } else {
            hspeed = 0;
            vspeed = 0;
        }
    }

    TriggerEvent(EventType::Step, (int)stepType);
}

void Instance::DrawEvent() {
    // Draw the sprite
    // TODO: Get graphics context from engine/platform
    // if (sprite_index > 0 && graphics) {
    //     graphics->DrawSprite(sprite, (uint32_t)image_index, x, y,
    //                          image_xscale, image_yscale, image_angle,
    //                          image_blend, image_alpha);
    // }
    TriggerEvent(EventType::Draw, 0);
}

void Instance::Animate() {
    if (sprite_index == 0) return;
    
    // TODO: Get actual sprite frame count
    image_index += image_speed;
    if (image_index >= 1.0) {
        image_index = 0;
    }
}

void Instance::UpdateAnimation() {
    Animate();
}

void Instance::Update() {
    if (bbox_dirty) {
        UpdateBBox();
    }
}

Variant Instance::GetVariable(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return Variant();
}

void Instance::SetVariable(const std::string& name, const Variant& value) {
    variables[name] = value;
}

} // namespace GM
