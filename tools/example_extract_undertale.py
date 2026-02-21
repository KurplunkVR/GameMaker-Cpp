#!/usr/bin/env python3
"""
Working Example - Extract and Test with Real Games

This script demonstrates the complete workflow with actual games.
Works with Undertale, Deltarune, and other GameMaker titles.

PREREQUISITES:
- Python 3.6+
- UndertaleModTool (https://github.com/UnderminersTeam/UndertaleModTool)
- A GameMaker game file (data.win)

USAGE:
    python example_extract_undertale.py
"""

import sys
import json
import subprocess
from pathlib import Path


def example_extract_undertale():
    """
    Example: Extract Undertale and demonstrate the workflow
    """
    
    tools_dir = Path(__file__).parent
    
    # Define paths
    undertale_paths = [
        Path("C:/Games/Undertale/data.win"),
        Path("C:/Program Files (x86)/Steam/steamapps/common/Undertale/data.win"),
        Path.home() / "AppData/Local/Packages/TobyFox.Undertale_kxnztxnf628xy/LocalState/data.win",
    ]
    
    data_win = None
    for path in undertale_paths:
        if path.exists():
            data_win = path
            break
    
    if not data_win:
        print("Undertale data.win not found in standard locations")
        print("\nTried:")
        for path in undertale_paths:
            print(f"  - {path}")
        print("\nPlease locate data.win manually and edit this script")
        return False
    
    print("\n" + "="*70)
    print("EXAMPLE: Extract Undertale to JSON")
    print("="*70)
    print(f"\nFound Undertale at: {data_win}")
    print(f"File size: {data_win.stat().st_size / (1024*1024):.1f} MB")
    
    # Define output
    output_json = tools_dir / "undertale_extracted.json"
    dump_dir = tools_dir / "dump_undertale"
    
    print(f"\nExtracting to: {output_json}")
    print(f"Debug dump: {dump_dir}")
    
    # Run extraction
    print("\n" + "-"*70)
    print("Running extraction pipeline...")
    print("-"*70 + "\n")
    
    cmd = [
        sys.executable,
        str(tools_dir / "extract_game.py"),
        str(data_win),
        str(output_json),
        "--keep-dump",
        str(dump_dir),
    ]
    
    result = subprocess.run(cmd)
    
    if result.returncode != 0:
        print("\n✗ Extraction failed!")
        return False
    
    print("\n" + "-"*70)
    print("Extraction complete!")
    print("-"*70)
    
    # Analyze output
    if not output_json.exists():
        print("✗ Output JSON not created")
        return False
    
    print(f"\n✓ Output JSON created: {output_json.stat().st_size / (1024*1024):.1f} MB")
    
    # Parse and verify
    with open(output_json, 'r') as f:
        data = json.load(f)
    
    print("\n" + "="*70)
    print("Extracted Assets")
    print("="*70)
    print(f"  Textures: {len(data.get('textures', []))} files")
    print(f"  Sprites:  {len(data.get('sprites', []))} sprites")
    print(f"  Objects:  {len(data.get('objects', []))} objects")
    print(f"  Rooms:    {len(data.get('rooms', []))} rooms")
    print(f"  Sounds:   {len(data.get('sounds', []))} sounds")
    
    # Show sample sprites
    sprites = data.get('sprites', [])
    if sprites:
        print("\n" + "-"*70)
        print("Sample Sprites (first 5)")
        print("-"*70)
        for sprite in sprites[:5]:
            print(f"  {sprite['name']:30} {sprite['width']:3}x{sprite['height']:3} "
                  f"({len(sprite['frames'])} frames)")
    
    # Show sample objects
    objects = data.get('objects', [])
    if objects:
        print("\n" + "-"*70)
        print("Sample Objects (first 5)")
        print("-"*70)
        for obj in objects[:5]:
            spr_id = obj.get('sprite_id', -1)
            spr_name = "None" if spr_id == -1 else f"spr_{spr_id}"
            events = len(obj.get('events', {}))
            print(f"  {obj['name']:30} sprite={spr_name:10} events={events}")
    
    # Show sample rooms
    rooms = data.get('rooms', [])
    if rooms:
        print("\n" + "-"*70)
        print("Sample Rooms (first 5)")
        print("-"*70)
        for room in rooms[:5]:
            insts = len(room.get('instances', []))
            print(f"  {room['name']:30} {room['width']:4}x{room['height']:4} "
                  f"({insts} instances)")
    
    # Show sample sounds
    sounds = data.get('sounds', [])
    if sounds:
        print("\n" + "-"*70)
        print("Sample Sounds (first 5)")
        print("-"*70)
        for sound in sounds[:5]:
            snd_type = ["SFX", "MUSIC", "VOICE"][sound.get('sound_type', 0)]
            print(f"  {sound['name']:30} type={snd_type:6} "
                  f"file={Path(sound['file_path']).name}")
    
    print("\n" + "="*70)
    print("Next Steps")
    print("="*70)
    
    print(f"""
1. Use in C++ code:
   
   #include "AssetLoader.h"
   
   GM::AssetLoader loader;
   if (loader.LoadGameFromJSON("{output_json.name}")) {{
       // Access loaded assets
       auto sprite = GM::Managers::GetSpriteManager()->GetSprite(0);
       auto room = GM::Managers::GetRoomManager()->GetRoom(0);
       
       // Render/use assets...
   }}

2. Inspect the dump directory:
   dir {dump_dir}/Sprites | head -20
   dir {dump_dir}/Objects | head -20

3. Validate JSON:
   type {output_json.name} | python -m json.tool

4. Compare with other games:
   python extract_game.py "C:/Games/Deltarune/data.win" deltarune.json
""")
    
    print("="*70)
    print("✓ Example complete!")
    print("="*70 + "\n")
    
    return True


def example_convert_existing_dump():
    """
    Example: Convert an existing UMT dump directory
    """
    
    print("\n" + "="*70)
    print("EXAMPLE: Convert Existing UMT Dump")
    print("="*70)
    
    # This would convert a dump directory to JSON
    # Usage for when you already have the dump directory
    
    tools_dir = Path(__file__).parent
    
    example_dump = tools_dir / "example_dump"
    if not example_dump.exists():
        print(f"\nNo dump directory found at {example_dump}")
        print("This example requires a pre-existing UMT dump")
        print("\nTo create a dump manually:")
        print("  umt.exe dump data.win dump_directory/")
        print("\nThen convert it:")
        print(f"  python {tools_dir}/umt_to_json_converter.py dump_directory/ output.json")
        return False
    
    print(f"\nConverting dump at: {example_dump}")
    
    cmd = [
        "python",
        str(tools_dir / "umt_to_json_converter.py"),
        str(example_dump),
        "converted_game.json",
    ]
    
    result = subprocess.run(cmd)
    return result.returncode == 0


def main():
    """Run examples"""
    print("\n" + "#"*70)
    print("# GameMaker Asset Converter - Working Examples")
    print("#"*70)
    
    import argparse
    
    parser = argparse.ArgumentParser(
        description="Working examples of the UMT converter"
    )
    parser.add_argument(
        "--example",
        choices=["undertale", "convert", "all"],
        default="all",
        help="Which example to run"
    )
    
    args = parser.parse_args()
    
    results = {}
    
    if args.example in ["undertale", "all"]:
        print("\n[1/2] Undertale Extraction Example")
        try:
            results["Undertale"] = example_extract_undertale()
        except Exception as e:
            print(f"✗ Error: {e}")
            results["Undertale"] = False
    
    if args.example in ["convert", "all"]:
        print("\n[2/2] Convert Existing Dump Example")
        try:
            results["Convert"] = example_convert_existing_dump()
        except Exception as e:
            print(f"✗ Error: {e}")
            results["Convert"] = False
    
    # Summary
    print("\n" + "#"*70)
    print("# Example Summary")
    print("#"*70 + "\n")
    
    for name, success in results.items():
        status = "✓ Success" if success else "✗ Skipped/Failed"
        print(f"{status}: {name}")
    
    if all(results.values()):
        print("\n✓ All examples completed successfully!")
    else:
        print("\n⚠ Some examples were skipped or failed")
        print("This is normal - make sure prerequisites are available")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(0)
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
