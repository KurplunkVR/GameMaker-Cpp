#include "GameEngine.h"
#include <cstdio>

namespace GM {

GameEngine::GameEngine(IPlatform* platform)
    : platform(platform) {
}

GameEngine::~GameEngine() {
    Shutdown();
}

bool GameEngine::Initialize(uint32_t display_width, uint32_t display_height) {
    running = true;
    frame_time = 1.0 / 60.0;  // 60 FPS default
    
    // Initialize audio
    globals.GetAudioManager().Initialize();
    
    return true;
}

void GameEngine::Shutdown() {
    running = false;
}

void GameEngine::Tick(double delta_time) {
    if (!running) return;

    frame_accumulator += delta_time * time_scale;
    fps_timer += delta_time;
    
    while (frame_accumulator >= frame_time) {
        Update();
        frame_accumulator -= frame_time;
        frame_count++;
        fps_frame_count++;
    }

    // Update FPS calculation
    if (fps_timer >= 1.0) {
        current_fps = fps_frame_count / fps_timer;
        fps_frame_count = 0;
        fps_timer = 0.0;
    }

    Draw();
}

void GameEngine::Update() {
    // Begin step
    auto room = GetCurrentRoom();
    if (room) {
        for (auto& inst : room->GetInstances()) {
            if (inst->GetActive()) {
                inst->StepEvent(StepEventType::BeginStep);
            }
        }
    }

    // Normal step
    DoStep();

    // End step
    if (room) {
        for (auto& inst : room->GetInstances()) {
            if (inst->GetActive()) {
                inst->StepEvent(StepEventType::EndStep);
            }
        }
    }

    // Update room
    if (room) {
        room->Update();
    }

    // Update audio
    globals.GetAudioManager().Update(frame_time);
}

void GameEngine::Draw() {
    if (!platform) return;

    platform->GetRenderer()->Clear();
    
    auto room = GetCurrentRoom();
    if (room) {
        room->Draw();
    }

    platform->GetRenderer()->Present();
}

void GameEngine::DoStep() {
    auto room = GetCurrentRoom();
    if (!room) return;

    // Update instances
    for (auto& inst : room->GetInstances()) {
        if (inst->GetActive()) {
            inst->Animate();
            inst->Update();
        }
    }
}

void GameEngine::LoadRoom(std::shared_ptr<Room> room) {
    auto current = GetCurrentRoom();
    if (current) {
        RoomEnd(current);
    }
    
    globals.GetRoomManager().SetCurrentRoom(room);
    if (room) {
        RoomStart(room, false);
    }
}

void GameEngine::SwitchRoom(uint32_t room_id, bool starting) {
    auto room = globals.GetRoomManager().GetRoom(room_id);
    if (room) {
        LoadRoom(room);
    }
}

void GameEngine::RoomStart(std::shared_ptr<Room> room, bool starting) {
    if (!room) return;

    room->RoomStartEvent();
    
    // Trigger room start event for all instances
    for (auto& inst : room->GetInstances()) {
        inst->CreateEvent();
    }

    room->Init();
}

void GameEngine::RoomEnd(std::shared_ptr<Room> room) {
    if (!room) return;

    // Trigger room end event for all instances
    for (auto& inst : room->GetInstances()) {
        inst->DestroyEvent();
    }

    room->RoomEndEvent();
}

} // namespace GM
