#!/usr/bin/env python3
"""
UMT (UndertaleModTool) Dump to JSON Converter

Converts UMT dump output (binary data.win files) to JSON format compatible with
the C++ GameMaker asset loader.

Usage:
    python umt_to_json_converter.py <umt_dump_directory> <output_json>
    
Example:
    python umt_to_json_converter.py undertale_dump/ undertale_game.json

Expected UMT dump structure:
    dump/
    ├── sprites/
    │   ├── spr_0/
    │   │   ├── xoffset, yoffset, width, height
    │   │   └── frames/ (texture references)
    │   └── spr_1/
    ├── objects/
    │   ├── obj_0/
    │   │   ├── name, mask_index, parent_index
    │   │   └── events/
    │   └── obj_1/
    ├── rooms/
    │   ├── rm_0/
    │   └── rm_1/
    ├── sounds/
    │   ├── snd_0/
    │   └── snd_1/
    ├── textures/
    └── strings/
"""

import os
import json
import sys
import re
from pathlib import Path
from typing import Dict, List, Any, Optional, Set
from dataclasses import dataclass, asdict
from enum import IntEnum


class SoundType(IntEnum):
    """Sound type enumeration matching C++ enum"""
    AUDIO_SOUND = 0
    AUDIO_MUSIC = 1
    AUDIO_SFX = 2


class PlaybackSpeedType(IntEnum):
    """Playback speed type matching C++ enum"""
    FRAMES_PER_SECOND = 0
    FRAMES_PER_GAME_FRAME = 1


@dataclass
class AnimationFrame:
    """Represents a single frame in a sprite animation"""
    texture_id: int
    duration: float = 1.0
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class Sprite:
    """Represents a GameMaker sprite"""
    id: int
    name: str
    width: int
    height: int
    x_offset: int = 0
    y_offset: int = 0
    collision_type: int = 0
    frames: List[AnimationFrame] = None
    playback_speed: float = 15.0
    playback_speed_type: int = PlaybackSpeedType.FRAMES_PER_GAME_FRAME
    
    def __post_init__(self):
        if self.frames is None:
            self.frames = []
    
    def to_dict(self) -> Dict[str, Any]:
        data = asdict(self)
        data['frames'] = [f.to_dict() for f in self.frames]
        return data


@dataclass
class Object:
    """Represents a GameMaker object"""
    id: int
    name: str
    sprite_id: int = -1
    solid: bool = False
    events: Dict[str, str] = None
    creation_code: str = ""
    
    def __post_init__(self):
        if self.events is None:
            self.events = {}
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class Instance:
    """Represents a room instance (object placed in room)"""
    id: int
    object_id: int
    x: int = 0
    y: int = 0
    creation_code: str = ""
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class Room:
    """Represents a GameMaker room"""
    id: int
    name: str
    width: int
    height: int
    instances: List[Instance] = None
    background_color: str = "0xFF000000"
    background_index: int = -1
    clear_buffer_color: int = 0xFFFFFFFF
    
    def __post_init__(self):
        if self.instances is None:
            self.instances = []
    
    def to_dict(self) -> Dict[str, Any]:
        data = asdict(self)
        data['instances'] = [inst.to_dict() for inst in self.instances]
        return data


@dataclass
class Sound:
    """Represents a GameMaker sound"""
    id: int
    name: str
    sound_type: int = SoundType.AUDIO_SOUND
    file_path: str = ""
    volume: float = 1.0
    pitch: float = 1.0
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


class UTConverter:
    """Converts UMT dump output to JSON format"""
    
    def __init__(self, dump_dir: str, verbose: bool = True):
        self.dump_dir = Path(dump_dir)
        self.verbose = verbose
        
        self.sprites: Dict[int, Sprite] = {}
        self.objects: Dict[int, Object] = {}
        self.rooms: Dict[int, Room] = {}
        self.sounds: Dict[int, Sound] = {}
        self.textures: Dict[int, str] = {}  # id -> filename
        
        if not self.dump_dir.exists():
            raise FileNotFoundError(f"Dump directory not found: {dump_dir}")
    
    def log(self, message: str):
        """Log message if verbose mode is enabled"""
        if self.verbose:
            print(f"[UMT Converter] {message}")
    
    def convert(self) -> Dict[str, Any]:
        """Convert UMT dump to JSON structure"""
        self.log("Starting conversion...")
        
        self._load_textures()
        self._load_sprites()
        self._load_objects()
        self._load_rooms()
        self._load_sounds()
        
        self.log("Conversion complete!")
        self.log(f"  Textures: {len(self.textures)}")
        self.log(f"  Sprites: {len(self.sprites)}")
        self.log(f"  Objects: {len(self.objects)}")
        self.log(f"  Rooms: {len(self.rooms)}")
        self.log(f"  Sounds: {len(self.sounds)}")
        
        return self._build_json()
    
    def _load_textures(self):
        """Load texture information from textures directory"""
        # Try new UMT CLI format first (EmbeddedTextures)
        textures_dir = self.dump_dir / "EmbeddedTextures"
        if not textures_dir.exists():
            # Fall back to original format (Textures)
            textures_dir = self.dump_dir / "Textures"
        
        if not textures_dir.exists():
            self.log("No textures directory found")
            return
        
        try:
            # Simple approach: list all image files
            for i, texture_file in enumerate(sorted(textures_dir.iterdir())):
                if texture_file.is_file():
                    self.textures[i] = texture_file.name
            
            if self.textures:
                self.log(f"Loaded {len(self.textures)} textures")
        except Exception as e:
            self.log(f"Error loading textures: {e}")
    
    def _load_sprites(self):
        """Load sprite data from sprites directory"""
        sprites_dir = self.dump_dir / "Sprites"
        if not sprites_dir.exists():
            self.log("No sprites directory found")
            return
        
        try:
            for sprite_idx, sprite_dir in enumerate(sorted(sprites_dir.iterdir())):
                if not sprite_dir.is_dir():
                    continue
                
                sprite_name = sprite_dir.name
                sprite_id = sprite_idx
                
                # Try to read sprite metadata
                metadata = self._read_sprite_metadata(sprite_dir)
                
                sprite = Sprite(
                    id=sprite_id,
                    name=sprite_name,
                    width=metadata.get('width', 32),
                    height=metadata.get('height', 32),
                    x_offset=metadata.get('xoffset', 0),
                    y_offset=metadata.get('yoffset', 0),
                )
                
                # Load frames
                frames_dir = sprite_dir / "frames"
                if frames_dir.exists():
                    for frame_idx, frame_file in enumerate(sorted(frames_dir.iterdir())):
                        if frame_file.is_file():
                            # Try to parse texture ID from filename
                            texture_id = self._extract_texture_id(frame_file.name)
                            frame = AnimationFrame(texture_id=texture_id, duration=1.0)
                            sprite.frames.append(frame)
                
                self.sprites[sprite_id] = sprite
            
            if self.sprites:
                self.log(f"Loaded {len(self.sprites)} sprites")
        except Exception as e:
            self.log(f"Error loading sprites: {e}")
    
    def _read_sprite_metadata(self, sprite_dir: Path) -> Dict[str, Any]:
        """Read sprite metadata from UMT dump"""
        metadata = {
            'width': 32,
            'height': 32,
            'xoffset': 0,
            'yoffset': 0,
        }
        
        # Try to read from metadata files
        for key in ['width', 'height', 'xoffset', 'yoffset', 'bbox_left', 'bbox_top', 'bbox_right', 'bbox_bottom']:
            filepath = sprite_dir / key
            if filepath.exists():
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        value = f.read().strip()
                        if key in ['width', 'height', 'xoffset', 'yoffset', 'bbox_left', 'bbox_top', 'bbox_right', 'bbox_bottom']:
                            metadata[key] = int(value)
                except:
                    pass
        
        return metadata
    
    def _extract_texture_id(self, filename: str) -> int:
        """Extract texture ID from filename"""
        # Try to parse texture ID from filename like "texture_123.png" or similar
        match = re.search(r'(\d+)', filename)
        if match:
            return int(match.group(1))
        return 0
    
    def _load_objects(self):
        """Load object data from objects directory"""
        # Try original format first
        objects_dir = self.dump_dir / "Objects"
        if objects_dir.exists():
            self._load_objects_original_format(objects_dir)
            return
        
        # Fall back to UMT CLI format (CodeEntries)
        code_dir = self.dump_dir / "CodeEntries"
        if code_dir.exists():
            self._load_objects_from_code_entries(code_dir)
            return
        
        self.log("No objects directory found")
    
    def _load_objects_from_code_entries(self, code_dir: Path):
        """Load objects from UMT CLI CodeEntries format"""
        try:
            # Parse gml files to extract object names and events
            # Files are named like: gml_Object_<name>_<event>.gml
            object_map = {}  # name -> {events: {}, ...}
            
            for code_file in sorted(code_dir.glob("gml_Object_*.gml")):
                # Parse filename
                stem = code_file.stem
                parts = stem.split("_", 3)  # gml_Object_<name>_<event>
                
                if len(parts) >= 4:
                    obj_name = parts[2]
                    event_name = "_".join(parts[3:])
                    
                    if obj_name not in object_map:
                        object_map[obj_name] = {"events": {}}
                    
                    try:
                        with open(code_file, 'r', encoding='utf-8', errors='ignore') as f:
                            code = f.read()
                            object_map[obj_name]["events"][event_name] = code
                    except:
                        pass
            
            # Convert to object list
            for obj_idx, (obj_name, obj_data) in enumerate(sorted(object_map.items())):
                obj = Object(
                    id=obj_idx,
                    name=obj_name,
                    events=obj_data["events"],
                )
                self.objects[obj_idx] = obj
            
            if self.objects:
                self.log(f"Loaded {len(self.objects)} objects from CodeEntries")
        except Exception as e:
            self.log(f"Error loading objects from CodeEntries: {e}")
    
    def _load_objects_original_format(self, objects_dir: Path):
        """Load object data from original UMT dump format"""
        try:
            for obj_idx, obj_dir in enumerate(sorted(objects_dir.iterdir())):
                if not obj_dir.is_dir():
                    continue
                
                obj_name = obj_dir.name
                obj_id = obj_idx
                
                # Read metadata
                metadata = self._read_object_metadata(obj_dir)
                
                obj = Object(
                    id=obj_id,
                    name=obj_name,
                    sprite_id=metadata.get('sprite_index', -1),
                    solid=metadata.get('solid', False),
                )
                
                # Try to read events
                events_dir = obj_dir / "events"
                if events_dir.exists():
                    for event_file in events_dir.iterdir():
                        if event_file.is_file():
                            try:
                                with open(event_file, 'r', encoding='utf-8', errors='ignore') as f:
                                    event_code = f.read()
                                    obj.events[event_file.stem] = event_code
                            except:
                                pass
                
                self.objects[obj_id] = obj
            
            if self.objects:
                self.log(f"Loaded {len(self.objects)} objects")
        except Exception as e:
            self.log(f"Error loading objects: {e}")
    
    def _read_object_metadata(self, obj_dir: Path) -> Dict[str, Any]:
        """Read object metadata from UMT dump"""
        metadata = {
            'sprite_index': -1,
            'solid': False,
        }
        
        # Try to read sprite index
        sprite_file = obj_dir / "sprite_index"
        if sprite_file.exists():
            try:
                with open(sprite_file, 'r', encoding='utf-8', errors='ignore') as f:
                    metadata['sprite_index'] = int(f.read().strip())
            except:
                pass
        
        # Try to read solid flag
        solid_file = obj_dir / "solid"
        if solid_file.exists():
            try:
                with open(solid_file, 'r', encoding='utf-8', errors='ignore') as f:
                    metadata['solid'] = f.read().strip().lower() in ['true', '1', 'yes']
            except:
                pass
        
        return metadata
    
    def _load_rooms(self):
        """Load room data from rooms directory or create defaults"""
        rooms_dir = self.dump_dir / "Rooms"
        
        if rooms_dir.exists():
            try:
                for room_idx, room_dir in enumerate(sorted(rooms_dir.iterdir())):
                    if not room_dir.is_dir():
                        continue
                    
                    room_name = room_dir.name
                    room_id = room_idx
                    
                    # Read metadata
                    metadata = self._read_room_metadata(room_dir)
                    
                    room = Room(
                        id=room_id,
                        name=room_name,
                        width=metadata.get('width', 1024),
                        height=metadata.get('height', 768),
                    )
                    
                    # Load instances
                    instances_file = room_dir / "instances.json"
                    if instances_file.exists():
                        try:
                            with open(instances_file, 'r', encoding='utf-8') as f:
                                instances_data = json.load(f)
                                if isinstance(instances_data, list):
                                    for inst_idx, inst_data in enumerate(instances_data):
                                        inst = Instance(
                                            id=inst_idx,
                                            x=int(inst_data.get('x', 0)),
                                            y=int(inst_data.get('y', 0)),
                                            object_id=int(inst_data.get('objectId', 0)),
                                        )
                                        room.instances.append(inst)
                        except Exception as e:
                            self.log(f"Error loading instances: {e}")
                    
                    self.rooms[room_id] = room
                
                if self.rooms:
                    self.log(f"Loaded {len(self.rooms)} rooms from disk")
                else:
                    self._create_default_rooms()
            except Exception as e:
                self.log(f"Error loading rooms from disk: {e}")
                self._create_default_rooms()
        else:
            # No Rooms directory found - create a default room
            self._create_default_rooms()
    
    def _create_default_rooms(self):
        """Create default rooms when no room data is available"""
        # Create a simple Basement room like Undertale
        room = Room(
            id=0,
            name="Basement",
            width=800,
            height=600,
            background_color="0xFF000000"  # Black background
        )
        
        # Add some instances at reasonable positions
        # Using the available objects from the game
        positions = [
            (200, 150, 0),  # Object 0 at (200, 150)
            (400, 150, 1),  # Object 1 at (400, 150)
            (600, 150, 2),  # Object 2 at (600, 150)
            (200, 350, 0),  # Object 0 at (200, 350)
            (400, 350, 1),  # Object 1 at (400, 350)
            (600, 350, 2),  # Object 2 at (600, 350)
        ]
        
        for inst_id, (x, y, obj_id) in enumerate(positions):
            inst = Instance(
                id=inst_id,
                object_id=min(obj_id, len(self.objects) - 1),  # Clamp to available objects
                x=x,
                y=y,
                creation_code=""
            )
            room.instances.append(inst)
        
        self.rooms[0] = room
        self.log("Created default Basement room with instances")
    
    def _read_room_metadata(self, room_dir: Path) -> Dict[str, Any]:
        """Read room metadata from UMT dump"""
        metadata = {
            'width': 1024,
            'height': 768,
        }
        
        # Try to read dimensions
        width_file = room_dir / "width"
        height_file = room_dir / "height"
        
        if width_file.exists():
            try:
                with open(width_file, 'r', encoding='utf-8', errors='ignore') as f:
                    metadata['width'] = int(f.read().strip())
            except:
                pass
        
        if height_file.exists():
            try:
                with open(height_file, 'r', encoding='utf-8', errors='ignore') as f:
                    metadata['height'] = int(f.read().strip())
            except:
                pass
        
        return metadata
    
    def _load_sounds(self):
        """Load sound data from sounds directory"""
        sounds_dir = self.dump_dir / "Sounds"
        if not sounds_dir.exists():
            self.log("No sounds directory found")
            return
        
        try:
            for sound_idx, sound_dir in enumerate(sorted(sounds_dir.iterdir())):
                if not sound_dir.is_dir():
                    continue
                
                sound_name = sound_dir.name
                sound_id = sound_idx
                
                # Try to find audio file
                audio_file = None
                for ext in ['.ogg', '.mp3', '.wav', '.flac']:
                    candidate = sound_dir / f"{sound_name}{ext}"
                    if candidate.exists():
                        audio_file = candidate.name
                        break
                
                sound = Sound(
                    id=sound_id,
                    name=sound_name,
                    file_path=audio_file or "",
                )
                
                self.sounds[sound_id] = sound
            
            if self.sounds:
                self.log(f"Loaded {len(self.sounds)} sounds")
        except Exception as e:
            self.log(f"Error loading sounds: {e}")
    
    def _build_json(self) -> Dict[str, Any]:
        """Build the final JSON structure"""
        return {
            'game': {
                'name': self.dump_dir.name,
                'version': '1.0',
            },
            'textures': list(self.textures.values()),
            'sprites': [sprite.to_dict() for sprite in sorted(self.sprites.values(), key=lambda s: s.id)],
            'objects': [obj.to_dict() for obj in sorted(self.objects.values(), key=lambda o: o.id)],
            'rooms': [room.to_dict() for room in sorted(self.rooms.values(), key=lambda r: r.id)],
            'sounds': [sound.to_dict() for sound in sorted(self.sounds.values(), key=lambda s: s.id)],
        }
    
    def save_json(self, output_path: str):
        """Save converted data to JSON file"""
        json_data = self.convert()
        
        output_file = Path(output_path)
        output_file.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(json_data, f, indent=2)
        
        self.log(f"Saved JSON to: {output_path}")
        return json_data


def main():
    """Command-line interface"""
    if len(sys.argv) < 2:
        print(__doc__)
        print("\nUsage: python umt_to_json_converter.py <dump_dir> [output.json]")
        sys.exit(1)
    
    dump_dir = sys.argv[1]
    output_json = sys.argv[2] if len(sys.argv) > 2 else "converted_game.json"
    
    try:
        converter = UTConverter(dump_dir, verbose=True)
        converter.save_json(output_json)
        print(f"\n✓ Conversion successful!")
        print(f"Output: {output_json}")
    except Exception as e:
        print(f"\n✗ Conversion failed: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
