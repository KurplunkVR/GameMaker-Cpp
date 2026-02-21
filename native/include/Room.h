#pragma once

#include "GMLTypes.h"
#include "Instance.h"
#include "Layer.h"
#include <memory>
#include <vector>
#include <map>
#include <string>

namespace GM {

class Room {
public:
    Room(uint32_t id, const std::string& name);
    ~Room();

    uint32_t GetID() const { return id; }
    const std::string& GetName() const { return name; }

    // Dimensions
    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }
    void SetWidth(uint32_t w) { width = w; }
    void SetHeight(uint32_t h) { height = h; }

    // Speed
    double GetSpeed() const { return speed; }
    void SetSpeed(double s) { speed = s; }

    // Properties
    const std::string& GetCaption() const { return caption; }
    void SetCaption(const std::string& cap) { caption = cap; }

    bool GetPersistent() const { return persistent; }
    void SetPersistent(bool val) { persistent = val; }

    Color GetBackgroundColor() const { return background_color; }
    void SetBackgroundColor(Color col) { background_color = col; }

    // Instances
    void AddInstance(std::shared_ptr<Instance> inst);
    void RemoveInstance(std::shared_ptr<Instance> inst);
    const std::vector<std::shared_ptr<Instance>>& GetInstances() const { return instances; }
    std::vector<std::shared_ptr<Instance>>& GetInstances() { return instances; }

    // Find instance by ID
    std::shared_ptr<Instance> FindInstance(uint32_t id);
    std::vector<std::shared_ptr<Instance>> FindInstancesByObject(std::shared_ptr<Object> obj);

    // Initialization
    void Init();
    void Clear();

    // Events
    void RoomStartEvent();
    void RoomEndEvent();
    void Update();
    void Draw();

    // Instance management
    void RemoveMarked();
    void UpdateBBoxes();

    // Views
    bool GetViewsEnabled() const { return views_enabled; }
    void SetViewsEnabled(bool val) { views_enabled = val; }

    // Layers
    void AddLayer(std::shared_ptr<Layer> layer);
    void RemoveLayer(uint32_t id);
    std::shared_ptr<Layer> GetLayer(uint32_t id);
    const std::vector<std::shared_ptr<Layer>>& GetLayers() const { return layers; }

    // Cameras
    void AddCamera(std::shared_ptr<Camera> camera);
    void RemoveCamera(uint32_t id);
    std::shared_ptr<Camera> GetCamera(uint32_t id);
    const std::vector<std::shared_ptr<Camera>>& GetCameras() const { return cameras; }
    std::shared_ptr<Camera> GetActiveCamera() const { return active_camera; }
    void SetActiveCamera(std::shared_ptr<Camera> cam) { active_camera = cam; }

    bool IsInitialized() const { return initialized; }
    void SetInitialized(bool val) { initialized = val; }

private:
    uint32_t id;
    std::string name;
    uint32_t width = 1024;
    uint32_t height = 768;
    double speed = 60;
    std::string caption;
    bool persistent = false;
    Color background_color = 0xFF000000;

    std::vector<std::shared_ptr<Layer>> layers;
    std::map<uint32_t, std::shared_ptr<Layer>> layer_map;

    std::vector<std::shared_ptr<Camera>> cameras;
    std::map<uint32_t, std::shared_ptr<Camera>> camera_map;
    std::shared_ptr<Camera> active_camera;

    std::vector<std::shared_ptr<Instance>> instances;
    std::vector<std::shared_ptr<Instance>> instances_to_add;

    bool views_enabled = true;
    bool initialized = false;
};

} // namespace GM
