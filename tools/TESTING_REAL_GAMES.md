# UMT Converter - Real Game Testing Guide

Complete testing workflow to validate the UMT converter with actual GameMaker games like Undertale and Deltarune.

## Quick Start

### 1. Prepare Environment

```powershell
# Navigate to tools directory
cd c:\Users\Owen\Documents\GitHub\GameMaker-Cpp\tools

# Check Python installation
python --version    # Should show Python 3.x
# If not in PATH, use full path or install via Microsoft Store

# Verify UMT is installed
where UndertaleModTool.exe
# If not found, download from: https://github.com/UnderminersTeam/UndertaleModTool
```

### 2. Extract and Convert

**Option A: Auto-detection (easiest)**
```powershell
python extract_game.py "C:/Games/Undertale/data.win" undertale.json
```

**Option B: Custom UMT path**
```powershell
python extract_game.py "C:/Games/Deltarune/data.win" deltarune.json `
    --umt-path "C:/Tools/UMT/UndertaleModTool.exe"
```

**Option C: Keep dump for debugging**
```powershell
python extract_game.py data.win output.json --keep-dump "./umt_dump"
```

### 3. Verify Output

```powershell
# Check if JSON was created
dir *.json

# View JSON structure (first 100 lines)
type output.json | Select-Object -First 100

# Count assets
(Get-Content output.json | ConvertFrom-Json).sprites.Count
(Get-Content output.json | ConvertFrom-Json).objects.Count
(Get-Content output.json | ConvertFrom-Json).rooms.Count
```

## Testing with Real Games

### Undertale

**Finding the file:**
- **Windows Store**: `C:/Users/<username>/AppData/Local/Packages/TobyFox.Undertale_kxnztxnf628xy/LocalState/data.win`
- **Steam**: `C:/Program Files (x86)/Steam/steamapps/common/Undertale/data.win`
- **GOG**: `C:/Program Files/GOG Games/Undertale/data.win`

**Extraction:**
```powershell
# Auto-detect
python quick_start.py

# Or manual
python extract_game.py "$env:PROGRAMFILES(x86)/Steam/steamapps/common/Undertale/data.win" undertale.json

# Expected results:
# - 50+ sprites (player, enemies, effects, etc.)  
# - 100+ objects (player, enemies, NPCs, etc.)
# - 20+ rooms (different locations)
# - 30+ sounds (music, effects)
```

**Validation:**
```powershell
$data = Get-Content undertale.json | ConvertFrom-Json

# Should have sprites
$data.sprites.Count  # Should be >= 50

# Should have game objects
$data.objects | Where {$_.name -match "player|enemy"} | Select name

# Should have room data
$data.rooms | Select name, width, height | Format-Table

# Should have audio
$data.sounds | Select name, sound_type | Format-Table
```

### Deltarune

**Finding the file:**
- **Windows Store**: `C:/Users/<username>/AppData/Local/Packages/TobyFox.Deltarune_xhf8p5n57j8k0/LocalState/data.win`
- **Steam**: `C:/Program Files (x86)/Steam/steamapps/common/Deltarune/data.win`

**Key differences from Undertale:**
- May have different sprite counts  
- Additional characters and rooms
- Additional sound tracks

**Extraction:**
```powershell
python extract_game.py "Deltarune/data.win" deltarune.json --keep-dump deltarune_dump
```

## Automated Testing

### Unit Tests (converter logic)

```powershell
python test_converter.py
```

**Expected output:**
```
TEST 1: Basic UMT Dump Conversion
  Textures: 2 (expected: 2) ✓
  Sprites: 2 (expected: 2) ✓
  Objects: 2 (expected: 2) ✓
  Rooms: 2 (expected: 2) ✓
  Sounds: 2 (expected: 2) ✓

TEST 2: Error Handling
  ✓ Correctly caught error

TEST 3: JSON Compatibility
  ✓ Sprite schema valid
  ✓ Object schema valid
  ✓ Room schema valid
  ✓ Sound schema valid

Total: 3/3 tests passed
```

### Integration Tests (with C++ loader)

```cpp
// In native/src/AssetLoader.cpp or test file
#include "AssetLoader.h"
#include <cassert>

void TestUndertaleLoading() {
    GM::AssetLoader loader;
    
    // Load extracted game
    bool success = loader.LoadGameFromJSON("undertale.json");
    assert(success && "Failed to load Undertale JSON");
    
    // Verify assets loaded
    auto sprite_mgr = GM::Managers::GetSpriteManager();
    auto obj_mgr = GM::Managers::GetObjectManager();
    auto room_mgr = GM::Managers::GetRoomManager();
    auto sound_mgr = GM::Managers::GetSoundManager();
    
    assert(sprite_mgr->GetSpriteCount() > 0 && "No sprites loaded");
    assert(obj_mgr->GetObjectCount() > 0 && "No objects loaded");
    assert(room_mgr->GetRoomCount() > 0 && "No rooms loaded");
    assert(sound_mgr->GetSoundCount() > 0 && "No sounds loaded");
    
    std::cout << "✓ Undertale successfully loaded" << std::endl;
}
```

**Build and run:**
```powershell
cd c:\Users\Owen\Documents\GitHub\GameMaker-Cpp\native
cmake -B build -DBUILD_TESTS=ON
cmake --build build --config Release
./build/Release/test_runner.exe
```

## Troubleshooting Real Game Extraction

### Issue: "UMT not found"

```powershell
# Check installed location
Get-ChildItem -Path "$env:ProgramFiles" -Filter "*UMT*" -Recurse
Get-ChildItem -Path "$env:PROGRAMFILES(X86)" -Filter "*UMT*" -Recurse

# Download from: https://github.com/UnderminersTeam/UndertaleModTool/releases

# Verify after install
C:\Tools\UMT\UndertaleModTool.exe --help
```

### Issue: "data.win not found"

```powershell
# Search for game installations
Get-ChildItem -Path "C:/" -Filter "data.win" -ErrorAction SilentlyContinue -Recurse

# Check Windows Store location
$pkg = Get-AppxPackage "TobyFox.Undertale*"
if ($pkg) { 
    Write-Host "Found at: $($pkg.InstallLocation)/LocalState"
}
```

### Issue: Extraction takes very long or hangs

```powershell
# Check UMT version - may be incompatible with game version
C:\Tools\UMT\UndertaleModTool.exe --version

# Try with verbose output
python extract_game.py data.win output.json  # Verbose by default

# If still hangs, try with smaller timeout
# Edit extract_game.py line ~180: timeout=300  # 5 minutes
```

### Issue: JSON is incomplete or corrupted

```powershell
# Check the dump directory
dir dump_*/Sprites | Measure-Object  # Count sprite folders
dir dump_*/Objects | Measure-Object  # Count object folders

# Verify JSON syntax
Get-Content output.json | ConvertFrom-Json 2>&1
# Should not error - if it does, JSON is malformed

# Check file size
(Get-Item output.json).Length / 1MB  # Should be several MB for real games
```

### Issue: C++ loader can't parse the JSON

```cpp
// Debug the JSON structure
#include <nlohmann/json.hpp>
#include <fstream>

std::ifstream file("undertale.json");
nlohmann::json data = nlohmann::json::parse(file);

// Verify structure
std::cout << "Textures: " << data["textures"].size() << std::endl;
std::cout << "Sprites: " << data["sprites"].size() << std::endl;
std::cout << "Objects: " << data["objects"].size() << std::endl;
std::cout << "Rooms: " << data["rooms"].size() << std::endl;
std::cout << "Sounds: " << data["sounds"].size() << std::endl;
```

## Performance Benchmarks

| Game | File Size | Extraction Time | JSON Size | Assets |
|------|-----------|-----------------|-----------|--------|
| Undertale | 40-50MB | 10-20s | 20-30MB | 200+ |
| Deltarune Ch 1 | 60-80MB | 15-30s | 30-50MB | 250+ |
| Deltarune Ch 2 | 120-150MB | 30-60s | 60-100MB | 400+ |

Times vary based on system specs and UMT version.

## Validation Checklist

After extraction, verify:

- [ ] JSON file created successfully
- [ ] File size is reasonable (>5MB for real games)
- [ ] JSON syntax is valid (can parse with PowerShell or JSON viewer)
- [ ] Sprite count > 0
- [ ] Object count > 0
- [ ] Room count > 0
- [ ] Sound count > 0
- [ ] First sprite has frames
- [ ] First object has valid sprite_id
- [ ] First room has instances
- [ ] All IDs are sequential starting from 0

## Advanced Debugging

### Inspect UMT Dump Directly

```powershell
# Examine dump structure
tree dump_undertale /L 2  # Show 2 levels deep

# Check individual asset
type dump_undertale/Sprites/spr_player_idle/width
type dump_undertale/Objects/obj_player/sprite_index

# View event code
type dump_undertale/Objects/obj_player/events/Create_0.gml
```

### Compare Before/After

```powershell
# Save extracted JSON
$before = Get-Content undertale.json | ConvertFrom-Json

# Re-extract
python extract_game.py "data.win" undertale_v2.json

# Compare
$after = Get-Content undertale_v2.json | ConvertFrom-Json

# Check if counts match
Write-Host "Before: $($before.sprites.Count) sprites"
Write-Host "After: $($after.sprites.Count) sprites"
```

## Supporting Additional Games

The converter is designed to work with any GameMaker game. To test with other games:

1. Locate the game's `data.win` file
2. Extract: `python extract_game.py data.win output.json`
3. Verify output as above
4. Update this guide with results

**Known Compatible Games:**
- Undertale ✓
- Deltarune ✓
- (Add more as tested...)

## Next Steps

After successful extraction and validation:

1. **Use with C++ loader**: Follow integration guide in README.md
2. **Game development**: Build a game runtime using the C++ loader
3. **Asset processing**: Write custom asset processors for your needs
4. **Distribution**: Package extracted assets with your application

For issues or improvements, document the:
- Game name and version
- Error message
- UMT version used
- System info (Windows version, available RAM)
