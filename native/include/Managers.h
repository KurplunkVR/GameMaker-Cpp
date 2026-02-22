#pragma once

#include "GMLTypes.h"
#include "Object.h"
#include "Instance.h"
#include "Room.h"
#include "Sprite.h"
#include "Audio.h"
#include "Layer.h"
#include "IPlatform.h"
#include <memory>
#include <vector>
#include <map>

namespace GM {

// Manages all game objects
class ObjectManager {
public:
    ObjectManager();
    ~ObjectManager();

    void AddObject(std::shared_ptr<Object> obj);
    void RemoveObject(uint32_t id);
    std::shared_ptr<Object> GetObject(uint32_t id);
    const std::vector<std::shared_ptr<Object>>& GetObjects() const { return objects; }

    void Clear();

private:
    std::vector<std::shared_ptr<Object>> objects;
    std::map<uint32_t, std::shared_ptr<Object>> object_map;
};

// Manages all instances in current room
class InstanceManager {
public:
    InstanceManager();
    ~InstanceManager();

    void AddInstance(std::shared_ptr<Instance> inst);
    void RemoveInstance(uint32_t id);
    std::shared_ptr<Instance> GetInstance(uint32_t id);
    const std::vector<std::shared_ptr<Instance>>& GetInstances() const { return instances; }
    std::vector<std::shared_ptr<Instance>>& GetInstances() { return instances; }

    void Update();
    void Draw();
    void Clear();

    void TriggerEvent(EventType type, int subType);
    void RemoveMarked();

private:
    std::vector<std::shared_ptr<Instance>> instances;
    std::map<uint32_t, std::shared_ptr<Instance>> instance_map;
};

// Manages all rooms
class RoomManager {
public:
    RoomManager();
    ~RoomManager();

    void AddRoom(std::shared_ptr<Room> room);
    void RemoveRoom(uint32_t id);
    std::shared_ptr<Room> GetRoom(uint32_t id);
    std::shared_ptr<Room> GetCurrentRoom() const { return current_room; }
    void SetCurrentRoom(std::shared_ptr<Room> room) { current_room = room; }

    const std::vector<std::shared_ptr<Room>>& GetRooms() const { return rooms; }

    void Clear();

private:
    std::vector<std::shared_ptr<Room>> rooms;
    std::map<uint32_t, std::shared_ptr<Room>> room_map;
    std::shared_ptr<Room> current_room;
};

// Manages sprites
class SpriteManager {
public:
    void AddSprite(std::shared_ptr<Sprite> sprite);
    std::shared_ptr<Sprite> GetSprite(uint32_t id);
    const std::vector<std::shared_ptr<Sprite>>& GetSprites() const { return sprites; }

    void Clear();

private:
    std::vector<std::shared_ptr<Sprite>> sprites;
    std::map<uint32_t, std::shared_ptr<Sprite>> sprite_map;
};

// Main game globals
class GameGlobals {
public:
    GameGlobals();
    ~GameGlobals();

    static GameGlobals& Get();

    ObjectManager& GetObjectManager() { return object_manager; }
    InstanceManager& GetInstanceManager() { return instance_manager; }
    RoomManager& GetRoomManager() { return room_manager; }
    SpriteManager& GetSpriteManager() { return sprite_manager; }
    AudioManager& GetAudioManager() { return audio_manager; }
    
    // Renderer access for drawing
    IRenderer* GetRenderer() { return renderer; }
    void SetRenderer(IRenderer* r) { renderer = r; }

    // Game state
    int GetScore() const { return score; }
    void SetScore(int val) { score = val; }

    int GetLives() const { return lives; }
    void SetLives(int val) { lives = val; }

    double GetGameSpeed() const { return game_speed; }
    void SetGameSpeed(double val) { game_speed = val; }

    int GetFPS() const { return fps; }
    void SetFPS(int val) { fps = val; }

    bool IsGameRunning() const { return running; }
    void SetGameRunning(bool val) { running = val; }

    // Time
    double GetCurrentTime() const { return current_time; }
    void SetCurrentTime(double val) { current_time = val; }

private:
    ObjectManager object_manager;
    InstanceManager instance_manager;
    RoomManager room_manager;
    SpriteManager sprite_manager;
    AudioManager audio_manager;
    IRenderer* renderer = nullptr;

    int score = 0;
    int lives = -1;
    double game_speed = 60.0;
    int fps = 60;
    bool running = false;
    double current_time = 0.0;

    static GameGlobals* instance;
};

} // namespace GM
