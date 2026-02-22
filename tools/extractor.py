#!/usr/bin/env python3
"""
GameMaker Asset Extractor
Converts raw GameMaker assets to portable JSON format for C++ runtime.

Usage:
    python extractor.py <input_dir> [output_dir]
    
    Where input_dir contains extracted GameMaker assets
    (sprites, rooms, objects, code folders from UndertaleModTool)
"""

import json
import os
import sys
from pathlib import Path
from dataclasses import dataclass, asdict, field
from typing import Dict, List, Optional, Any
import re


@dataclass
class SpriteFrame:
    frameIndex: int
    texturePage: str
    sourceX: int
    sourceY: int
    sourceWidth: int
    sourceHeight: int
    boundingBox: Optional[Dict[str, int]] = None


@dataclass
class SpriteData:
    id: int
    name: str
    width: int
    height: int
    originX: int
    originY: int
    marginLeft: int
    marginRight: int
    marginTop: int
    marginBottom: int
    playbackSpeed: float
    playbackSpeedType: str
    frames: List[SpriteFrame] = field(default_factory=list)


@dataclass
class EventData:
    eventType: str
    eventSubtype: str
    codeId: int


@dataclass
class GameObjectData:
    id: int
    name: str
    parentId: int
    spriteId: int
    solid: bool
    visible: bool
    depth: int
    events: Dict[str, EventData] = field(default_factory=dict)


@dataclass
class InstanceData:
    instanceId: int
    objectId: int
    x: int
    y: int
    scaleX: float
    scaleY: float
    rotation: float
    color: str
    creationCodeId: int
    imageIndex: float
    imageSpeed: float


@dataclass
class RoomData:
    id: int
    name: str
    width: int
    height: int
    speed: int
    speedType: str
    clearDisplayBuffer: bool
    instances: List[InstanceData] = field(default_factory=list)


@dataclass
class CodeData:
    id: int
    name: str
    parentId: int
    assembly: str


@dataclass
class GeneralInfo:
    name: str
    version: str
    width: int
    height: int
    targetFPS: int
    windowColor: str


class GameExtractor:
    def __init__(self, input_dir: str, output_dir: str):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        self.sprites_map: Dict[int, SpriteData] = {}
        self.objects_map: Dict[int, GameObjectData] = {}
        self.rooms_map: Dict[int, RoomData] = {}
        self.code_map: Dict[int, CodeData] = {}

    def extract_all(self):
        """Extract all game assets"""
        print("Extracting assets...")
        print("  General Info...", end=" ", flush=True)
        self.extract_general_info()
        print("✓")
        
        print("  Sprites...", end=" ", flush=True)
        self.extract_sprites()
        print(f"✓ ({len(self.sprites_map)} sprites)")
        
        print("  Objects...", end=" ", flush=True)
        self.extract_objects()
        print(f"✓ ({len(self.objects_map)} objects)")
        
        print("  Rooms...", end=" ", flush=True)
        self.extract_rooms()
        print(f"✓ ({len(self.rooms_map)} rooms)")
        
        print("  Code...", end=" ", flush=True)
        self.extract_code()
        print(f"✓ ({len(self.code_map)} code objects)")

    def extract_general_info(self):
        """Extract game metadata"""
        info = GeneralInfo(
            name="GameMaker Game",
            version="2.3",
            width=1024,
            height=768,
            targetFPS=60,
            windowColor="#000000"
        )
        
        self._save_json("general_info.json", asdict(info))

    def extract_sprites(self):
        """Extract sprite definitions - for UMT dumps, use placeholder data"""
        # UMT dumps don't have sprite frame data, so create minimal entries
        # Real sprite extraction requires UndertaleModLib (C#/.NET only)
        
        print("(extracting sprite references)", end="")
        
        # Extract embedded textures if available
        textures_dir = self.input_dir / "EmbeddedTextures"
        sprite_id = 0
        
        if textures_dir.exists():
            for tex_file in sorted(textures_dir.glob("*.png")):
                sprite = SpriteData(
                    id=sprite_id,
                    name=tex_file.stem,
                    width=256,  # Placeholder
                    height=256,  # Placeholder
                    originX=0,
                    originY=0,
                    marginLeft=0,
                    marginRight=0,
                    marginTop=0,
                    marginBottom=0,
                    playbackSpeed=0,
                    playbackSpeedType="FramesPerGameFrame",
                    frames=[
                        SpriteFrame(
                            frameIndex=0,
                            texturePage=tex_file.stem,
                            sourceX=0,
                            sourceY=0,
                            sourceWidth=256,
                            sourceHeight=256
                        )
                    ]
                )
                self.sprites_map[sprite_id] = sprite
                sprite_id += 1

        self._save_json("sprites.json", 
                       [asdict(s, dict_factory=self._dataclass_to_dict) 
                        for s in self.sprites_map.values()])
        print(f" ✓ ({sprite_id} sprites from embedded textures)")

    def _parse_sprite_json(self, path: Path, sprite_id: int):
        """Parse sprite from JSON file"""
        with open(path) as f:
            data = json.load(f)
        
        frames = []
        for i, frame_data in enumerate(data.get("frames", [])):
            frames.append(SpriteFrame(
                frameIndex=i,
                texturePage=frame_data.get("texturePage", ""),
                sourceX=frame_data.get("sourceX", 0),
                sourceY=frame_data.get("sourceY", 0),
                sourceWidth=frame_data.get("sourceWidth", 0),
                sourceHeight=frame_data.get("sourceHeight", 0)
            ))

        sprite = SpriteData(
            id=sprite_id,
            name=data.get("name", path.stem),
            width=data.get("width", 0),
            height=data.get("height", 0),
            originX=data.get("originX", 0),
            originY=data.get("originY", 0),
            marginLeft=data.get("marginLeft", 0),
            marginRight=data.get("marginRight", 0),
            marginTop=data.get("marginTop", 0),
            marginBottom=data.get("marginBottom", 0),
            playbackSpeed=data.get("playbackSpeed", 0),
            playbackSpeedType=data.get("playbackSpeedType", "FramesPerGameFrame"),
            frames=frames
        )
        
        self.sprites_map[sprite_id] = sprite

    def _parse_sprite_directory(self, path: Path, sprite_id: int) -> Optional[SpriteData]:
        """Parse sprite from directory structure"""
        # Look for info.json or sprite.json
        info_file = path / "info.json"
        
        if info_file.exists():
            with open(info_file) as f:
                info = json.load(f)
            
            frames = []
            frames_dir = path / "frames"
            if frames_dir.exists():
                for i, frame_file in enumerate(sorted(frames_dir.glob("*.json"))):
                    with open(frame_file) as f:
                        frame_data = json.load(f)
                    frames.append(SpriteFrame(
                        frameIndex=i,
                        texturePage=frame_data.get("texturePage", "default"),
                        sourceX=frame_data.get("sourceX", 0),
                        sourceY=frame_data.get("sourceY", 0),
                        sourceWidth=frame_data.get("sourceWidth", info.get("width", 0)),
                        sourceHeight=frame_data.get("sourceHeight", info.get("height", 0))
                    ))
            
            sprite = SpriteData(
                id=sprite_id,
                name=info.get("name", path.name),
                width=info.get("width", 0),
                height=info.get("height", 0),
                originX=info.get("originX", 0),
                originY=info.get("originY", 0),
                marginLeft=info.get("marginLeft", 0),
                marginRight=info.get("marginRight", 0),
                marginTop=info.get("marginTop", 0),
                marginBottom=info.get("marginBottom", 0),
                playbackSpeed=info.get("playbackSpeed", 0),
                playbackSpeedType=info.get("playbackSpeedType", "FramesPerGameFrame"),
                frames=frames
            )
            
            return sprite
        
        return None

    def extract_objects(self):
        """Extract game object definitions"""
        objects_dir = self.input_dir / "objects"
        
        if not objects_dir.exists():
            print("(no objects found)", end="")
            return

        obj_id = 0
        for obj_file in sorted(objects_dir.glob("*.json")):
            try:
                with open(obj_file) as f:
                    obj_data = json.load(f)
                
                events = {}
                for event_key, event_info in obj_data.get("events", {}).items():
                    events[event_key] = EventData(
                        eventType=event_info.get("eventType", "0"),
                        eventSubtype=event_info.get("eventSubtype", "0"),
                        codeId=event_info.get("codeId", -1)
                    )
                
                obj = GameObjectData(
                    id=obj_id,
                    name=obj_data.get("name", obj_file.stem),
                    parentId=obj_data.get("parentId", -1),
                    spriteId=obj_data.get("spriteId", -1),
                    solid=obj_data.get("solid", False),
                    visible=obj_data.get("visible", True),
                    depth=obj_data.get("depth", 0),
                    events=events
                )
                
                self.objects_map[obj_id] = obj
                obj_id += 1
            except Exception as e:
                print(f"\n    Warning: Failed to parse {obj_file.name}: {e}")

        self._save_json("objects.json",
                       [asdict(o, dict_factory=self._dataclass_to_dict)
                        for o in self.objects_map.values()])

    def extract_rooms(self):
        """Extract room definitions"""
        rooms_dir = self.input_dir / "rooms"
        
        if not rooms_dir.exists():
            print("(no rooms found)", end="")
            return

        room_id = 0
        for room_file in sorted(rooms_dir.glob("*.json")):
            try:
                with open(room_file) as f:
                    room_data = json.load(f)
                
                instances = []
                for inst in room_data.get("instances", []):
                    instances.append(InstanceData(
                        instanceId=inst.get("instanceId", 0),
                        objectId=inst.get("objectId", -1),
                        x=inst.get("x", 0),
                        y=inst.get("y", 0),
                        scaleX=inst.get("scaleX", 1.0),
                        scaleY=inst.get("scaleY", 1.0),
                        rotation=inst.get("rotation", 0),
                        color=inst.get("color", "#FFFFFF"),
                        creationCodeId=inst.get("creationCodeId", -1),
                        imageIndex=inst.get("imageIndex", 0),
                        imageSpeed=inst.get("imageSpeed", 0)
                    ))
                
                room = RoomData(
                    id=room_id,
                    name=room_data.get("name", room_file.stem),
                    width=room_data.get("width", 800),
                    height=room_data.get("height", 600),
                    speed=room_data.get("speed", 60),
                    speedType=room_data.get("speedType", "GameFramePerGameFrame"),
                    clearDisplayBuffer=room_data.get("clearDisplayBuffer", True),
                    instances=instances
                )
                
                self.rooms_map[room_id] = room
                room_id += 1
            except Exception as e:
                print(f"\n    Warning: Failed to parse {room_file.name}: {e}")

        self._save_json("rooms.json",
                       [asdict(r, dict_factory=self._dataclass_to_dict)
                        for r in self.rooms_map.values()])

    def extract_code(self):
        """Extract code/script definitions from CodeEntries or code directory"""
        # Try CodeEntries first (UMT dump format)
        code_dir = self.input_dir / "CodeEntries"
        if not code_dir.exists():
            # Try code directory as fallback
            code_dir = self.input_dir / "code"
        
        if not code_dir.exists():
            print("(no code found)", end="")
            return

        code_id = 0
        for code_file in sorted(code_dir.glob("*.gml")):
            try:
                with open(code_file, 'r', encoding='utf-8', errors='replace') as f:
                    assembly = f.read()
                
                code = CodeData(
                    id=code_id,
                    name=code_file.stem,
                    parentId=-1,
                    assembly=assembly[:1000]  # Truncate for preview
                )
                
                self.code_map[code_id] = code
                code_id += 1
            except Exception as e:
                print(f"\n    Warning: Failed to parse {code_file.name}: {e}")

        self._save_json("code.json",
                       [asdict(c, dict_factory=self._dataclass_to_dict)
                        for c in self.code_map.values()])

    def _save_json(self, filename: str, data: Any):
        """Save data structure to JSON file"""
        output_path = self.output_dir / filename
        with open(output_path, 'w') as f:
            json.dump(data, f, indent=2)

    @staticmethod
    def _dataclass_to_dict(fields):
        """Convert dataclass fields for JSON serialization"""
        result = {}
        for key, value in fields:
            if hasattr(value, '__dataclass_fields__'):
                result[key] = asdict(value, dict_factory=GameExtractor._dataclass_to_dict)
            elif isinstance(value, (list, tuple)):
                result[key] = [
                    asdict(item, dict_factory=GameExtractor._dataclass_to_dict)
                    if hasattr(item, '__dataclass_fields__') else item
                    for item in value
                ]
            else:
                result[key] = value
        return result


def main():
    if len(sys.argv) < 2:
        print("GameMaker Asset Extractor")
        print("Usage: python extractor.py <input_dir> [output_dir]")
        print("\nExample:")
        print("  python extractor.py ./dump_undertale ./extracted")
        sys.exit(1)

    input_dir = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else os.path.join(input_dir, "extracted")

    if not os.path.isdir(input_dir):
        print(f"Error: Input directory not found: {input_dir}", file=sys.stderr)
        sys.exit(1)

    try:
        extractor = GameExtractor(input_dir, output_dir)
        extractor.extract_all()
        print(f"\n✓ Extraction complete! Output: {output_dir}")
    except Exception as e:
        print(f"\nError: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
