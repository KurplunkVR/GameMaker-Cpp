#!/usr/bin/env python3
"""
Quick demo of the UMT converter showing expected output

This demonstrates what the converter produces when run on real game data.
"""

import json
from pathlib import Path


# Example output from converting Undertale-like game
UNDERTALE_EXAMPLE = {
    "game": {
        "name": "undertale",
        "version": "1.0"
    },
    "textures": [
        "texture_0.png",
        "texture_1.png", 
        "texture_2.png",
        "texture_3.png"
    ],
    "sprites": [
        {
            "id": 0,
            "name": "spr_player_idle",
            "width": 32,
            "height": 32,
            "x_offset": 16,
            "y_offset": 16,
            "collision_type": 0,
            "frames": [
                {"texture_id": 0, "duration": 1.0},
                {"texture_id": 1, "duration": 1.0}
            ],
            "playback_speed": 12.0,
            "playback_speed_type": 1
        },
        {
            "id": 1,
            "name": "spr_player_walk",
            "width": 32,
            "height": 32,
            "x_offset": 16,
            "y_offset": 16,
            "collision_type": 0,
            "frames": [
                {"texture_id": 2, "duration": 0.5},
                {"texture_id": 3, "duration": 0.5}
            ],
            "playback_speed": 12.0,
            "playback_speed_type": 1
        }
    ],
    "objects": [
        {
            "id": 0,
            "name": "obj_player",
            "sprite_id": 0,
            "solid": True,
            "events": {
                "Create": "speed = 0; direction = 0; hp = 20; inv = 0;",
                "Step": "if (keyboard_check(vk_left)) direction = 180; if (keyboard_check(vk_right)) direction = 0; if (keyboard_check(vk_up)) direction = 90; if (keyboard_check(vk_down)) direction = 270;",
                "Draw": "draw_self();"
            },
            "creation_code": ""
        },
        {
            "id": 1,
            "name": "obj_enemy",
            "sprite_id": 1,
            "solid": True,
            "events": {
                "Create": "speed = 1; direction = 0; hp = 10;",
                "Step": "direction = point_direction(x, y, obj_player.x, obj_player.y);",
                "Collision": "if (other.object_index == obj_player) other.hp -= 1;"
            },
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
                {
                    "x": 400,
                    "y": 300,
                    "object_id": 0,
                    "creation_code": ""
                },
                {
                    "x": 200,
                    "y": 200,
                    "object_id": 1,
                    "creation_code": "hp = 15;"
                },
                {
                    "x": 600,
                    "y": 200,
                    "object_id": 1,
                    "creation_code": "hp = 15;"
                }
            ],
            "background_index": -1,
            "clear_buffer_color": 4294967295
        }
    ],
    "sounds": [
        {
            "id": 0,
            "name": "snd_jump",
            "sound_type": 2,
            "file_path": "snd_jump.ogg",
            "volume": 1.0,
            "pitch": 1.0
        },
        {
            "id": 1,
            "name": "snd_battle",
            "sound_type": 1,
            "file_path": "snd_battle.ogg",
            "volume": 0.8,
            "pitch": 1.0
        }
    ]
}


def main():
    """Generate example output files"""
    
    # Create examples directory
    examples_dir = Path(__file__).parent / "examples"
    examples_dir.mkdir(exist_ok=True)
    
    # Save Undertale example
    with open(examples_dir / "undertale_example.json", 'w') as f:
        json.dump(UNDERTALE_EXAMPLE, f, indent=2)
    
    print("=" * 70)
    print("UMT Converter - Example Output Generation")
    print("=" * 70)
    print()
    print("Generated example outputs:")
    print(f"  ✓ examples/undertale_example.json")
    print()
    print("These examples show the expected JSON format from the converter")
    print("when run on real GameMaker games like Undertale.")
    print()
    print("=" * 70)
    print("Converter Usage")
    print("=" * 70)
    print()
    print("1. Extract and convert in one step:")
    print("   python extract_game.py \"C:/Games/Undertale/data.win\" undertale.json")
    print()
    print("2. Just convert existing UMT dump:")
    print("   python umt_to_json_converter.py dump_directory/ output.json")
    print()
    print("3. Run tests:")
    print("   python test_converter.py")
    print()
    print("=" * 70)


if __name__ == "__main__":
    main()
