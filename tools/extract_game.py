#!/usr/bin/env python3
"""
GameMaker Game Extractor

Complete pipeline to extract GameMaker games from data.win files and convert
them to JSON format compatible with the C++ asset loader.

This supports real games like Undertale, Deltarune, and other GameMaker titles.

Usage:
    python extract_game.py <data.win_path> <output_json> [--umt-path PATH]
    
Examples:
    # Extract Undertale with default UMT path
    python extract_game.py "C:/Games/Undertale/data.win" undertale.json
    
    # Extract Deltarune with custom UMT path
    python extract_game.py deltarune/data.win deltarune.json --umt-path "C:/Tools/UMT/UndertaleModTool.exe"
    
    # Extract and save dump directory for inspection
    python extract_game.py "C:/Games/Game/data.win" game.json --keep-dump dump_output/
"""

import os
import sys
import subprocess
import shutil
import tempfile
import json
from pathlib import Path
from typing import Optional, Tuple
import argparse


class GameExtractor:
    """Extracts GameMaker games and converts them to JSON"""
    
    def __init__(self, umt_path: Optional[str] = None, verbose: bool = True):
        self.verbose = verbose
        self.umt_path = self._locate_umt(umt_path)
        
        if not self.umt_path:
            raise RuntimeError("UndertaleModTool not found. Please specify --umt-path or install UMT.")
    
    def log(self, message: str):
        if self.verbose:
            print(f"[Game Extractor] {message}")
    
    def _locate_umt(self, custom_path: Optional[str]) -> Optional[str]:
        """Locate UMT executable"""
        if custom_path:
            path = Path(custom_path)
            if path.exists():
                self.log(f"Using custom UMT: {path}")
                return str(path)
            else:
                self.log(f"Custom UMT path not found: {path}")
                return None
        
        # Try common installation locations
        common_paths = [
            Path(os.path.expandvars("%PROGRAMFILES%/UMT")),
            Path(os.path.expandvars("%PROGRAMFILES(X86)%/UMT")),
            Path.home() / "Downloads" / "UMT",
            Path("C:/Tools/UMT"),
            Path("./UMT"),
        ]
        
        # Also check PATH
        umt = shutil.which("UndertaleModTool.exe")
        if umt:
            self.log(f"Found UMT in PATH: {umt}")
            return umt
        
        # Check common paths
        for path in common_paths:
            umt_exe = path / "UndertaleModTool.exe"
            if umt_exe.exists():
                self.log(f"Found UMT at: {umt_exe}")
                return str(umt_exe)
        
        return None
    
    def extract(self, data_win_path: str, output_json: str, 
                keep_dump: Optional[str] = None) -> Tuple[bool, str]:
        """
        Extract game from data.win and convert to JSON
        
        Args:
            data_win_path: Path to data.win file
            output_json: Output JSON file path
            keep_dump: Optional directory to keep the UMT dump for inspection
        
        Returns:
            (success: bool, message: str)
        """
        data_win = Path(data_win_path)
        if not data_win.exists():
            return False, f"File not found: {data_win_path}"
        
        self.log(f"Extracting: {data_win}")
        
        # Create temporary directory for UMT dump
        with tempfile.TemporaryDirectory() as temp_dir:
            dump_dir = Path(temp_dir) / "dump"
            dump_dir.mkdir()
            
            # Run UMT dump
            success, message = self._run_umt_dump(data_win, dump_dir)
            if not success:
                return False, message
            
            # Optionally save dump directory
            if keep_dump:
                keep_path = Path(keep_dump)
                keep_path.mkdir(parents=True, exist_ok=True)
                self.log(f"Saving dump to: {keep_path}")
                
                for item in dump_dir.iterdir():
                    if item.is_dir():
                        shutil.copytree(item, keep_path / item.name, dirs_exist_ok=True)
                    else:
                        shutil.copy2(item, keep_path)
            
            # Convert to JSON
            success, message = self._convert_dump_to_json(dump_dir, output_json)
            return success, message
    
    def _run_umt_dump(self, data_win: Path, output_dir: Path) -> Tuple[bool, str]:
        """Run UMT dump command"""
        self.log(f"Running UMT dump...")
        self.log(f"  Input: {data_win}")
        self.log(f"  Output: {output_dir}")
        
        try:
            # UMT dump format: UMT.exe dump <input> <output>
            cmd = [str(self.umt_path), "dump", str(data_win), str(output_dir)]
            
            self.log(f"  Command: {' '.join(cmd)}")
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout
            )
            
            if result.returncode != 0:
                error_msg = result.stderr or result.stdout or "Unknown error"
                return False, f"UMT dump failed: {error_msg}"
            
            self.log("UMT dump completed successfully")
            return True, "OK"
        
        except subprocess.TimeoutExpired:
            return False, "UMT dump timed out (5 minutes)"
        except Exception as e:
            return False, f"Error running UMT: {e}"
    
    def _convert_dump_to_json(self, dump_dir: Path, output_json: str) -> Tuple[bool, str]:
        """Convert UMT dump to JSON"""
        self.log("Converting dump to JSON...")
        
        try:
            # Import the converter module
            import sys
            import importlib.util
            
            converter_path = Path(__file__).parent / "umt_to_json_converter.py"
            
            if not converter_path.exists():
                return False, f"Converter script not found: {converter_path}"
            
            # Load converter module
            spec = importlib.util.spec_from_file_location("umt_converter", converter_path)
            converter_module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(converter_module)
            
            # Run conversion
            converter = converter_module.UTConverter(str(dump_dir), verbose=self.verbose)
            converter.save_json(output_json)
            
            self.log(f"JSON conversion successful: {output_json}")
            return True, "OK"
        
        except Exception as e:
            return False, f"Conversion failed: {e}"


def main():
    parser = argparse.ArgumentParser(
        description="Extract GameMaker games from data.win files and convert to JSON",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python extract_game.py "C:/Games/Undertale/data.win" undertale.json
  python extract_game.py deltarune/data.win deltarune.json --umt-path "C:/Tools/UMT/UndertaleModTool.exe"
  python extract_game.py data.win output.json --keep-dump ./dump_debug/
        """
    )
    
    parser.add_argument("data_win", help="Path to data.win file")
    parser.add_argument("output_json", help="Output JSON file path")
    parser.add_argument("--umt-path", help="Path to UndertaleModTool.exe")
    parser.add_argument("--keep-dump", help="Directory to save UMT dump for inspection")
    parser.add_argument("--quiet", action="store_true", help="Suppress output")
    
    args = parser.parse_args()
    
    try:
        extractor = GameExtractor(umt_path=args.umt_path, verbose=not args.quiet)
        success, message = extractor.extract(
            args.data_win,
            args.output_json,
            keep_dump=args.keep_dump
        )
        
        if success:
            print(f"✓ Extraction successful!")
            print(f"  Output: {args.output_json}")
            if args.keep_dump:
                print(f"  Dump: {args.keep_dump}")
            sys.exit(0)
        else:
            print(f"✗ Extraction failed: {message}", file=sys.stderr)
            sys.exit(1)
    
    except Exception as e:
        print(f"✗ Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
