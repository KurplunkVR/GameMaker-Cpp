#pragma once

#include "GMLTypes.h"
#include "Instance.h"
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace GM {

class Object : public std::enable_shared_from_this<Object> {
public:
    Object(uint32_t id, const std::string& name);
    ~Object();

    uint32_t GetID() const { return id; }
    const std::string& GetName() const { return name; }

    // Parent object
    std::shared_ptr<Object> GetParent() const { return parent; }
    void SetParent(std::shared_ptr<Object> p) { parent = p; }

    // Sprite
    uint32_t GetSpriteIndex() const { return sprite_index; }
    void SetSpriteIndex(uint32_t idx) { sprite_index = idx; }

    // Mask
    uint32_t GetMaskIndex() const { return mask_index; }
    void SetMaskIndex(uint32_t idx) { mask_index = idx; }

    // Visibility
    bool GetVisible() const { return visible; }
    void SetVisible(bool val) { visible = val; }

    bool GetSolid() const { return solid; }
    void SetSolid(bool val) { solid = val; }

    // Depth
    double GetDepth() const { return depth; }
    void SetDepth(double val) { depth = val; }

    // Physics
    bool GetPhysicsEnabled() const { return physics_enabled; }
    void SetPhysicsEnabled(bool val) { physics_enabled = val; }

    // Instances
    void AddInstance(std::shared_ptr<Instance> inst);
    void RemoveInstance(std::shared_ptr<Instance> inst);
    const std::vector<std::shared_ptr<Instance>>& GetInstances() const { return instances; }

    // Events
    using EventCallback = std::function<void(Instance*)>;
    void SetEventCallback(EventType type, int subType, EventCallback callback);
    void TriggerEvent(Instance* inst, EventType type, int subType);

    // Variables (default values for new instances)
    Variant GetVariable(const std::string& name) const;
    void SetVariable(const std::string& name, const Variant& value);

    // Create instance
    std::shared_ptr<Instance> CreateInstance(double x, double y, uint32_t id);

private:
    uint32_t id;
    std::string name;
    std::shared_ptr<Object> parent;

    uint32_t sprite_index = 0;
    uint32_t mask_index = 0;
    bool visible = true;
    bool solid = true;
    double depth = 0;
    bool physics_enabled = false;

    std::vector<std::shared_ptr<Instance>> instances;
    
    // Event callbacks: [EventType][subType] = callback
    std::map<int, std::map<int, EventCallback>> event_callbacks;
    
    // Default variables for instances
    std::map<std::string, Variant> variables;
};

} // namespace GM
