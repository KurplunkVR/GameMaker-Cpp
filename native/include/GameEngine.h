#pragma once

#include "GMLTypes.h"
#include "Managers.h"
#include "IPlatform.h"
#include <memory>

namespace GM {

// Main game engine
class GameEngine {
public:
    GameEngine(IPlatform* platform);
    ~GameEngine();

    // Initialization
    bool Initialize(uint32_t display_width, uint32_t display_height);
    void Shutdown();

    // Game loop
    void Tick(double delta_time);
    void Update();
    void Draw();

    // Room management
    void LoadRoom(std::shared_ptr<Room> room);
    void SwitchRoom(uint32_t room_id, bool starting = false);

    // State
    bool IsRunning() const { return running; }
    void Stop() { running = false; }

    GameGlobals& GetGlobals() { return globals; }
    IPlatform* GetPlatform() { return platform; }

    // Current room
    std::shared_ptr<Room> GetCurrentRoom() { return globals.GetRoomManager().GetCurrentRoom(); }

    // Statistics/Debug
    int GetFrameCount() const { return frame_count; }
    double GetFPS() const { return current_fps; }
    double GetDeltaTime() const { return frame_time; }
    void SetTargetFPS(double fps) { frame_time = 1.0 / fps; }

    // Time scaling
    double GetTimeScale() const { return time_scale; }
    void SetTimeScale(double scale) { time_scale = scale; }

private:
    void DoStep();
    void RoomStart(std::shared_ptr<Room> room, bool starting);
    void RoomEnd(std::shared_ptr<Room> room);

    IPlatform* platform;
    GameGlobals globals;
    bool running = false;
    double frame_time = 1.0 / 60.0;  // 60 FPS default
    int frame_count = 0;
    double current_fps = 60.0;

    // For frame timing
    double time_scale = 1.0;
    double frame_accumulator = 0.0;
    double fps_timer = 0.0;
    int fps_frame_count = 0;
};

} // namespace GM
