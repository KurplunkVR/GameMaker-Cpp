# OpenGM Data Loading Pipeline - Detailed Analysis

## Overview

OpenGM is an open-source .NET GameMaker engine emulator that successfully runs Undertale, Deltarune, and other GameMaker titles. It uses a **two-stage conversion pipeline** to load and execute GameMaker games.

---

## Two-Stage Pipeline Architecture

### Stage 1: Initial Conversion (One-time per game)
```
data.win 
  ↓ (UndertaleIO.Read using UndertaleModLib)
UndertaleData object (in-memory representation of entire game)
  ↓ (GameConverter.ConvertGame)
data_OpenGM.win (custom binary format, optimized for runtime loading)
```

### Stage 2: Runtime Loading (Every execution)
```
data_OpenGM.win
  ↓ (Binary deserialization)
In-memory game structures (sprites, rooms, code, objects, etc.)
  ↓ (VM Execution)
Running game instances
```

---

## Stage 1: Binary Parsing with UndertaleModLib

### Entry Point: Entry.cs - LoadGame()

```csharp
public static void LoadGame(string dataWinPath, string[] parameters)
{
    // Only converts once - subsequent runs load cached data_OpenGM.win
    if (!File.Exists(Path.Combine(DataWinFolder, "data_OpenGM.win")))
    {
        if (!File.Exists(dataWinPath))
        {
            DebugLog.LogError("ERROR - data.win not found");
            return;
        }

        Console.WriteLine("Extracting game assets...");
        using var stream = new FileStream(dataWinPath, FileMode.Open, FileAccess.Read);
        using var data = UndertaleIO.Read(stream);    // ← UndertaleModLib parses binary
        GameConverter.ConvertGame(data);               // ← Converts to custom format
    }

    AudioManager.Init();
    GameLoader.LoadGame();      // ← Loads data_OpenGM.win for runtime
    VMExecutor.InitRandom(0);
    
    // Calculate next instance ID based on game data
    InstanceManager.NextInstanceID = GameLoader.GeneralInfo.LastObjectID + 1;
    
    // Create window and start main loop
    window = new CustomWindow(gameSettings, nativeSettings);
    window.Run();
}
```

### Key Libraries

- **UndertaleModLib** (NuGet package)
  - Handles all binary `data.win` parsing
  - Returns `UndertaleData` object with complete game state
  - Provides disassemblers for GML bytecode → text assembly
  - Used for extracting sprites, rooms, objects, code, etc.

- **MemoryPack** 
  - Fast binary serialization for .NET
  - Used to serialize converted data to `data_OpenGM.win`

---

## Stage 2: GameConverter - Asset Extraction

### Conversion Order (CRITICAL: Must match GameLoader order)

```csharp
public static void ConvertGame(UndertaleData data)
{
    using var stream = File.OpenWrite(Path.Combine(Entry.DataWinFolder, "data_OpenGM.win"));
    using var writer = new BinaryWriter(stream);

    try
    {
        // Must match order of gameloader execution
        ExportGeneralInfo(writer, data);          // Game metadata
        ExportAssetOrder(writer, data);           // Asset index mappings
        ConvertScripts(writer, data);             // Script declarations
        ConvertCode(writer, data, data.Code);     // GML bytecode → VM instructions
        ExportExtensions(writer, data);           // DLL extensions
        ExportGlobalInitCode(writer, data);       // Global init code references
        ExportObjectDefinitions(writer, data);    // Object definitions + events
        ExportBackgrounds(writer, data);          // Background assets
        ExportRooms(writer, data);                // Room data + instances
        ConvertSprites(writer, data);             // Sprite frame data
        ExportFonts(writer, data);                // Font metrics
        ExportPages(writer, data);                // Texture pages (PNG images)
        ExportTextureGroups(writer, data);        // Texture group metadata
        ExportTileSets(writer, data);             // Tileset definitions
        ExportSounds(writer, data);               // Audio files
        ExportPaths(writer, data);                // Path curves
        ExportShaders(writer, data);              // GLSL shader code
        ExportAnimCurves(writer, data);           // Animation curves
    }
    catch
    {
        writer.Close();
        File.Delete(Path.Combine(Entry.DataWinFolder, "data_OpenGM.win"));
        throw;
    }

    GC.Collect(); // Memory cleanup after heavy processing
}
```

---

## Critical: GML Bytecode Conversion

### The Process

GameMaker compiles GML scripts to bytecode (binary format). OpenGM needs to convert this to executable VM instructions.

```csharp
public static void ConvertCode(BinaryWriter writer, UndertaleData data, IList<UndertaleCode> codes)
{
    Console.Write($"Converting code...");
    writer.Write(codes.Count);
    
    foreach (var code in codes)
    {
        // Step 1: Get local variable metadata
        UndertaleCodeLocals? codeLocals = null;
        if (data.CodeLocals != null)
        {
            codeLocals = data.CodeLocals.For(code);
        }

        // Step 2: Disassemble binary bytecode to text assembly
        var asmFile = code.Disassemble(data.Variables, codeLocals);
        
        // Step 3: Parse assembly into custom VM instruction format
        var asset = ConvertAssembly(asmFile);

        // Step 4: Attach metadata
        asset.AssetId = codes.IndexOf(code);
        asset.Name = code.Name.Content;
        asset.ParentAssetId = codes.IndexOf(code.ParentEntry) ?? -1;

        // Step 5: Serialize to binary
        writer.WriteMemoryPack(asset);
    }

    Console.WriteLine($" Done!");
}
```

### Assembly Parsing (ConvertAssembly)

Takes disassembled GML bytecode and converts to `VMCode` objects:

```csharp
public static VMCode ConvertAssembly(string asmFile)
{
    var asset = new VMCode();
    var lines = asmFile.Split('\n');

    foreach (var line in lines)
    {
        if (line.Length == 0) continue;

        var instruction = new VMCodeInstruction();
        var opcode = line.Split(' ')[0];

        // Parse each opcode and extract operands
        switch (opcode)
        {
            case VMOpcode.PUSH:
            case VMOpcode.PUSHBLTN:
            case VMOpcode.PUSHI:
                var value = line.Substring(opcode.Length + 1);
                switch (instruction.TypeOne)
                {
                    case VMType.s:
                        // String data - unescape escape sequences
                        var stringData = removedQuotes
                            .Replace(@"\n", "\n")
                            .Replace(@"\\", @"\")
                            .Replace("\\\"", "\"");
                        instruction.StringData = stringData;
                        break;
                    case VMType.i:
                        // Integer or variable reference
                        if (int.TryParse(value, out var intResult))
                        {
                            instruction.IntData = intResult;
                        }
                        else
                        {
                            instruction.StringData = value;
                            if (value.StartsWith("[function]"))
                            {
                                instruction.PushFunction = true;
                                instruction.StringData = value[10..];
                            }
                        }
                        break;
                    case VMType.d:
                        instruction.DoubleData = double.Parse(value);
                        break;
                    case VMType.v:
                        // Variable reference - needs lookup
                        instruction.StringData = value;
                        shouldGetVariableInfo = true;
                        break;
                }
                break;

            case VMOpcode.CALL:
                // Function call - extract function name and argument count
                var function = line.Substring(opcode.Length + 1);
                var argcIndex = function.IndexOf("argc=");
                var argumentCount = int.Parse(function[(argcIndex + 5)..^1]);
                var functionName = function.Substring(0, function.IndexOf('('));
                instruction.FunctionArgumentCount = argumentCount;
                instruction.FunctionName = functionName;
                break;

            case VMOpcode.CALLV:
                // Variable function call
                instruction.IntData = int.Parse(line.Substring(opcode.Length + 1));
                break;

            // ... more opcodes (JMP, POP, RETURN, BREAK, etc.)
        }

        // Resolve variable context (global, local, instance, etc.)
        if (shouldGetVariableInfo)
        {
            GetVariableInfo(instruction, out var variableName, out var variableType, 
                          out var variablePrefix, out var assetId);
            instruction.variableName = variableName;
            instruction.variableType = variableType;
            instruction.variablePrefix = variablePrefix;
            instruction.assetId = assetId;
        }

        asset.Instructions.Add(instruction);
    }

    return asset;
}
```

### Supported Opcodes

- `PUSH*` - Push value onto stack (integer, string, double, etc.)
- `POP*` - Pop from stack into variable
- `CALL` - Call function or script
- `CALLV` - Call function by variable
- `JMP` - Unconditional jump
- `BT`, `BF` - Branch if true/false
- `RETURN` - Return from function
- `CONV` - Type conversion
- `ADD`, `SUB`, `MUL`, `DIV` - Arithmetic
- `AND`, `OR`, `XOR` - Bitwise operations
- `EQ`, `LT`, `GT` - Comparisons
- Many others...

---

## Room Export Detail

### Data Structure

```csharp
public static void ExportRooms(BinaryWriter writer, UndertaleData data)
{
    Console.Write($"Exporting rooms...");

    writer.Write(data.Rooms.Count);
    foreach (var room in data.Rooms)
    {
        var asset = new CRoom();
        asset.Name = room.Name.Content;
        asset.Width = room.Width;
        asset.Height = room.Height;
        asset.Speed = room.Speed;
        asset.SpeedType = room.SpeedType;
        asset.DefaultViewSettings = room.DefaultGameObjectView;
        asset.ClearDisplayBuffer = room.ClearDisplayBuffer;
        
        int CurrentElementID = 0;

        // Export each layer
        foreach (var layer in room.Layers)
        {
            var layerasset = new CLayer();
            layerasset.Name = layer.LayerName.Content;
            layerasset.LayerType = layer.LayerType;
            layerasset.Depth = layer.Depth;
            layerasset.Visible = layer.Visible;
            layerasset.Offset = new Vector2i(layer.OffsetX, layer.OffsetY);

            if (layer.LayerType == UndertaleRoom.LayerType.Instances)
            {
                // Instance layer - game objects placed in the room
                var instancesData = layer.InstancesData;

                foreach (var instance in instancesData)
                {
                    var objectAsset = new CLayerObjectElement()
                    {
                        Type = ElementType.Instance,
                        Id = CurrentElementID++,
                        X = instance.X,
                        Y = instance.Y,
                        Name = instance.CreationCode?.Name.Content ?? "",
                        DefinitionID = data.GameObjects.IndexOf(instance.ObjectDefinition),
                        InstanceID = (int)instance.InstanceID,
                        CreationCodeID = codes.IndexOf(instance.CreationCode),
                        ScaleX = instance.ScaleX,
                        ScaleY = instance.ScaleY,
                        Color = (int)instance.Color,
                        Rotation = instance.Rotation,
                        FrameIndex = instance.ImageIndex,
                        ImageSpeed = instance.ImageSpeed,
                        PreCreateCodeID = codes.IndexOf(instance.PreCreateCode),
                    };

                    layerasset.Elements.Add(objectAsset);
                }
            }
            else if (layer.LayerType == UndertaleRoom.LayerType.Background)
            {
                // Background layer
                var backgroundElement = new CLayerBackgroundElement()
                {
                    Type = ElementType.Background,
                    Id = CurrentElementID++,
                    Name = layer.LayerName.Content,
                    Visible = layer.BackgroundData.Visible,
                    Foreground = layer.BackgroundData.Foreground,
                    Index = data.Sprites.IndexOf(layer.BackgroundData.Sprite),
                    HTiled = layer.BackgroundData.TiledHorizontally,
                    VTiled = layer.BackgroundData.TiledVertically,
                    XScale = layer.BackgroundData.CalcScaleX,
                    YScale = layer.BackgroundData.CalcScaleY,
                    Stretch = layer.BackgroundData.Stretch,
                    Color = layer.BackgroundData.Color,
                    Alpha = layer.BackgroundData.Color.ABGRToCol4().A,
                    FirstFrame = (int)layer.BackgroundData.FirstFrame,
                    AnimationSpeed = layer.BackgroundData.AnimationSpeed,
                    AnimationSpeedType = layer.BackgroundData.AnimationSpeedType
                };

                layerasset.Elements.Add(backgroundElement);
            }
            else if (layer.LayerType == UndertaleRoom.LayerType.Assets)
            {
                // Asset layer - tiles, sprites, particle systems, etc.
                var assetsData = layer.AssetsData;

                // Tiles
                if (assetsData.LegacyTiles != null)
                {
                    foreach (var tile in assetsData.LegacyTiles)
                    {
                        var val = new CLayerTileElement();
                        val.Type = ElementType.Tile;
                        val.Id = CurrentElementID++;
                        val.X = tile.X;
                        val.Y = tile.Y;
                        val.Definition = data.Sprites.IndexOf(tile.SpriteDefinition);
                        val.SourceLeft = (int)tile.SourceX;
                        val.SourceTop = (int)tile.SourceY;
                        val.SourceWidth = (int)tile.Width;
                        val.SourceHeight = (int)tile.Height;
                        val.ScaleX = tile.ScaleX;
                        val.ScaleY = tile.ScaleY;
                        val.Color = tile.Color;

                        layerasset.Elements.Add(val);
                    }
                }

                // Sprites in asset layer
                if (assetsData.Sprites != null)
                {
                    foreach (var item in assetsData.Sprites)
                    {
                        var val = new CLayerSpriteElement();
                        val.Type = ElementType.Sprite;
                        val.Id = CurrentElementID++;
                        val.Name = item.Name.Content;
                        val.Definition = data.Sprites.IndexOf(item.Sprite);
                        val.X = item.X;
                        val.Y = item.Y;
                        val.ScaleX = item.ScaleX;
                        val.ScaleY = item.ScaleY;
                        val.Color = item.Color;
                        val.AnimationSpeed = item.AnimationSpeed;
                        val.AnimationSpeedType = item.AnimationSpeedType;
                        val.FrameIndex = item.FrameIndex;
                        val.Rotation = item.Rotation;

                        layerasset.Elements.Add(val);
                    }
                }
            }

            asset.Layers.Add(layerasset);
        }

        // Legacy room instances (pre-layer format)
        foreach (var instance in room.GameObjects)
        {
            var objectAsset = new GameObject
            {
                Type = ElementType.Instance,
                Id = (int)instance.InstanceID,
                X = instance.X,
                Y = instance.Y,
                DefinitionID = data.GameObjects.IndexOf(instance.ObjectDefinition),
                InstanceID = (int)instance.InstanceID,
                CreationCodeID = codes.IndexOf(instance.CreationCode),
                ScaleX = instance.ScaleX,
                ScaleY = instance.ScaleY,
                Color = (int)instance.Color,
                Rotation = instance.Rotation,
                FrameIndex = instance.ImageIndex,
                ImageSpeed = instance.ImageSpeed,
                PreCreateCodeID = codes.IndexOf(instance.PreCreateCode),
            };

            asset.GameObjects.Add(objectAsset);
        }

        // Tile data
        foreach (var tile in room.Tiles)
        {
            var definition = tile.spriteMode
                ? data.Sprites.IndexOf(tile.SpriteDefinition)
                : data.Backgrounds.IndexOf(tile.BackgroundDefinition);

            var tileAsset = new Tile()
            {
                X = tile.X,
                Y = tile.Y,
                Definition = definition,
                SpriteMode = tile.spriteMode,
                SourceLeft = (int)tile.SourceX,
                SourceTop = (int)tile.SourceY,
                SourceHeight = (int)tile.Height,
                SourceWidth = (int)tile.Width,
                Depth = tile.TileDepth,
                InstanceID = (int)tile.InstanceID,
                ScaleX = tile.ScaleX,
                ScaleY = tile.ScaleY,
                Color = tile.Color
            };

            asset.Tiles.Add(tileAsset);
        }

        writer.WriteMemoryPack(asset);
    }

    Console.WriteLine($" Done!");
}
```

### Instance Data Includes
- Position: X, Y coordinates in room
- Scale: ScaleX, ScaleY for scaling from base sprite size
- Rotation: Image angle
- Color: RGBA tint applied to sprite
- Object Reference: Links to object definition (for event handlers)
- Creation Code: Optional GML executed when instance spawns
- Pre-Create Code: GML executed before creation code
- Image Index: Which sprite frame to display
- Image Speed: Animation speed

---

## Sprite Export Detail

```csharp
public static void ConvertSprites(BinaryWriter writer, UndertaleData data, IList<UndertaleSprite> sprites)
{
    Console.Write($"Converting sprites...");

    writer.Write(sprites.Count);
    foreach (var sprite in sprites)
    {
        var asset = new Sprite();
        asset.Name = sprite.Name.Content;
        asset.Width = sprite.Width;
        asset.Height = sprite.Height;
        asset.MarginLeft = sprite.MarginLeft;
        asset.MarginTop = sprite.MarginTop;
        asset.MarginRight = sprite.MarginRight;
        asset.MarginBottom = sprite.MarginBottom;
        asset.OriginX = sprite.OriginX;
        asset.OriginY = sprite.OriginY;
        asset.PlaybackSpeed = sprite.PlaybackSpeed;
        asset.PlaybackSpeedType = sprite.PlaybackSpeedType;
        asset.Transparent = sprite.Transparent;

        // Export each frame with texture coordinates
        foreach (var frame in sprite.Frames)
        {
            var textureRef = frame.Texture;
            
            // Store texture page + region within that page
            var pageItem = new SpritePageItem
            {
                SourceX = textureRef.SourceX,      // Pixel X in texture page
                SourceY = textureRef.SourceY,      // Pixel Y in texture page
                SourceWidth = textureRef.SourceWidth,    // Frame width
                SourceHeight = textureRef.SourceHeight,  // Frame height
                TargetX = textureRef.TargetX,      // Offset X when drawn
                TargetY = textureRef.TargetY,      // Offset Y when drawn
                TargetWidth = textureRef.TargetWidth,    // Scale width
                TargetHeight = textureRef.TargetHeight,  // Scale height
                BoundingWidth = textureRef.BoundingWidth,
                BoundingHeight = textureRef.BoundingHeight,
                Page = textureRef.TexturePage.Name.Content  // Which texture page
            };

            asset.Textures.Add(pageItem);
        }

        // Collision masks (per frame)
        foreach (var item in sprite.CollisionMasks)
        {
            asset.CollisionMasks.Add(item.Data);
        }

        writer.WriteMemoryPack(asset);
    }

    Console.WriteLine($" Done!");
}
```

### Key Sprite Data
- **Frames**: Each sprite has multiple animation frames
- **Texture Atlas**: Frames stored in texture pages (to reduce draw calls)
- **Origin**: Pivot point for rotation/scaling
- **Collision Mask**: Per-frame pixel-perfect collision data
- **Playback**: Animation speed and type (frames per second vs. per game frame)

---

## Stage 2: Runtime Loading

### GameLoader.cs - Sequential Loading

```csharp
public static void LoadGame()
{
    Console.WriteLine($"Loading game files...");

    // Check for replacement scripts (debugging/modding)
    var replacementFolder = Path.Combine(Entry.DataWinFolder, "replacement_scripts");
    if (Directory.Exists(replacementFolder))
    {
        var replacementScripts = Directory.GetFiles(replacementFolder, "*.json");
        foreach (var replacementScript in replacementScripts)
        {
            _replacementVMCodes.Add(JsonConvert.DeserializeObject<VMCode>(File.ReadAllText(replacementScript))!);
        }
    }

    using var stream = File.OpenRead(Path.Combine(Entry.DataWinFolder, "data_OpenGM.win"));
    using var reader = new BinaryReader(stream);

    try
    {
        // Must match order of GameConverter
        GeneralInfo = reader.ReadMemoryPack<GameData>();
        AssetIndexManager.LoadAssetIndexes(reader);
        LoadScripts(reader);
        LoadCode(reader);
        LoadExtensions(reader);
        LoadGlobalInitCode(reader);
        LoadObjects(reader);
        LoadBackgrounds(reader);
        LoadRooms(reader);
        LoadSprites(reader);
        LoadFonts(reader);
        LoadTexturePages(reader);
        LoadTextureGroups(reader);
        LoadTileSets(reader);
        AudioManager.LoadSounds(reader);
        LoadPaths(reader);
        LoadShaders(reader);
        LoadAnimCurves(reader);
    }
    catch
    {
        reader.Close();
        File.Delete(Path.Combine(Entry.DataWinFolder, "data_OpenGM.win"));
        throw;
    }

    GC.Collect(); // Cleanup after heavy loading
}
```

### Load Functions

**LoadCode()** - Recreate VM instruction cache
```csharp
private static void LoadCode(BinaryReader reader)
{
    Console.Write($"Loading code...");

    Codes.Clear();
    var length = reader.ReadInt32();
    
    for (var i = 0; i < length; i++)
    {
        var asset = reader.ReadMemoryPack<VMCode>();

        // Allow replacement scripts for debugging
        if (_replacementVMCodes.Any(x => x.Name == asset.Name))
        {
            DebugLog.Log($"Replacing {asset.Name} with custom script...");
            var assetID = asset.AssetId;
            var parentAssetID = asset.ParentAssetId;
            asset = _replacementVMCodes.First(x => x.Name == asset.Name);
            asset.AssetId = assetID;
            asset.ParentAssetId = parentAssetID;
        }

        Codes.Add(asset.AssetId, asset);
    }

    Console.WriteLine($" Done!");
}
```

**LoadRooms()** - Load room instances
```csharp
private static void LoadRooms(BinaryReader reader)
{
    Console.Write($"Loading rooms...");

    RoomManager.Rooms.Clear();

    var length = reader.ReadInt32();
    for (var i = 0; i < length; i++)
    {
        var asset = reader.ReadMemoryPack<CRoom>();
        RoomManager.Rooms.Add(asset);
    }

    Console.WriteLine($" Done!");
}
```

---

## Virtual Machine Execution

### VMExecutor - Stack-Based VM

The VM maintains:
- **DataStack** - Operands for operations
- **CallStack** - Function call frames
- **EnvStack** - Environment frames (current object context)

```csharp
public static partial class VMExecutor
{
    public static Stack<VMEnvFrame?> EnvStack = new();      // Object context
    public static Stack<VMCallFrame> CallStack = new();     // Function frames

    public class VMEnvFrame
    {
        public IStackContextSelf Self = null!;              // Current object
        public ObjectDefinition? ObjectDefinition;
    }

    public class VMCallFrame
    {
        public string CodeName = null!;
        public DataStack Stack = null!;                     // Operand stack
        public Dictionary<string, object?> Locals = null!;  // Local variables
        public object? ReturnValue;
        public EventType EventType;
        public int EventIndex;
    }
}
```

### Execution Loop

```csharp
// Execute global init scripts
VMExecutor.GlobalInit = true;
foreach (var item in ScriptResolver.GlobalInit)
{
    VMExecutor.ExecuteCode(item, null);
}
VMExecutor.GlobalInit = false;

// Change to first room
RoomManager.FirstRoom = true;
RoomManager.New_Room = GameLoader.GeneralInfo.RoomOrder[0];
RoomManager.ChangeToWaitingRoom();

// Main loop
window.Run();
```

### Built-in Functions

OpenGM implements ~1500+ GameMaker built-in functions:
- **Drawing**: `draw_sprite`, `draw_text`, `draw_rectangle`, etc.
- **Instance**: `instance_create`, `instance_destroy`, `with`, etc.
- **Room**: `room_goto`, `room_add`, etc.
- **Input**: `mouse_check_button`, `keyboard_check`, etc.
- **Audio**: `audio_play_sound`, `audio_stop_sound`, etc.
- **Math**: `sin`, `cos`, `sqrt`, `random`, etc.
- **String**: `string_concat`, `string_length`, etc.
- **File I/O**: `file_exists`, `file_open`, `file_write`, etc.
- Many more...

---

## Key Insights for C++ Implementation

### 1. Use UndertaleModLib for Extraction
- It's battle-tested and open-source
- Handles all GameMaker versions
- Provides full game data structures
- Can use via C# interop or build separate extractor tool

### 2. Two-Format Approach
- **Exchange Format**: Human-readable JSON for portability
  - Sprites with frame + texture coordinates
  - Rooms with instance positions + object references
  - Game objects with event code pointers
  - ALL bytecode (either as text or custom binary)
  
- **Runtime Format**: Optimized for execution
  - Pre-compiled VM instructions
  - Pre-indexed asset references
  - No text parsing needed

### 3. Critical Data to Extract
- **Sprites**: Frame bounds, texture page coords, animation data
- **Rooms**: Instance positions, layers, depths, camera settings
- **Objects**: Event handlers, sprites, properties, parent references
- **Code**: GML bytecode + disassembly info
- **Instances**: Position, scale, rotation, creation code references
- **Textures**: Page images + metadata

### 4. Build Order
1. **Extractor Tool** (C# using UndertaleModLib)
   - Parse data.win
   - Export to JSON/binary intermediate format
   
2. **VM Interpreter** (C++)
   - Load extracted bytecode
   - Implement opcode handlers
   - Replicate built-in functions
   
3. **Rendering** (SDL/OpenGL)
   - Draw sprites/text/shapes
   - Handle camera/viewport
   
4. **Input/Audio** (SDL)
   - Keyboard/mouse events
   - Audio playback

### 5. Format Portability
- **Custom binary format (.win) is .NET-specific**
  - Uses MemoryPack serializer
  - Not portable to C++
  - Solution: Export to JSON instead
  
- **JSON provides**
  - Human readability for debugging
  - Cross-platform compatibility
  - Easy to parse in C++
  - Git-friendly (text format)

---

## File Structure Reference

### OpenGM Project Layout
```
OpenGM/
├── Loading/
│   ├── GameConverter.cs      ← Extracts data.win to data_OpenGM.win
│   └── GameLoader.cs         ← Loads data_OpenGM.win at runtime
├── VirtualMachine/
│   ├── VMExecutor.cs         ← Stack-based VM
│   ├── DataStack.cs          ← Operand stack
│   ├── Opcodes/              ← Opcode implementations
│   └── BuiltInFunctions/     ← GameMaker function replicas
├── Rendering/                ← OpenGL rendering
├── Entry.cs                  ← Main entry point
└── SerializedFiles/          ← Data structures for binary serialization
```

### GameMaker File Format
```
data.win (binary)
├── GeneralInfo      - Game metadata
├── Scripts          - Script declarations
├── Code             - Compiled bytecode (binary)
├── Rooms            - Room definitions + instances
├── Sprites          - Sprite frame data + texture refs
├── Objects          - Object definitions + events
├── Backgrounds      - Background assets
├── Textures         - Texture page images
└── ...more asset types
```

---

## Performance Considerations

### OpenGM Optimizations
1. **Caching**: Pre-converts to data_OpenGM.win once, reuses on subsequent runs
2. **MemoryPack**: Fast binary serialization (way faster than JSON)
3. **Lazy Loading**: Could load assets on-demand (not currently done)
4. **Vertex Buffers**: Batched sprite rendering via GPU

### For C++ Implementation
- Use binary format (MessagePack, Protocol Buffers, or Cap'n Proto) instead of JSON for runtime
- Implement asset streaming for large games
- Pre-compile VM bytecode into native instructions (JIT) if performance needed
- Batch rendering calls

---

## Potential Issues & Solutions

### Issue 1: Version Compatibility
- **Problem**: GameMaker bytecode differs across engine versions
- **Solution**: OpenGM uses CompatFlags for version-specific behavior
- **For C++**: Extract version from data.win, handle format differences

### Issue 2: Custom Extensions
- **Problem**: Games may use DLL extensions
- **Solution**: OpenGM's ExtensionManager supports loading native libraries
- **For C++**: Can interop with or wrap DLLs

### Issue 3: Binary Format Variance
- **Problem**: data.win format changed between GM versions
- **Solution**: UndertaleModLib handles all known versions
- **For C++**: Use UndertaleModLib to convert, output stable format

### Issue 4: Code Replacement
- **Problem**: Debugging/modding requires code injection
- **Solution**: OpenGM supports replacement_scripts folder with JSON overrides
- **For C++**: Same approach - load override JSON files after main conversion

---

## Resources & References

- **OpenGM GitHub**: https://github.com/misternebula/OpenGM
- **UndertaleModLib GitHub**: https://github.com/UnderminersTeam/UndertaleModTool
- **UndertaleModLib Nuget**: https://www.nuget.org/packages/UndertaleModLib
- **GameMaker Documentation**: https://manual.gamemaker.io/

---

## Summary

OpenGM's approach is: **Binary Parse → Custom Conversion → Fast Loading → VM Execution**

This achieves:
1. ✅ **Portability** - Runs on Windows, Linux, Mac via OpenTK
2. ✅ **Accuracy** - Executes actual GML bytecode
3. ✅ **Performance** - Optimized binary format
4. ✅ **Compatibility** - Supports multiple GameMaker versions

For your C++ engine, the equivalent pipeline would be:
```
data.win 
  → [UndertaleModLib (C# tool)]
  → JSON/binary intermediate
  → [C++ engine loads & executes]
```
