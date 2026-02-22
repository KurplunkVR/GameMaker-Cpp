#include "AssetLoader.h"
#include "Managers.h"
#include "Graphics.h"
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <iostream>

namespace GM {

AssetLoader::AssetLoader() {
}

AssetLoader::~AssetLoader() {
}

void AssetLoader::ReportProgress(int current, int total) {
    if (progress_callback) {
        progress_callback(current, total);
    }
}

bool AssetLoader::LoadGameFromJSON(const std::string& json_path, ProgressCallback callback) {
    progress_callback = callback;
    loading_count = 0;
    loading_total = 0;
    
    std::cout << "[AssetLoader] Loading game from JSON: " << json_path << std::endl;
    
    // Check if file exists
    if (!std::filesystem::exists(json_path)) {
        std::cerr << "[AssetLoader] File not found: " << json_path << std::endl;
        return false;
    }
    
    // Extract base path for relative asset paths
    base_path = std::filesystem::path(json_path).parent_path().string();
    
    // Load JSON file
    std::ifstream file(json_path);
    json game_data;
    
    try {
        file >> game_data;
    } catch (const std::exception& e) {
        std::cerr << "[AssetLoader] Failed to parse JSON: " << e.what() << std::endl;
        return false;
    }
    
    // Count total assets for progress tracking
    if (game_data.contains("textures")) loading_total += game_data["textures"].size();
    if (game_data.contains("sprites")) loading_total += game_data["sprites"].size();
    if (game_data.contains("objects")) loading_total += game_data["objects"].size();
    if (game_data.contains("rooms")) loading_total += game_data["rooms"].size();
    if (game_data.contains("sounds")) loading_total += game_data["sounds"].size();
    if (game_data.contains("backgrounds")) loading_total += game_data["backgrounds"].size();
    if (game_data.contains("fonts")) loading_total += game_data["fonts"].size();
    
    ReportProgress(0, loading_total);
    
    // Load in dependency order
    std::cout << "[AssetLoader] Loading textures..." << std::endl;
    if (!LoadTextures(game_data)) {
        std::cerr << "[AssetLoader] Failed to load textures" << std::endl;
        return false;
    }
    
    std::cout << "[AssetLoader] Loading sprites..." << std::endl;
    if (!LoadSprites(game_data)) {
        std::cerr << "[AssetLoader] Failed to load sprites" << std::endl;
        return false;
    }
    
    std::cout << "[AssetLoader] Loading objects..." << std::endl;
    if (!LoadObjects(game_data)) {
        std::cerr << "[AssetLoader] Failed to load objects" << std::endl;
        return false;
    }
    
    std::cout << "[AssetLoader] Loading rooms..." << std::endl;
    if (!LoadRooms(game_data)) {
        std::cerr << "[AssetLoader] Failed to load rooms" << std::endl;
        return false;
    }
    
    std::cout << "[AssetLoader] Loading sounds..." << std::endl;
    if (!LoadSounds(game_data)) {
        std::cerr << "[AssetLoader] Failed to load sounds" << std::endl;
        return false;
    }
    
    std::cout << "[AssetLoader] Loading backgrounds..." << std::endl;
    if (!LoadBackgrounds(game_data)) {
        std::cerr << "[AssetLoader] Failed to load backgrounds" << std::endl;
        return false;
    }
    
    std::cout << "[AssetLoader] Loading fonts..." << std::endl;
    if (!LoadFonts(game_data)) {
        std::cerr << "[AssetLoader] Failed to load fonts" << std::endl;
        return false;
    }
    
    ReportProgress(loading_total, loading_total);
    
    // Set first room as current
    if (game_data.contains("RoomOrder") && !game_data["RoomOrder"].empty()) {
        uint32_t first_room_id = game_data["RoomOrder"][0];
        auto& room_manager = GameGlobals::Get().GetRoomManager();
        auto first_room = room_manager.GetRoom(first_room_id);
        if (first_room) {
            room_manager.SetCurrentRoom(first_room);
            std::cout << "[AssetLoader] Set starting room: " << first_room_id << std::endl;
        }
    }
    
    std::cout << "[AssetLoader] Game loaded successfully!" << std::endl;
    return true;
}

bool AssetLoader::LoadTextures(const json& game_data) {
    if (!game_data.contains("textures")) {
        std::cout << "[AssetLoader] No textures in game data" << std::endl;
        return true;
    }

    for (size_t i = 0; i < game_data["textures"].size(); i++) {
        std::string tex_name = game_data["textures"][i];
        
        // Create placeholder texture (256x256 for now)
        auto texture = std::make_shared<Texture>(256, 256);
        // Fill with test pattern
        uint8_t color = (i * 10) % 256;
        for (uint32_t y = 0; y < texture->GetHeight(); y++) {
            for (uint32_t x = 0; x < texture->GetWidth(); x++) {
                uint8_t* pixel = texture->GetPixelData() + ((y * texture->GetWidth() + x) * 4);
                pixel[0] = color;        // R
                pixel[1] = (color + 50) % 256;  // G
                pixel[2] = (color + 100) % 256; // B
                pixel[3] = 255;          // A
            }
        }
        
        textures[tex_name] = texture;
        std::cout << "[AssetLoader] Loaded texture " << i << ": " << tex_name << std::endl;
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
    }
    
    return true;
}

bool AssetLoader::LoadSprites(const json& game_data) {
    if (!game_data.contains("sprites") || game_data["sprites"].empty()) {
        std::cout << "[AssetLoader] No sprites in game data" << std::endl;
        return true;
    }
    
    auto& sprite_manager = GameGlobals::Get().GetSpriteManager();
    
    for (const auto& sprite_data : game_data["sprites"]) {
        if (sprite_data.is_null()) {
            loading_count++;
            ReportProgress(loading_count, loading_total);
            continue;
        }
        
        try {
            uint32_t sprite_id = sprite_data["id"];
            std::string sprite_name = sprite_data.value("name", "sprite_" + std::to_string(sprite_id));
            
            auto sprite = std::make_shared<Sprite>(sprite_id, sprite_name);
            
            // Load frames if present
            if (sprite_data.contains("frames")) {
                for (const auto& frame_data : sprite_data["frames"]) {
                    SpriteFrame frame;
                    
                    // Try to get texture by index
                    if (frame_data.contains("texture_id")) {
                        int tex_id = frame_data["texture_id"];
                        if (tex_id >= 0 && tex_id < (int)game_data["textures"].size()) {
                            std::string tex_name = game_data["textures"][tex_id];
                            if (textures.find(tex_name) != textures.end()) {
                                frame.SetTexture(textures[tex_name]);
                                auto tex = textures[tex_name];
                                frame.SetWidth(tex->GetWidth());
                                frame.SetHeight(tex->GetHeight());
                            }
                        }
                    }
                    
                    if (frame_data.contains("duration")) {
                        frame.SetDuration(frame_data["duration"]);
                    }
                    
                    sprite->AddFrame(frame);
                }
            }
            
            // Set origin/hotspot
            if (sprite_data.contains("xorigin")) {
                sprite->SetXOrigin(sprite_data["xorigin"]);
            }
            if (sprite_data.contains("yorigin")) {
                sprite->SetYOrigin(sprite_data["yorigin"]);
            }
            
            sprite_manager.AddSprite(sprite);
            
            std::cout << "[AssetLoader] Sprite " << sprite_id << ": " << sprite_name << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[AssetLoader] Error loading sprite: " << e.what() << std::endl;
        }
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
    }
    
    return true;
}

bool AssetLoader::LoadObjects(const json& game_data) {
    if (!game_data.contains("objects")) {
        std::cout << "[AssetLoader] No objects in game data" << std::endl;
        return true;
    }
    
    auto& object_manager = GameGlobals::Get().GetObjectManager();
    
    for (const auto& object_data : game_data["objects"]) {
        if (object_data.is_null()) {
            loading_count++;
            ReportProgress(loading_count, loading_total);
            continue;
        }
        
        try {
            uint32_t object_id = object_data["id"];
            std::string object_name = object_data.value("name", "obj_" + std::to_string(object_id));
            
            auto object = std::make_shared<Object>(object_id, object_name);
            
            // Set sprite
            if (object_data.contains("sprite_index")) {
                object->SetSpriteIndex(object_data["sprite_index"]);
            }
            
            // Set parent
            if (object_data.contains("parent_index")) {
                uint32_t parent_id = object_data["parent_index"];
                if (parent_id != 0) {
                    auto parent = object_manager.GetObject(parent_id);
                    if (parent) {
                        object->SetParent(parent);
                    }
                }
            }
            
            // Set flags
            if (object_data.contains("visible")) {
                // TODO: Set visible flag
            }
            if (object_data.contains("solid")) {
                // TODO: Set solid flag
            }
            
            object_manager.AddObject(object);
            
            std::cout << "[AssetLoader] Object " << object_id << ": " << object_name << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[AssetLoader] Error loading object: " << e.what() << std::endl;
        }
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
    }
    
    return true;
}

bool AssetLoader::LoadRooms(const json& game_data) {
    auto& room_manager = GameGlobals::Get().GetRoomManager();
    auto& object_manager = GameGlobals::Get().GetObjectManager();
    
    std::cout << "[AssetLoader] Checking for rooms... contains: " << game_data.contains("rooms") << std::endl;
    if (game_data.contains("rooms")) {
        std::cout << "[AssetLoader] Rooms array size: " << game_data["rooms"].size() << std::endl;
    }
    
    if (!game_data.contains("rooms") || game_data["rooms"].empty()) {
        std::cout << "[AssetLoader] No rooms in game data, creating a default room" << std::endl;
        
        // Create a default room with texture display
        auto room = std::make_shared<Room>(0, "DefaultRoom");
        room->SetWidth(800);
        room->SetHeight(600);
        room->SetBackgroundColor(0xFF1a1a2e);
        room_manager.AddRoom(room);
        
        auto camera = std::make_shared<Camera>();
        camera->SetID(0);
        camera->SetX(0);
        camera->SetY(0);
        camera->SetWidth(800);
        camera->SetHeight(600);
        camera->SetViewportWidth(800);
        camera->SetViewportHeight(600);
        room->AddCamera(camera);
        room->SetActiveCamera(camera);
        
        auto layer = std::make_shared<Layer>(0, "Textures", LayerType::Instances);
        room->AddLayer(layer);
        
        auto test_obj = std::make_shared<Object>(9999, "test_display");
        object_manager.AddObject(test_obj);
        
        int x = 20, y = 20;
        uint32_t inst_id = 0;
        for (auto& [tex_name, texture] : textures) {
            auto inst = std::make_shared<Instance>(x, y, inst_id++, test_obj);
            inst->SetSpeed(0);
            inst->SetVSpeed(0);
            inst->SetVisible(true);
            room->AddInstance(inst);
            
            x += texture->GetWidth() + 10;
            if (x > 700) {
                x = 20;
                y += texture->GetHeight() + 10;
            }
        }
        
        std::cout << "[AssetLoader] Created " << inst_id << " instances for texture display" << std::endl;
        room_manager.SetCurrentRoom(room);
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
        return true;
    }
    
    // Load actual rooms from JSON
    for (const auto& room_data : game_data["rooms"]) {
        if (room_data.is_null()) {
            loading_count++;
            ReportProgress(loading_count, loading_total);
            continue;
        }
        
        try {
            uint32_t room_id = room_data["id"];
            std::string room_name = room_data.value("name", "rm_" + std::to_string(room_id));
            
            auto room = std::make_shared<Room>(room_id, room_name);
            
            // Set room properties
            if (room_data.contains("width")) {
                room->SetWidth(room_data["width"]);
            }
            if (room_data.contains("height")) {
                room->SetHeight(room_data["height"]);
            }
            if (room_data.contains("background_color")) {
                std::string color_str = room_data["background_color"];
                // Parse hex color
                uint32_t color = std::stoul(color_str, nullptr, 16);
                room->SetBackgroundColor(color);
            }
            
            // Create camera for room
            auto camera = std::make_shared<Camera>();
            camera->SetID(0);
            camera->SetX(0);
            camera->SetY(0);
            camera->SetWidth(room->GetWidth());
            camera->SetHeight(room->GetHeight());
            camera->SetViewportWidth(800);
            camera->SetViewportHeight(600);
            room->AddCamera(camera);
            room->SetActiveCamera(camera);
            
            // Create instance layer
            auto layer = std::make_shared<Layer>(0, "Instances", LayerType::Instances);
            room->AddLayer(layer);
            
            // Load instances
            if (room_data.contains("instances")) {
                for (const auto& inst_data : room_data["instances"]) {
                    if (inst_data.is_null()) continue;
                    
                    try {
                        uint32_t inst_id = inst_data["id"];
                        uint32_t obj_id = inst_data["object_id"];
                        double inst_x = inst_data.value("x", 0.0);
                        double inst_y = inst_data.value("y", 0.0);
                        
                        auto obj = object_manager.GetObject(obj_id);
                        if (!obj) {
                            // Create object if it doesn't exist
                            obj = std::make_shared<Object>(obj_id, "obj_" + std::to_string(obj_id));
                            object_manager.AddObject(obj);
                        }
                        
                        auto inst = std::make_shared<Instance>(inst_x, inst_y, inst_id, obj);
                        inst->SetVisible(true);
                        room->AddInstance(inst);
                        
                        std::cout << "[AssetLoader] Room " << room_id << " Instance " << inst_id << ": " << obj->GetName() << " at (" << inst_x << ", " << inst_y << ")" << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "[AssetLoader] Error loading instance: " << e.what() << std::endl;
                    }
                }
            }
            
            room_manager.AddRoom(room);
            
            // Set first room as current
            if (room_id == 0) {
                room_manager.SetCurrentRoom(room);
            }
            
            std::cout << "[AssetLoader] Room " << room_id << ": " << room_name << " (" << room->GetWidth() << "x" << room->GetHeight() << ")" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[AssetLoader] Error loading room: " << e.what() << std::endl;
        }
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
    }
    
    return true;
}

bool AssetLoader::LoadSounds(const json& game_data) {
    if (!game_data.contains("sounds")) {
        std::cout << "[AssetLoader] No sounds in game data" << std::endl;
        return true;
    }
    
    auto& audio_manager = GameGlobals::Get().GetAudioManager();
    
    for (size_t i = 0; i < game_data["sounds"].size(); i++) {
        const auto& sound_data = game_data["sounds"][i];
        
        if (sound_data.is_null()) {
            loading_count++;
            ReportProgress(loading_count, loading_total);
            continue;
        }
        
        try {
            uint32_t sound_id = sound_data["id"];
            std::string sound_name = sound_data.value("name", "snd_" + std::to_string(sound_id));
            
            // TODO: Create sound asset and add to manager
            std::cout << "[AssetLoader] Sound " << sound_id << ": " << sound_name << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[AssetLoader] Error loading sound: " << e.what() << std::endl;
        }
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
    }
    
    return true;
}

bool AssetLoader::LoadBackgrounds(const json& game_data) {
    if (!game_data.contains("backgrounds")) {
        std::cout << "[AssetLoader] No backgrounds in game data" << std::endl;
        return true;
    }
    
    for (size_t i = 0; i < game_data["backgrounds"].size(); i++) {
        // TODO: Implement background loading
        std::cout << "[AssetLoader] Background " << i << std::endl;
    }
    
    return true;
}

bool AssetLoader::LoadFonts(const json& game_data) {
    if (!game_data.contains("fonts")) {
        std::cout << "[AssetLoader] No fonts in game data" << std::endl;
        return true;
    }
    
    for (const auto& font_data : game_data["fonts"]) {
        if (font_data.is_null()) continue;
        
        // TODO: Implement font loading
        std::cout << "[AssetLoader] Font: " << font_data.value("name", "font") << std::endl;
    }
    
    return true;
}

} // namespace GM
