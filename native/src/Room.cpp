#include "Room.h"
#include <algorithm>

namespace GM {

Room::Room(uint32_t id, const std::string& name)
    : id(id), name(name) {
}

Room::~Room() {
    Clear();
}

void Room::AddInstance(std::shared_ptr<Instance> inst) {
    auto it = std::find(instances.begin(), instances.end(), inst);
    if (it == instances.end()) {
        instances.push_back(inst);
    }
}

void Room::RemoveInstance(std::shared_ptr<Instance> inst) {
    auto it = std::find(instances.begin(), instances.end(), inst);
    if (it != instances.end()) {
        instances.erase(it);
    }
}

std::shared_ptr<Instance> Room::FindInstance(uint32_t id) {
    for (auto& inst : instances) {
        if (inst->GetID() == id) {
            return inst;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Instance>> Room::FindInstancesByObject(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Instance>> result;
    for (auto& inst : instances) {
        if (inst->GetObject() == obj) {
            result.push_back(inst);
        }
    }
    return result;
}

void Room::Init() {
    for (auto& inst : instances) {
        inst->CreateEvent();
    }
    initialized = true;
}

void Room::Clear() {
    instances.clear();
    instances_to_add.clear();
}

void Room::RoomStartEvent() {
    // Called when room starts
}

void Room::RoomEndEvent() {
    // Called when room ends
}

void Room::Update() {
    // Add any pending instances
    for (auto& inst : instances_to_add) {
        instances.push_back(inst);
    }
    instances_to_add.clear();

    // Update all instances
    for (auto& inst : instances) {
        if (inst->GetActive()) {
            inst->Update();
        }
    }

    RemoveMarked();
}

void Room::Draw() {
    // Sort instances by depth (lower depth = drawn first)
    std::sort(instances.begin(), instances.end(),
        [](const std::shared_ptr<Instance>& a, const std::shared_ptr<Instance>& b) {
            return a->GetDepth() < b->GetDepth();
        });

    for (auto& inst : instances) {
        if (inst->GetVisible()) {
            inst->DrawEvent();
        }
    }
}

void Room::RemoveMarked() {
    instances.erase(
        std::remove_if(instances.begin(), instances.end(),
            [](const std::shared_ptr<Instance>& inst) {
                return inst->IsMarked();
            }),
        instances.end()
    );
}

void Room::UpdateBBoxes() {
    for (auto& inst : instances) {
        inst->UpdateBBox();
    }
}

void Room::AddLayer(std::shared_ptr<Layer> layer) {
    if (!layer) return;
    layer_map[layer->GetID()] = layer;
    layers.push_back(layer);
}

void Room::RemoveLayer(uint32_t id) {
    auto it = layer_map.find(id);
    if (it != layer_map.end()) {
        auto layer = it->second;
        layer_map.erase(it);
        layers.erase(std::find(layers.begin(), layers.end(), layer));
    }
}

std::shared_ptr<Layer> Room::GetLayer(uint32_t id) {
    auto it = layer_map.find(id);
    if (it != layer_map.end()) {
        return it->second;
    }
    return nullptr;
}

void Room::AddCamera(std::shared_ptr<Camera> camera) {
    if (!camera) return;
    camera_map[camera->GetID()] = camera;
    cameras.push_back(camera);
}

void Room::RemoveCamera(uint32_t id) {
    auto it = camera_map.find(id);
    if (it != camera_map.end()) {
        auto camera = it->second;
        camera_map.erase(it);
        cameras.erase(std::find(cameras.begin(), cameras.end(), camera));
    }
}

std::shared_ptr<Camera> Room::GetCamera(uint32_t id) {
    auto it = camera_map.find(id);
    if (it != camera_map.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace GM
