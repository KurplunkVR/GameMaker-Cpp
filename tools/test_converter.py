#!/usr/bin/env python3
"""
Test suite for UMT to JSON converter

Tests the converter with simulated UMT dump structures to ensure it correctly
parses and converts GameMaker asset data.
"""

import os
import sys
import json
import tempfile
from pathlib import Path
import shutil


# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from umt_to_json_converter import (
    UTConverter, Sprite, Object, Room, Sound, AnimationFrame,
    SoundType, PlaybackSpeedType
)


def create_test_dump(dump_dir: Path) -> None:
    """Create a test UMT dump structure"""
    dump_dir.mkdir(parents=True, exist_ok=True)
    
    # Create textures directory
    textures_dir = dump_dir / "Textures"
    textures_dir.mkdir()
    (textures_dir / "texture_0.png").touch()
    (textures_dir / "texture_1.png").touch()
    
    # Create sprites directory
    sprites_dir = dump_dir / "Sprites"
    sprites_dir.mkdir()
    
    # Sprite 0: spr_test_1
    spr_0 = sprites_dir / "spr_test_1"
    spr_0.mkdir()
    (spr_0 / "width").write_text("32")
    (spr_0 / "height").write_text("32")
    (spr_0 / "xoffset").write_text("0")
    (spr_0 / "yoffset").write_text("0")
    frames_0 = spr_0 / "frames"
    frames_0.mkdir()
    (frames_0 / "frame_0.txt").write_text("texture_0")
    (frames_0 / "frame_1.txt").write_text("texture_1")
    
    # Sprite 1: spr_test_2
    spr_1 = sprites_dir / "spr_test_2"
    spr_1.mkdir()
    (spr_1 / "width").write_text("64")
    (spr_1 / "height").write_text("64")
    (spr_1 / "xoffset").write_text("32")
    (spr_1 / "yoffset").write_text("32")
    
    # Create objects directory
    objects_dir = dump_dir / "Objects"
    objects_dir.mkdir()
    
    # Object 0: obj_player
    obj_0 = objects_dir / "obj_player"
    obj_0.mkdir()
    (obj_0 / "sprite_index").write_text("0")
    (obj_0 / "solid").write_text("true")
    events_0 = obj_0 / "events"
    events_0.mkdir()
    (events_0 / "Create_0.gml").write_text("// Player creation code\nhp = 100;")
    (events_0 / "Step_0.gml").write_text("// Player step event\nspeed = 4;")
    
    # Object 1: obj_enemy
    obj_1 = objects_dir / "obj_enemy"
    obj_1.mkdir()
    (obj_1 / "sprite_index").write_text("1")
    (obj_1 / "solid").write_text("false")
    
    # Create rooms directory
    rooms_dir = dump_dir / "Rooms"
    rooms_dir.mkdir()
    
    # Room 0: rm_start
    rm_0 = rooms_dir / "rm_start"
    rm_0.mkdir()
    (rm_0 / "width").write_text("800")
    (rm_0 / "height").write_text("600")
    instances_0 = [
        {"x": 100, "y": 100, "objectId": 0},
        {"x": 400, "y": 300, "objectId": 1},
    ]
    (rm_0 / "instances.json").write_text(json.dumps(instances_0, indent=2))
    
    # Room 1: rm_level_1
    rm_1 = rooms_dir / "rm_level_1"
    rm_1.mkdir()
    (rm_1 / "width").write_text("1024")
    (rm_1 / "height").write_text("768")
    
    # Create sounds directory
    sounds_dir = dump_dir / "Sounds"
    sounds_dir.mkdir()
    
    # Sound 0: snd_jump
    snd_0 = sounds_dir / "snd_jump"
    snd_0.mkdir()
    (snd_0 / "snd_jump.ogg").touch()
    
    # Sound 1: snd_music
    snd_1 = sounds_dir / "snd_music"
    snd_1.mkdir()
    (snd_1 / "snd_music.ogg").touch()


def test_basic_conversion():
    """Test basic conversion of test dump"""
    print("=" * 60)
    print("TEST 1: Basic UMT Dump Conversion")
    print("=" * 60)
    
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_path = Path(temp_dir)
        dump_dir = temp_path / "test_dump"
        output_json = temp_path / "output.json"
        
        # Create test dump
        print("Creating test dump structure...")
        create_test_dump(dump_dir)
        
        # Convert
        print("Converting dump to JSON...")
        converter = UTConverter(str(dump_dir), verbose=True)
        converter.save_json(str(output_json))
        
        # Verify output
        print("\nVerifying output...")
        with open(output_json, 'r') as f:
            data = json.load(f)
        
        # Check structure
        assert 'textures' in data, "Missing 'textures' key"
        assert 'sprites' in data, "Missing 'sprites' key"
        assert 'objects' in data, "Missing 'objects' key"
        assert 'rooms' in data, "Missing 'rooms' key"
        assert 'sounds' in data, "Missing 'sounds' key"
        
        # Check counts
        print(f"  Textures: {len(data['textures'])} (expected: 2)")
        print(f"  Sprites: {len(data['sprites'])} (expected: 2)")
        print(f"  Objects: {len(data['objects'])} (expected: 2)")
        print(f"  Rooms: {len(data['rooms'])} (expected: 2)")
        print(f"  Sounds: {len(data['sounds'])} (expected: 2)")
        
        assert len(data['textures']) == 2, "Incorrect texture count"
        assert len(data['sprites']) == 2, "Incorrect sprite count"
        assert len(data['objects']) == 2, "Incorrect object count"
        assert len(data['rooms']) == 2, "Incorrect room count"
        assert len(data['sounds']) == 2, "Incorrect sound count"
        
        # Check sprite details
        sprite_0 = data['sprites'][0]
        print(f"\n  Sprite 0: {sprite_0['name']}")
        print(f"    Size: {sprite_0['width']}x{sprite_0['height']}")
        print(f"    Offset: ({sprite_0['x_offset']}, {sprite_0['y_offset']})")
        print(f"    Frames: {len(sprite_0['frames'])}")
        
        assert sprite_0['name'] == 'spr_test_1', "Incorrect sprite name"
        assert sprite_0['width'] == 32, "Incorrect sprite width"
        assert sprite_0['height'] == 32, "Incorrect sprite height"
        assert len(sprite_0['frames']) == 2, "Incorrect frame count"
        
        # Check object details
        obj_0 = data['objects'][0]
        print(f"\n  Object 0: {obj_0['name']}")
        print(f"    Sprite ID: {obj_0['sprite_id']}")
        print(f"    Solid: {obj_0['solid']}")
        print(f"    Events: {len(obj_0['events'])}")
        
        assert obj_0['name'] == 'obj_player', "Incorrect object name"
        assert obj_0['sprite_id'] == 0, "Incorrect sprite reference"
        assert obj_0['solid'] == True, "Incorrect solid flag"
        assert len(obj_0['events']) > 0, "Missing events"
        
        # Check room details
        room_0 = data['rooms'][0]
        print(f"\n  Room 0: {room_0['name']}")
        print(f"    Size: {room_0['width']}x{room_0['height']}")
        print(f"    Instances: {len(room_0['instances'])}")
        
        assert room_0['name'] == 'rm_start', "Incorrect room name"
        assert room_0['width'] == 800, "Incorrect room width"
        assert room_0['height'] == 600, "Incorrect room height"
        assert len(room_0['instances']) == 2, "Incorrect instance count"
        
        # Check sound details
        sound_0 = data['sounds'][0]
        print(f"\n  Sound 0: {sound_0['name']}")
        print(f"    File: {sound_0['file_path']}")
        
        assert sound_0['name'] == 'snd_jump', "Incorrect sound name"
        
        print("\n✓ All assertions passed!")
        return True


def test_error_handling():
    """Test error handling with missing/empty dump"""
    print("\n" + "=" * 60)
    print("TEST 2: Error Handling")
    print("=" * 60)
    
    # Test with non-existent directory
    print("Testing with non-existent directory...")
    try:
        converter = UTConverter("/non/existent/path", verbose=False)
        print("✗ Should have raised FileNotFoundError")
        return False
    except FileNotFoundError as e:
        print(f"✓ Correctly caught error: {e}")
    
    # Test with empty dump directory
    print("Testing with empty dump directory...")
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_path = Path(temp_dir)
        converter = UTConverter(str(temp_path), verbose=True)
        result = converter.convert()
        
        assert result['sprites'] == [], "Should have no sprites"
        assert result['objects'] == [], "Should have no objects"
        assert result['rooms'] == [], "Should have no rooms"
        assert result['sounds'] == [], "Should have no sounds"
        
        print("✓ Empty dump handled correctly")
    
    return True


def test_json_compatibility():
    """Test that output JSON is compatible with C++ loader schema"""
    print("\n" + "=" * 60)
    print("TEST 3: JSON Schema Compatibility")
    print("=" * 60)
    
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_path = Path(temp_dir)
        dump_dir = temp_path / "test_dump"
        output_json = temp_path / "output.json"
        
        create_test_dump(dump_dir)
        converter = UTConverter(str(dump_dir), verbose=False)
        converter.save_json(str(output_json))
        
        with open(output_json, 'r') as f:
            data = json.load(f)
        
        # Validate JSON schema
        print("Checking JSON schema compatibility...")
        
        # Check sprite schema
        if data['sprites']:
            sprite = data['sprites'][0]
            required_fields = ['id', 'name', 'width', 'height', 'frames']
            for field in required_fields:
                assert field in sprite, f"Sprite missing required field: {field}"
            print("✓ Sprite schema valid")
        
        # Check object schema
        if data['objects']:
            obj = data['objects'][0]
            required_fields = ['id', 'name', 'sprite_id']
            for field in required_fields:
                assert field in obj, f"Object missing required field: {field}"
            print("✓ Object schema valid")
        
        # Check room schema
        if data['rooms']:
            room = data['rooms'][0]
            required_fields = ['id', 'name', 'width', 'height', 'instances']
            for field in required_fields:
                assert field in room, f"Room missing required field: {field}"
            print("✓ Room schema valid")
        
        # Check sound schema
        if data['sounds']:
            sound = data['sounds'][0]
            required_fields = ['id', 'name']
            for field in required_fields:
                assert field in sound, f"Sound missing required field: {field}"
            print("✓ Sound schema valid")
        
        return True


def run_all_tests():
    """Run all tests"""
    print("\n" + "=" * 60)
    print("UMT to JSON Converter - Test Suite")
    print("=" * 60 + "\n")
    
    tests = [
        ("Basic Conversion", test_basic_conversion),
        ("Error Handling", test_error_handling),
        ("JSON Compatibility", test_json_compatibility),
    ]
    
    results = []
    for name, test_func in tests:
        try:
            result = test_func()
            results.append((name, result))
        except AssertionError as e:
            print(f"\n✗ Test failed: {e}")
            results.append((name, False))
        except Exception as e:
            print(f"\n✗ Error: {e}")
            results.append((name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    for name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{status}: {name}")
    
    passed = sum(1 for _, r in results if r)
    total = len(results)
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    return all(r for _, r in results)


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
