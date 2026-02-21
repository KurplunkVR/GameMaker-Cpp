#pragma once

#include "GMLTypes.h"
#include "Managers.h"
#include <nlohmann/json.hpp>
#include <string>
#include <functional>
#include <memory>

namespace GM {

using json = nlohmann::json;

/**
 * Asset Loader for GameMaker runtime
 * 
 * Loads GameMaker games from JSON format (exported from UndertaleModTool or HTML5 target).
 * 
 * For data.win files from native builds:
 *   1. Use UndertaleModTool: https://github.com/UnderminersTeam/UndertaleModTool
 *   2. Convert: UndertaleModTool.exe data.win output.json
 *   3. Load the JSON: loader.LoadGameFromJSON("output.json")
 * 
 * For HTML5 builds:
 *   1. GameMaker exports game.json directly
 *   2. Load it directly: loader.LoadGameFromJSON("game.json")
 */
class TextureManager;
class AssetLoader {
public:
    using ProgressCallback = std::function<void(int current, int total)>;

    AssetLoader();
    ~AssetLoader();

    /**
     * Load a GameMaker game from a JSON file (exported from UMT or HTML5 target)
     * @param json_path Path to game.json
     * @param progress_callback Optional callback for loading progress
     * @return true if successful
     */
    bool LoadGameFromJSON(const std::string& json_path, ProgressCallback progress_callback = nullptr);

    /**
     * NOTE: To load data.win files, use UndertaleModTool to convert to JSON first:
     * https://github.com/UnderminersTeam/UndertaleModTool
     * Then load the converted JSON file with LoadGameFromJSON()
     */

private:
    // Loading phases in dependency order
    bool LoadTextures(const json& game_data);
    bool LoadSprites(const json& game_data);
    bool LoadObjects(const json& game_data);
    bool LoadRooms(const json& game_data);
    bool LoadSounds(const json& game_data);
    bool LoadBackgrounds(const json& game_data);
    bool LoadFonts(const json& game_data);

    // Helper methods
    std::string base_path;
    ProgressCallback progress_callback;
    int loading_count = 0;
    int loading_total = 0;

    void ReportProgress(int current, int total);
};

} // namespace GM
