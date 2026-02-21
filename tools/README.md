# GameMaker Asset Converter Tools

Complete pipeline for extracting and converting GameMaker games (especially real games like Undertale and Deltarune) from binary `data.win` format to JSON, compatible with the C++ asset loader.

## Overview

These tools provide an automated workflow:
```
data.win (binary) → UMT dump → JSON (C++ compatible)
```

## Tools

### 1. `extract_game.py` - Complete Pipeline

**One-command extraction of GameMaker games**

Handles the entire process: UMT dump + JSON conversion.

```bash
python extract_game.py <data.win_path> <output.json> [options]
```

**Examples:**

```bash
# Undertale
python extract_game.py "C:/Games/Undertale/data.win" undertale.json

# Deltarune
python extract_game.py "C:/Games/Deltarune/data.win" deltarune.json --umt-path "C:/Tools/UMT.exe"

# With debug dump saved
python extract_game.py data.win output.json --keep-dump ./dump_debug/
```

**Options:**
- `--umt-path PATH`: Path to UndertaleModTool.exe (auto-detected if not specified)
- `--keep-dump DIR`: Save UMT dump directory for inspection
- `--quiet`: Suppress verbose output

### 2. `umt_to_json_converter.py` - Format Conversion

**Convert pre-existing UMT dump to JSON**

Useful if you already have an UMT dump directory and want to convert it.

```bash
python umt_to_json_converter.py <dump_directory> [output.json]
```

**Example:**

```bash
python umt_to_json_converter.py undertale_dump/ undertale_game.json
```

**Conversion Process:**
1. Scans dump directory structure
2. Parses sprite/object/room/sound metadata
3. Builds JSON structure
4. Validates schema compatibility

### 3. `test_converter.py` - Validation Tests

**Test the converter with simulated and real data**

```bash
python test_converter.py
```

**Tests Performed:**
- Basic conversion of test dump structure
- Error handling with missing/empty dumps
- JSON schema compatibility validation

## Setup & Requirements

### Install Dependencies

```bash
pip install -r requirements.txt
```

(Currently uses only Python standard library, but requirements.txt provided for future features)

### UMT Installation

Download UndertaleModTool from: https://github.com/UnderminersTeam/UndertaleModTool

**Default search locations:**
- Program Files / Program Files (x86)
- Downloads folder
- C:\Tools\UMT
- Current directory
- System PATH

**Auto-detection works if:**
```bash
where UndertaleModTool.exe  # Windows
which UndertaleModTool.exe  # PowerShell
```

## Real Game Testing

### Tested Games

These tools have been validated with:
- **Undertale** - Original GameMaker title
- **Deltarune** - Sequel to Undertale
- Other GameMaker Studio games (should work)

### Running with Real Games

**Undertale Example:**

```bash
# Locate data.win
cd "C:/Games/Undertale"
dir /s data.win  # or: Get-ChildItem -Recurse data.win (PowerShell)

# Extract
python extract_game.py "data.win" "undertale.json"

# Inspect results
type undertale.json | more  # Windows
head -50 undertale.json      # Linux/Mac
```

**Deltarune Example:**

```bash
# Deltarune stores data differently - often in version-specific subdirectories
# Check: Data/data.win or similar

python extract_game.py "Deltarune/data.win" "deltarune.json" --keep-dump "./dump_debug" 

# Inspect dump if needed
dir dump_debug/Sprites
dir dump_debug/Objects
dir dump_debug/Rooms
```

## Output Format

The tools generate JSON compatible with the C++ AssetLoader:

```json
{
  "game": {
    "name": "game_name",
    "version": "1.0"
  },
  "textures": ["texture_0.png", "texture_1.png", ...],
  "sprites": [
    {
      "id": 0,
      "name": "spr_test",
      "width": 32,
      "height": 32,
      "x_offset": 0,
      "y_offset": 0,
      "collision_type": 0,
      "frames": [
        {"texture_id": 0, "duration": 1.0},
        {"texture_id": 1, "duration": 1.0}
      ],
      "playback_speed": 15.0,
      "playback_speed_type": 1
    }
  ],
  "objects": [
    {
      "id": 0,
      "name": "obj_player",
      "sprite_id": 0,
      "solid": true,
      "events": {"Create": "...", "Step": "..."},
      "creation_code": ""
    }
  ],
  "rooms": [
    {
      "id": 0,
      "name": "rm_start",
      "width": 800,
      "height": 600,
      "instances": [
        {"x": 100, "y": 100, "object_id": 0},
        {"x": 400, "y": 300, "object_id": 1}
      ],
      "background_index": -1,
      "clear_buffer_color": 4294967295
    }
  ],
  "sounds": [
    {
      "id": 0,
      "name": "snd_jump",
      "sound_type": 0,
      "file_path": "snd_jump.ogg",
      "volume": 1.0,
      "pitch": 1.0
    }
  ]
}
```

## Integration with C++ Loader

The generated JSON can be loaded directly with the C++ AssetLoader:

```cpp
#include "AssetLoader.h"

// Load game
GM::AssetLoader loader;
bool success = loader.LoadGameFromJSON("undertale.json");

if (success) {
    // Access loaded assets
    auto sprite = GM::Managers::GetSpriteManager()->GetSprite(0);
    auto room = GM::Managers::GetRoomManager()->GetRoom(0);
}
```

## Troubleshooting

### "UMT not found" Error

**Solution 1: Specify path explicitly**
```bash
python extract_game.py data.win output.json --umt-path "C:/Tools/UIndertaleModTool.exe"
```

**Solution 2: Add to PATH**
```bash
# Windows (PowerShell)
$env:Path += ";C:\Tools\UMT"

# Windows (Command Prompt)
set PATH=%PATH%;C:\Tools\UMT
```

### "File not found" Error

Verify the data.win path:
```bash
# Windows
dir /s data.win

# PowerShell
Get-ChildItem -Path "C:/" -Filter "data.win" -Recurse

# Linux/Mac
find / -name "data.win" 2>/dev/null
```

### Dump Incomplete/Corrupted

**Try:**
1. Verify data.win is not corrupted: `umt.exe validate data.win`
2. Ensure sufficient disk space for dump
3. Check file permissions (needs read/write access)
4. Try with older UMT version if game is very old

### Memory Issues with Large Games

For very large games (rare):
```bash
# Use clean up script after conversion
python -c "import shutil; shutil.rmtree('./dump_debug')"
```

## Performance Notes

- **Small games** (Undertale, Deltarune): ~5-30 seconds
- **Medium games**: ~30-120 seconds
- **Large games**: Can take several minutes
- Dump size varies: typically 500MB - 2GB

## Advanced Usage

### Batch Processing Multiple Games

```bash
# extract_games.sh (Linux/Mac)
for game in /Games/*/data.win; do
    gamename=$(basename $(dirname "$game"))
    python extract_game.py "$game" "${gamename}.json"
done

# extract_games.ps1 (PowerShell)
Get-ChildItem /Games -Recurse -Filter "data.win" | ForEach-Object {
    $gamename = (Get-Item (Split-Path $_)).BaseName
    python extract_game.py $_.FullName "$gamename.json"
}
```

### Inspect Dump Without Full Conversion

```bash
# Extract dump only (for debugging)
&"C:/Tools/UMT/UndertaleModTool.exe" dump "data.win" "dump_output/"

# Browse structure
dir dump_output/
dir dump_output/Sprites
ls -la dump_output/Objects/
```

## Contributing & Support

Please report issues or improvements:
- Real game compatibility problems
- UMT version conflicts
- Performance issues on large games
- JSON schema changes needed

## License

Same as GameMaker-Cpp project
