#include "Layer.h"
#include "Instance.h"
#include <algorithm>

namespace GM {

// Camera implementation
Camera::Camera() {
}

Camera::~Camera() {
}

// Layer implementation
Layer::Layer(uint32_t id, const std::string& name, LayerType type)
    : id(id), name(name), type(type) {
}

Layer::~Layer() {
    instances.clear();
}

void Layer::AddInstance(std::shared_ptr<Instance> inst) {
    if (!inst) return;
    auto it = std::find(instances.begin(), instances.end(), inst);
    if (it == instances.end()) {
        instances.push_back(inst);
    }
}

void Layer::RemoveInstance(std::shared_ptr<Instance> inst) {
    if (!inst) return;
    auto it = std::find(instances.begin(), instances.end(), inst);
    if (it != instances.end()) {
        instances.erase(it);
    }
}

} // namespace GM
