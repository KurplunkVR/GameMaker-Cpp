# GameMaker Extractor

C# utility to extract GameMaker `data.win` binary files to portable JSON format.

## Usage

```bash
dotnet run <path_to_data.win> [output_directory]
```

### Examples

```bash
# Extract Undertale to default 'extracted' folder
dotnet run "C:\Games\Undertale\data.win"

# Extract to specific directory
dotnet run "C:\Games\Undertale\data.win" "C:\output"
```

## Output

Extracts to JSON files:
- `general_info.json` - Game metadata (name, resolution, FPS)
- `sprites.json` - Sprite definitions with frame data and texture coordinates
- `objects.json` - Game object definitions with event code references
- `rooms.json` - Room definitions with instance placements and creation code
- `code.json` - Disassembled GML bytecode
- `sprites/` - Export texture pages as PNG files

## Requirements

- .NET 8.0 or later
- UndertaleModLib (installed via NuGet)

## Building Standalone Executable

```bash
dotnet publish -c Release -r win-x64 --self-contained
```

Output: `bin/Release/net8.0/win-x64/publish/Extractor.exe`

## Architecture

1. **UndertaleIO.Read()** - Parses binary data.win using UndertaleModLib
2. **GameExtractor** - Iterates through all assets and exports to JSON
3. **DataClasses** - Define JSON output structure

## Notes

- Extraction is one-time per game; results are cached
- Large games (Undertale: ~15MB → ~50MB JSON) 
- Texture extraction saves pages as individual PNG files
- Disassembly preview is truncated (first 1000 chars) for performance
