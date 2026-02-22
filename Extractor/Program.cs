using System;
using System.IO;
using System.Text.Json;
using System.Text.Json.Serialization;
using UndertaleModLib;
using UndertaleModLib.Models;
using System.Collections.Generic;
using System.Linq;

namespace GameMakerExtractor
{
    public class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: Extractor.exe <path_to_data.win> [output_directory]");
                Console.WriteLine("Example: Extractor.exe C:\\Games\\Undertale\\data.win C:\\output");
                return;
            }

            string dataWinPath = args[0];
            string outputDir = args.Length > 1 ? args[1] : Path.Combine(Path.GetDirectoryName(dataWinPath) ?? "", "extracted");

            if (!File.Exists(dataWinPath))
            {
                Console.Error.WriteLine($"Error: File not found: {dataWinPath}");
                return;
            }

            try
            {
                Console.WriteLine($"Loading: {dataWinPath}");
                using var stream = File.OpenRead(dataWinPath);
                using var data = UndertaleIO.Read(stream);
                
                Directory.CreateDirectory(outputDir);
                
                Console.WriteLine("Extracting assets...");
                var extractor = new GameExtractor(data, outputDir);
                extractor.ExtractAll();
                
                Console.WriteLine($"✓ Extraction complete! Output: {outputDir}");
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"Error: {ex.Message}");
                Console.Error.WriteLine(ex.StackTrace);
                Environment.Exit(1);
            }
        }
    }

    public class GameExtractor
    {
        private readonly UndertaleData data;
        private readonly string outputDir;
        private readonly JsonSerializerOptions jsonOptions;

        public GameExtractor(UndertaleData data, string outputDir)
        {
            this.data = data;
            this.outputDir = outputDir;
            this.jsonOptions = new JsonSerializerOptions
            {
                WriteIndented = true,
                DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
                PropertyNamingPolicy = JsonNamingPolicy.CamelCase
            };
        }

        public void ExtractAll()
        {
            ExtractGeneralInfo();
            ExtractSprites();
            ExtractObjects();
            ExtractRooms();
            ExtractCode();
            ExtractTextures();
        }

        void ExtractGeneralInfo()
        {
            Console.Write("  General Info...");
            var info = new
            {
                name = data.GeneralInfo.Name.Content,
                version = data.GeneralInfo.Major,
                width = data.GeneralInfo.GameData.Width,
                height = data.GeneralInfo.GameData.Height,
                targetFPS = data.GeneralInfo.GameData.GameSpeed,
                windowColor = data.GeneralInfo.GameData.WindowColor.ToString()
            };

            var json = JsonSerializer.Serialize(info, jsonOptions);
            File.WriteAllText(Path.Combine(outputDir, "general_info.json"), json);
            Console.WriteLine(" ✓");
        }

        void ExtractSprites()
        {
            Console.Write("  Sprites...");
            var sprites = new List<SpriteData>();

            for (int i = 0; i < data.Sprites.Count; i++)
            {
                var sprite = data.Sprites[i];
                var frames = new List<SpriteFrame>();

                for (int f = 0; f < sprite.Frames.Count; f++)
                {
                    var frameRef = sprite.Frames[f];
                    var textureRef = frameRef.Texture;

                    frames.Add(new SpriteFrame
                    {
                        FrameIndex = f,
                        TexturePage = textureRef?.TexturePage?.Name?.Content ?? "",
                        SourceX = textureRef?.SourceX ?? 0,
                        SourceY = textureRef?.SourceY ?? 0,
                        SourceWidth = textureRef?.SourceWidth ?? sprite.Width,
                        SourceHeight = textureRef?.SourceHeight ?? sprite.Height,
                        BoundingBox = textureRef != null ? new BoundingBox
                        {
                            Left = textureRef.BoundingLeft,
                            Top = textureRef.BoundingTop,
                            Right = textureRef.BoundingRight,
                            Bottom = textureRef.BoundingBottom
                        } : null
                    });
                }

                sprites.Add(new SpriteData
                {
                    Id = i,
                    Name = sprite.Name.Content,
                    Width = sprite.Width,
                    Height = sprite.Height,
                    OriginX = sprite.OriginX,
                    OriginY = sprite.OriginY,
                    MarginLeft = sprite.MarginLeft,
                    MarginRight = sprite.MarginRight,
                    MarginTop = sprite.MarginTop,
                    MarginBottom = sprite.MarginBottom,
                    PlaybackSpeed = sprite.PlaybackSpeed,
                    PlaybackSpeedType = sprite.PlaybackSpeedType.ToString(),
                    Frames = frames
                });
            }

            var json = JsonSerializer.Serialize(sprites, jsonOptions);
            File.WriteAllText(Path.Combine(outputDir, "sprites.json"), json);
            Console.WriteLine($" ✓ ({sprites.Count} sprites)");
        }

        void ExtractObjects()
        {
            Console.Write("  Objects...");
            var objects = new List<GameObjectData>();

            for (int i = 0; i < data.GameObjects.Count; i++)
            {
                var obj = data.GameObjects[i];
                var events = new Dictionary<string, EventData>();

                // Extract event handlers
                foreach (var eventType in obj.Events)
                {
                    if (eventType == null) continue;

                    foreach (var eventItem in eventType)
                    {
                        if (eventItem?.EventSubtype == null || eventItem.CodeID == null) continue;

                        string eventKey = $"{eventItem.EventType}_{eventItem.EventSubtype}";
                        events[eventKey] = new EventData
                        {
                            EventType = eventItem.EventType.ToString(),
                            EventSubtype = eventItem.EventSubtype.ToString(),
                            CodeId = data.Code.IndexOf(eventItem.CodeID)
                        };
                    }
                }

                objects.Add(new GameObjectData
                {
                    Id = i,
                    Name = obj.Name.Content,
                    ParentId = obj.ParentId,
                    SpriteId = obj.SpriteId,
                    Solid = obj.Solid,
                    Visible = obj.Visible,
                    Depth = obj.Depth,
                    Events = events
                });
            }

            var json = JsonSerializer.Serialize(objects, jsonOptions);
            File.WriteAllText(Path.Combine(outputDir, "objects.json"), json);
            Console.WriteLine($" ✓ ({objects.Count} objects)");
        }

        void ExtractRooms()
        {
            Console.Write("  Rooms...");
            var rooms = new List<RoomData>();

            for (int r = 0; r < data.Rooms.Count; r++)
            {
                var room = data.Rooms[r];
                var instances = new List<InstanceData>();

                // Extract instances from all layers
                foreach (var layer in room.Layers)
                {
                    if (layer.LayerType == UndertaleRoom.LayerType.Instances && layer.InstancesData != null)
                    {
                        foreach (var instance in layer.InstancesData)
                        {
                            instances.Add(new InstanceData
                            {
                                InstanceId = (int)instance.InstanceID,
                                ObjectId = data.GameObjects.IndexOf(instance.ObjectDefinition),
                                X = instance.X,
                                Y = instance.Y,
                                ScaleX = instance.ScaleX,
                                ScaleY = instance.ScaleY,
                                Rotation = instance.Rotation,
                                Color = instance.Color.ToString(),
                                CreationCodeId = instance.CreationCode != null ? data.Code.IndexOf(instance.CreationCode) : -1,
                                ImageIndex = instance.ImageIndex,
                                ImageSpeed = instance.ImageSpeed
                            });
                        }
                    }
                }

                rooms.Add(new RoomData
                {
                    Id = r,
                    Name = room.Name.Content,
                    Width = room.Width,
                    Height = room.Height,
                    Speed = room.Speed,
                    SpeedType = room.SpeedType.ToString(),
                    ClearDisplayBuffer = room.ClearDisplayBuffer,
                    Instances = instances
                });
            }

            var json = JsonSerializer.Serialize(rooms, jsonOptions);
            File.WriteAllText(Path.Combine(outputDir, "rooms.json"), json);
            Console.WriteLine($" ✓ ({rooms.Count} rooms)");
        }

        void ExtractCode()
        {
            Console.Write("  Code...");
            var codes = new List<CodeData>();

            for (int i = 0; i < data.Code.Count; i++)
            {
                var code = data.Code[i];
                
                try
                {
                    var locals = data.CodeLocals?.For(code);
                    var assembly = code.Disassemble(data.Variables, locals);

                    codes.Add(new CodeData
                    {
                        Id = i,
                        Name = code.Name.Content,
                        ParentId = data.Code.IndexOf(code.ParentEntry),
                        Assembly = assembly.Substring(0, Math.Min(assembly.Length, 1000)) // Truncate for preview
                    });
                }
                catch
                {
                    // Skip code that fails to disassemble
                    codes.Add(new CodeData
                    {
                        Id = i,
                        Name = code.Name.Content,
                        ParentId = data.Code.IndexOf(code.ParentEntry),
                        Assembly = "[Failed to disassemble]"
                    });
                }
            }

            var json = JsonSerializer.Serialize(codes, jsonOptions);
            File.WriteAllText(Path.Combine(outputDir, "code.json"), json);
            Console.WriteLine($" ✓ ({codes.Count} code objects)");
        }

        void ExtractTextures()
        {
            Console.Write("  Textures...");
            string texturesDir = Path.Combine(outputDir, "sprites");
            Directory.CreateDirectory(texturesDir);

            int saved = 0;
            foreach (var page in data.TexturePages)
            {
                try
                {
                    if (page.Texture != null)
                    {
                        string filename = Path.Combine(texturesDir, $"page_{page.Name.Content}.png");
                        page.Texture.ExportPNG(filename);
                        saved++;
                    }
                }
                catch
                {
                    // Skip textures that fail to export
                }
            }

            Console.WriteLine($" ✓ ({saved} texture pages)");
        }
    }

    // Data classes for JSON serialization
    public class SpriteData
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public uint Width { get; set; }
        public uint Height { get; set; }
        public int OriginX { get; set; }
        public int OriginY { get; set; }
        public int MarginLeft { get; set; }
        public int MarginRight { get; set; }
        public int MarginTop { get; set; }
        public int MarginBottom { get; set; }
        public double PlaybackSpeed { get; set; }
        public string PlaybackSpeedType { get; set; }
        public List<SpriteFrame> Frames { get; set; }
    }

    public class SpriteFrame
    {
        public int FrameIndex { get; set; }
        public string TexturePage { get; set; }
        public int SourceX { get; set; }
        public int SourceY { get; set; }
        public int SourceWidth { get; set; }
        public int SourceHeight { get; set; }
        public BoundingBox BoundingBox { get; set; }
    }

    public class BoundingBox
    {
        public int Left { get; set; }
        public int Top { get; set; }
        public int Right { get; set; }
        public int Bottom { get; set; }
    }

    public class GameObjectData
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int ParentId { get; set; }
        public int SpriteId { get; set; }
        public bool Solid { get; set; }
        public bool Visible { get; set; }
        public int Depth { get; set; }
        public Dictionary<string, EventData> Events { get; set; }
    }

    public class EventData
    {
        public string EventType { get; set; }
        public string EventSubtype { get; set; }
        public int CodeId { get; set; }
    }

    public class RoomData
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public int Speed { get; set; }
        public string SpeedType { get; set; }
        public bool ClearDisplayBuffer { get; set; }
        public List<InstanceData> Instances { get; set; }
    }

    public class InstanceData
    {
        public int InstanceId { get; set; }
        public int ObjectId { get; set; }
        public int X { get; set; }
        public int Y { get; set; }
        public double ScaleX { get; set; }
        public double ScaleY { get; set; }
        public double Rotation { get; set; }
        public string Color { get; set; }
        public int CreationCodeId { get; set; }
        public double ImageIndex { get; set; }
        public double ImageSpeed { get; set; }
    }

    public class CodeData
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int ParentId { get; set; }
        public string Assembly { get; set; }
    }
}
