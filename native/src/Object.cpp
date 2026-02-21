#include "Object.h"
#include <algorithm>

namespace GM {

Object::Object(uint32_t id, const std::string& name)
    : id(id), name(name) {
}

Object::~Object() {
    instances.clear();
}

void Object::AddInstance(std::shared_ptr<Instance> inst) {
    auto it = std::find(instances.begin(), instances.end(), inst);
    if (it == instances.end()) {
        instances.push_back(inst);
    }
}

void Object::RemoveInstance(std::shared_ptr<Instance> inst) {
    auto it = std::find(instances.begin(), instances.end(), inst);
    if (it != instances.end()) {
        instances.erase(it);
    }
}

void Object::SetEventCallback(EventType type, int subType, EventCallback callback) {
    event_callbacks[(int)type][subType] = callback;
}

void Object::TriggerEvent(Instance* inst, EventType type, int subType) {
    auto type_it = event_callbacks.find((int)type);
    if (type_it != event_callbacks.end()) {
        auto subtype_it = type_it->second.find(subType);
        if (subtype_it != type_it->second.end()) {
            subtype_it->second(inst);
        }
    }
}

Variant Object::GetVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return Variant();
}

void Object::SetVariable(const std::string& name, const Variant& value) {
    variables[name] = value;
}

std::shared_ptr<Instance> Object::CreateInstance(double x, double y, uint32_t id) {
    auto inst = std::make_shared<Instance>(x, y, id, shared_from_this());
    AddInstance(inst);
    return inst;
}

} // namespace GM
