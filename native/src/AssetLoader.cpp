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
    if (game_data.contains("Textures")) loading_total += game_data["Textures"].size();
    if (game_data.contains("Sprites")) loading_total += game_data["Sprites"].size();
    if (game_data.contains("Objects")) loading_total += game_data["Objects"].size();
    if (game_data.contains("GMRooms")) loading_total += game_data["GMRooms"].size();
    if (game_data.contains("Sounds")) loading_total += game_data["Sounds"].size();
    if (game_data.contains("Backgrounds")) loading_total += game_data["Backgrounds"].size();
    if (game_data.contains("Fonts")) loading_total += game_data["Fonts"].size();
    
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
    if (!game_data.contains("Textures")) {
        std::cout << "[AssetLoader] No textures in game data" << std::endl;
        return true;
    }
    
    for (size_t i = 0; i < game_data["Textures"].size(); i++) {
        // TODO: Implement texture loading from PNG/JPG files
        // For now, create placeholder textures
        std::string tex_name = game_data["Textures"][i];
        std::cout << "[AssetLoader] Texture " << i << ": " << tex_name << std::endl;
        
        // Create a placeholder texture (32x32 white)
        auto texture = std::make_shared<Texture>(32, 32);
        // TODO: Load actual image data
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
    }
    
    return true;
}

bool AssetLoader::LoadSprites(const json& game_data) {
    if (!game_data.contains("Sprites")) {
        std::cout << "[AssetLoader] No sprites in game data" << std::endl;
        return true;
    }
    
    auto& sprite_manager = GameGlobals::Get().GetSpriteManager();
    
    for (const auto& sprite_data : game_data["Sprites"]) {
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
                    // TODO: Extract frame data and create actual SpriteFrame objects
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
    if (!game_data.contains("Objects")) {
        std::cout << "[AssetLoader] No objects in game data" << std::endl;
        return true;
    }
    
    auto& object_manager = GameGlobals::Get().GetObjectManager();
    
    for (const auto& object_data : game_data["Objects"]) {
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
    if (!game_data.contains("GMRooms")) {
        std::cout << "[AssetLoader] No rooms in game data" << std::endl;
        return true;
    }
    
    auto& room_manager = GameGlobals::Get().GetRoomManager();
    
    for (const auto& room_data : game_data["GMRooms"]) {
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
                // TODO: Set room size
            }
            if (room_data.contains("height")) {
                // TODO: Set room size
            }
            
            // TODO: Load layers and instances from room data
            
            room_manager.AddRoom(room);
            
            std::cout << "[AssetLoader] Room " << room_id << ": " << room_name << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[AssetLoader] Error loading room: " << e.what() << std::endl;
        }
        
        loading_count++;
        ReportProgress(loading_count, loading_total);
    }
    
    return true;
}

bool AssetLoader::LoadSounds(const json& game_data) {
    if (!game_data.contains("Sounds")) {
        std::cout << "[AssetLoader] No sounds in game data" << std::endl;
        return true;
    }
    
    auto& audio_manager = GameGlobals::Get().GetAudioManager();
    
    for (size_t i = 0; i < game_data["Sounds"].size(); i++) {
        const auto& sound_data = game_data["Sounds"][i];
        
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
    if (!game_data.contains("Backgrounds")) {
        std::cout << "[AssetLoader] No backgrounds in game data" << std::endl;
        return true;
    }
    
    for (size_t i = 0; i < game_data["Backgrounds"].size(); i++) {
        // TODO: Implement background loading
        std::cout << "[AssetLoader] Background " << i << std::endl;
    }
    
    return true;
}

bool AssetLoader::LoadFonts(const json& game_data) {
    if (!game_data.contains("Fonts")) {
        std::cout << "[AssetLoader] No fonts in game data" << std::endl;
        return true;
    }
    
    for (const auto& font_data : game_data["Fonts"]) {
        if (font_data.is_null()) continue;
        
        // TODO: Implement font loading
        std::cout << "[AssetLoader] Font: " << font_data.value("name", "font") << std::endl;
    }
    
    return true;
}

} // namespace GM
