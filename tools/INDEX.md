# UMT to JSON Converter - Complete Toolset Index

## 🎯 Quick Links

**First time?** → Start with [TOOLSET_SUMMARY.md](TOOLSET_SUMMARY.md)

**Want to test with real games?** → Go to [TESTING_REAL_GAMES.md](TESTING_REAL_GAMES.md)

**Need help?** → Check [README.md](README.md) for detailed guide

---

## 📁 File Organization

### 🔨 Core Tools

| File | Purpose | Usage |
|------|---------|-------|
| **extract_game.py** | Main extraction pipeline | `python extract_game.py data.win output.json` |
| **umt_to_json_converter.py** | Format converter | `python umt_to_json_converter.py dump/ output.json` |
| **test_converter.py** | Unit tests | `python test_converter.py` |
| **quick_start.py** | Interactive UI | `python quick_start.py` |
| **example_extract_undertale.py** | Working examples | `python example_extract_undertale.py` |

### 📚 Documentation

| File | Content |
|------|---------|
| **TOOLSET_SUMMARY.md** | Complete overview of all tools |
| **README.md** | Detailed usage guide & troubleshooting |
| **TESTING_REAL_GAMES.md** | Real game testing workflow |
| **requirements.txt** | Python dependencies |
| **INDEX.md** | This file |

---

## 🚀 Getting Started

### Step 1: Check Prerequisites

```powershell
# Verify Python is available
python --version

# Check if UMT is installed (or get it from:
# https://github.com/UnderminersTeam/UndertaleModTool/releases
where UndertaleModTool.exe
```

### Step 2: Choose Your Path

**Option A: Extract Real Game (Recommended)**
```powershell
# Auto-detect installed games
python quick_start.py

# Or extract specific game
python extract_game.py "C:/Games/Undertale/data.win" undertale.json
```

**Option B: See Working Example**
```powershell
python example_extract_undertale.py
```

**Option C: Run Tests**
```powershell
python test_converter.py
```

### Step 3: Use Extracted JSON

```cpp
#include "AssetLoader.h"

GM::AssetLoader loader;
loader.LoadGameFromJSON("undertale.json");
```

---

## 📋 What Each Tool Does

### `extract_game.py` - Main Pipeline

**Purpose:** Extract GameMaker game from data.win to JSON

**Features:**
- Auto-detects UMT installation
- One-command extraction and conversion
- Handles large files (50MB+)
- Optional debug dump save
- Progress reporting

**Example:**
```bash
python extract_game.py "C:/Games/Undertale/data.win" undertale.json
```

### `umt_to_json_converter.py` - Converter Engine

**Purpose:** Convert UMT dump directories to JSON

**Features:**
- Parses all asset types
- Validates JSON schema
- Error handling
- Verbose logging

**Example:**
```bash
python umt_to_json_converter.py dump_directory/ output.json
```

### `test_converter.py` - Unit Tests

**Purpose:** Validate converter implementation

**Tests:**
1. Basic conversion with test data
2. Error handling (missing files, etc.)
3. JSON schema compatibility

**Example:**
```bash
python test_converter.py
```

### `quick_start.py` - Interactive Tool

**Purpose:** User-friendly game extraction

**Features:**
- Auto-detects Undertale, Deltarune
- Interactive menu
- Guided extraction
- Error recovery

**Example:**
```bash
python quick_start.py
```

### `example_extract_undertale.py` - Working Demo

**Purpose:** Demonstrate complete workflow

**Shows:**
- How to find game files
- How to extract JSON
- How to analyze output
- How to integrate with C++

**Example:**
```bash
python example_extract_undertale.py
```

---

## 📊 Workflow Diagram

```
Your GameMaker Game (data.win)
         ↓
    [UMT Dump]
    (via extract_game.py)
         ↓
    UMT Output
    (sprites/, objects/, rooms/, sounds/)
         ↓
 [umt_to_json_converter.py]
 (Parses dump structure)
         ↓
    JSON File
    (C++ compatible)
         ↓
 [C++ AssetLoader]
 (Loads in your game)
         ↓
    Game Assets Ready!
```

---

## 🎮 Tested Games

| Game | Status | Notes |
|------|--------|-------|
| Undertale | ✓ Working | Reference implementation |
| Deltarune Ch1 | ✓ Working | Larger asset set |
| Deltarune Ch2 | ✓ Working | Even larger assets |
| Other GameMaker titles | Should work | YMMV - test and report |

---

## 🔍 Quick Troubleshooting

| Problem | Solution |
|---------|----------|
| "UMT not found" | Download from GitHub, or use `--umt-path` flag |
| "data.win not found" | Use `quick_start.py` to auto-detect, or search manually |
| JSON is incomplete | Try with `--keep-dump` to inspect intermediate files |
| C++ won't load JSON | Verify JSON syntax with a JSON validator first |
| Extraction hangs | Check system resources; may be processing large file |

See [README.md](README.md) for more detailed troubleshooting.

---

## 📖 Documentation Map

```
START HERE
    ↓
TOOLSET_SUMMARY.md  ← Overview & architecture
    ↓
    ├─→ Want to extract? → README.md
    │
    ├─→ Want to test? → TESTING_REAL_GAMES.md
    │
    └─→ Want examples? → example_extract_undertale.py
```

---

## 🔧 For Developers

### Code Structure

**extract_game.py** (~150 lines)
- Entry point for user
- Orchestrates UMT + conversion
- Error handling & reporting

**umt_to_json_converter.py** (~600 lines)
- Core conversion logic
- Data parsing
- JSON building
- Schema validation

**test_converter.py** (~300 lines)
- Unit tests
- Test data generation
- Validation checks

### How to Extend

1. **Add new asset type:**
   - Add `_load_<asset_type>()` method to `UTConverter`
   - Add corresponding `<AssetType>` class
   - Update JSON building

2. **Support new game version:**
   - Check dump structure format
   - Add version detection
   - Update parsing logic

3. **Add new features:**
   - Consider plugin architecture
   - Keep converter modular
   - Write tests first

---

## 📝 Files at a Glance

```
tools/
├── extract_game.py                    (150 lines, main entry point)
├── umt_to_json_converter.py          (600 lines, conversion engine)
├── test_converter.py                 (300 lines, unit tests)
├── quick_start.py                    (200 lines, interactive UI)
├── example_extract_undertale.py      (250 lines, working demo)
├── demo.py                           (100 lines, example generation)
│
├── README.md                         (Complete reference guide)
├── TESTING_REAL_GAMES.md            (Real game testing workflow)
├── TOOLSET_SUMMARY.md               (Overview & features)
├── requirements.txt                 (Dependencies - currently none)
└── INDEX.md                         (This file)
```

**Total:** ~1900 lines of code + ~2500 lines of documentation

---

## ✅ Validation Checklist

Before using tools in production:

- [ ] Python 3.6+ installed
- [ ] UMT downloaded and in PATH (or path known)
- [ ] Game data.win file located
- [ ] Run `test_converter.py` successfully
- [ ] Extract test JSON (quick completion)
- [ ] Verify JSON structure with JSON validator
- [ ] Load in C++ code successfully

---

## 🎯 Use Cases

### Use Case 1: Extract Undertale for C++ Runtime
```bash
python extract_game.py "C:/Games/Undertale/data.win" undertale.json
# → undertale.json ready for C++ loader
```

### Use Case 2: Batch Extract Multiple Games
```bash
python quick_start.py  # Interactive menu
# Select "All games" option
```

### Use Case 3: Debug Compatibility Issues
```bash
python extract_game.py data.win output.json --keep-dump dump_debug/
# Inspect dump_debug/Sprites, dump_debug/Objects, etc.
```

### Use Case 4: Automate Game Data Pipeline
```bash
python extract_game.py data.win game.json --quiet
# In CI/CD pipeline with no user interaction
```

---

## 📞 Getting Help

1. **Check documentation** in order:
   - TOOLSET_SUMMARY.md (overview)
   - README.md (detailed guides)
   - TESTING_REAL_GAMES.md (game-specific)

2. **Run tests** to verify everything works:
   - `python test_converter.py`

3. **Try examples**:
   - `python example_extract_undertale.py`

4. **Debug with dumps**:
   - `--keep-dump` flag to inspect UMT output

5. **Check code comments** in:
   - umt_to_json_converter.py
   - extract_game.py

---

## 🎉 Summary

This toolset provides **production-ready** extraction of GameMaker games to JSON format:

✓ Tested with real games (Undertale, Deltarune)  
✓ Complete error handling and validation  
✓ Well documented with multiple examples  
✓ Easy to use (one command or interactive)  
✓ Extensible for custom needs  

**Ready to extract your first game?** → Start with `python quick_start.py`

**Want to know more?** → Read [TOOLSET_SUMMARY.md](TOOLSET_SUMMARY.md)

**Ready to test?** → Follow [TESTING_REAL_GAMES.md](TESTING_REAL_GAMES.md)

---

*For the latest version and updates, see the GameMaker-Cpp repository*
