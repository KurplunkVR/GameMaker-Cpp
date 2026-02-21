#include "Managers.h"
#include <algorithm>

namespace GM {

GameGlobals* GameGlobals::instance = nullptr;

// ObjectManager
ObjectManager::ObjectManager() {
}

ObjectManager::~ObjectManager() {
    Clear();
}

void ObjectManager::AddObject(std::shared_ptr<Object> obj) {
    object_map[obj->GetID()] = obj;
    objects.push_back(obj);
}

void ObjectManager::RemoveObject(uint32_t id) {
    auto it = object_map.find(id);
    if (it != object_map.end()) {
        auto obj = it->second;
        object_map.erase(it);
        objects.erase(std::find(objects.begin(), objects.end(), obj));
    }
}

std::shared_ptr<Object> ObjectManager::GetObject(uint32_t id) {
    auto it = object_map.find(id);
    if (it != object_map.end()) {
        return it->second;
    }
    return nullptr;
}

void ObjectManager::Clear() {
    objects.clear();
    object_map.clear();
}

// InstanceManager
InstanceManager::InstanceManager() {
}

InstanceManager::~InstanceManager() {
    Clear();
}

void InstanceManager::AddInstance(std::shared_ptr<Instance> inst) {
    instance_map[inst->GetID()] = inst;
    instances.push_back(inst);
}

void InstanceManager::RemoveInstance(uint32_t id) {
    auto it = instance_map.find(id);
    if (it != instance_map.end()) {
        auto inst = it->second;
        instance_map.erase(it);
        instances.erase(std::find(instances.begin(), instances.end(), inst));
    }
}

std::shared_ptr<Instance> InstanceManager::GetInstance(uint32_t id) {
    auto it = instance_map.find(id);
    if (it != instance_map.end()) {
        return it->second;
    }
    return nullptr;
}

void InstanceManager::Update() {
    for (auto& inst : instances) {
        if (inst->GetActive()) {
            inst->StepEvent(StepEventType::NormalStep);
        }
    }
    RemoveMarked();
}

void InstanceManager::Draw() {
    for (auto& inst : instances) {
        if (inst->GetVisible()) {
            inst->DrawEvent();
        }
    }
}

void InstanceManager::Clear() {
    instances.clear();
    instance_map.clear();
}

void InstanceManager::TriggerEvent(EventType type, int subType) {
    for (auto& inst : instances) {
        inst->TriggerEvent(type, subType);
    }
}

void InstanceManager::RemoveMarked() {
    instances.erase(
        std::remove_if(instances.begin(), instances.end(),
            [this](const std::shared_ptr<Instance>& inst) {
                if (inst->IsMarked()) {
                    instance_map.erase(inst->GetID());
                    return true;
                }
                return false;
            }),
        instances.end()
    );
}

// RoomManager
RoomManager::RoomManager() {
}

RoomManager::~RoomManager() {
    Clear();
}

void RoomManager::AddRoom(std::shared_ptr<Room> room) {
    room_map[room->GetID()] = room;
    rooms.push_back(room);
}

void RoomManager::RemoveRoom(uint32_t id) {
    auto it = room_map.find(id);
    if (it != room_map.end()) {
        auto room = it->second;
        room_map.erase(it);
        rooms.erase(std::find(rooms.begin(), rooms.end(), room));
    }
}

std::shared_ptr<Room> RoomManager::GetRoom(uint32_t id) {
    auto it = room_map.find(id);
    if (it != room_map.end()) {
        return it->second;
    }
    return nullptr;
}

void RoomManager::Clear() {
    rooms.clear();
    room_map.clear();
    current_room = nullptr;
}

// SpriteManager
void SpriteManager::AddSprite(std::shared_ptr<Sprite> sprite) {
    sprite_map[sprite->GetID()] = sprite;
    sprites.push_back(sprite);
}

std::shared_ptr<Sprite> SpriteManager::GetSprite(uint32_t id) {
    auto it = sprite_map.find(id);
    if (it != sprite_map.end()) {
        return it->second;
    }
    return nullptr;
}

void SpriteManager::Clear() {
    sprites.clear();
    sprite_map.clear();
}

// GameGlobals
GameGlobals::GameGlobals() {
    if (instance == nullptr) {
        instance = this;
    }
}

GameGlobals::~GameGlobals() {
    if (instance == this) {
        instance = nullptr;
    }
}

GameGlobals& GameGlobals::Get() {
    if (instance == nullptr) {
        instance = new GameGlobals();
    }
    return *instance;
}

} // namespace GM
