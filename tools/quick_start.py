#!/usr/bin/env python3
"""
Quick Start Guide - Extract Real GameMaker Games

This script provides guided extraction of real GameMaker games like
Undertale and Deltarune to JSON format compatible with the C++ loader.

Usage:
    python quick_start.py
"""

import os
import sys
from pathlib import Path
import subprocess


def find_undertale():
    """Find Undertale installation"""
    possible_locations = [
        Path.home() / "AppData/Local/Packages/TobyFox.Undertale_kxnztxnf628xy/LocalState",
        Path("C:/Games/Undertale"),
        Path("C:/Program Files/Undertale"),
        Path("C:/Program Files (x86)/Undertale"),
        Path.home() / "Downloads/Undertale",
    ]
    
    for loc in possible_locations:
        data_win = loc / "data.win"
        if data_win.exists():
            return str(data_win)
    
    return None


def find_deltarune():
    """Find Deltarune installation"""
    possible_locations = [
        Path.home() / "AppData/Local/Packages/TobyFox.Deltarune_xhf8p5n57j8k0/LocalState",
        Path("C:/Games/Deltarune"),
        Path("C:/Program Files/Deltarune"),
        Path.home() / "Downloads/Deltarune",
    ]
    
    for loc in possible_locations:
        data_win = loc / "data.win"
        if data_win.exists():
            return str(data_win)
    
    return None


def extract_game(data_win_path, output_name):
    """Extract game using the extraction tool"""
    print(f"\n{'='*70}")
    print(f"Extracting {output_name}...")
    print(f"{'='*70}")
    print(f"Source: {data_win_path}")
    
    output_json = f"{output_name}.json"
    cmd = [
        sys.executable, 
        "extract_game.py",
        data_win_path,
        output_json,
        "--keep-dump",
        f"dump_{output_name}/"
    ]
    
    print(f"Command: {' '.join(cmd)}\n")
    
    try:
        result = subprocess.run(cmd, cwd=str(Path(__file__).parent))
        if result.returncode == 0:
            print(f"\n✓ Extraction successful!")
            print(f"  Output JSON: {output_json}")
            print(f"  Debug dump: dump_{output_name}/")
            return True
        else:
            print(f"\n✗ Extraction failed with return code {result.returncode}")
            return False
    except FileNotFoundError:
        print(f"✗ extract_game.py not found in {Path(__file__).parent}")
        return False
    except Exception as e:
        print(f"✗ Error: {e}")
        return False


def main():
    """Interactive game extraction"""
    print("\n" + "="*70)
    print("GameMaker Game Extractor - Quick Start")
    print("="*70)
    print("\nThis tool extracts GameMaker games to JSON format")
    print("Compatible with: Undertale, Deltarune, and other GameMaker titles\n")
    
    tools_dir = Path(__file__).parent
    
    # Check for Python and requirements
    try:
        import json
    except ImportError:
        print("✗ Error: Python standard library not available")
        sys.exit(1)
    
    # Try auto-detection
    print("Searching for installed games...\n")
    
    games_found = {}
    
    undertale_path = find_undertale()
    if undertale_path:
        games_found['Undertale'] = undertale_path
        print(f"✓ Found Undertale: {undertale_path}")
    else:
        print("✗ Undertale not found in default locations")
    
    deltarune_path = find_deltarune()
    if deltarune_path:
        games_found['Deltarune'] = deltarune_path
        print(f"✓ Found Deltarune: {deltarune_path}")
    else:
        print("✗ Deltarune not found in default locations")
    
    if not games_found:
        print("\n" + "="*70)
        print("No games found in default locations")
        print("="*70)
        print("\nPlease specify manually:")
        print("  1. Find your game's data.win file:")
        print("     - Windows Store games: AppData/Local/Packages/...")
        print("     - Steam: SteamLibrary/steamapps/common/...")
        print("     - GOG: Program Files/GOG Games/...")
        print("\n  2. Run extraction:")
        print("     python extract_game.py <path/to/data.win> output.json")
        print("\n" + "="*70)
        sys.exit(1)
    
    # Interactive extraction
    print("\n" + "="*70)
    print("Select games to extract (or 'all'):")
    print("="*70)
    
    game_list = list(games_found.items())
    for i, (name, path) in enumerate(game_list, 1):
        print(f"{i}. {name}")
    print(f"{len(game_list)+1}. All games")
    print(f"{len(game_list)+2}. Custom path")
    
    choice = input("\nEnter selection (1-{}): ".format(len(game_list)+2)).strip()
    
    games_to_extract = []
    
    try:
        choice_num = int(choice)
        if 1 <= choice_num <= len(game_list):
            games_to_extract = [game_list[choice_num-1]]
        elif choice_num == len(game_list) + 1:
            games_to_extract = game_list
        elif choice_num == len(game_list) + 2:
            custom_path = input("Enter path to data.win: ").strip()
            if Path(custom_path).exists():
                custom_name = input("Enter game name: ").strip() or "custom_game"
                games_to_extract = [(custom_name, custom_path)]
            else:
                print("✗ File not found")
                sys.exit(1)
    except ValueError:
        print("✗ Invalid selection")
        sys.exit(1)
    
    # Extract selected games
    results = {}
    for name, path in games_to_extract:
        success = extract_game(path, name.lower())
        results[name] = success
    
    # Summary
    print("\n" + "="*70)
    print("Extraction Summary")
    print("="*70)
    
    for name, success in results.items():
        status = "✓ Success" if success else "✗ Failed"
        print(f"{status}: {name}")
    
    successful = sum(1 for s in results.values() if s)
    print(f"\nTotal: {successful}/{len(results)} games extracted successfully")
    
    if successful > 0:
        print("\n" + "="*70)
        print("Next Steps")
        print("="*70)
        print("\n1. Use the JSON with C++ loader:")
        print("   #include \"AssetLoader.h\"")
        print("   GM::AssetLoader loader;")
        print("   loader.LoadGameFromJSON(\"game.json\");")
        print("\n2. Or inspect the JSON:")
        print("   type *.json | more  # Windows")
        print("   cat *.json          # Linux/Mac")
        print("\n3. Check the debug dump for troubleshooting:")
        print("   dir dump_*/ | head -20")
        print("="*70 + "\n")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nAborted by user")
        sys.exit(0)
    except Exception as e:
        print(f"\n✗ Error: {e}")
        sys.exit(1)
