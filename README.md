# GameMaker-Cpp

A cross-platform C++ GameMaker runtime with a complete GML virtual machine, SDL3 rendering, and bytecode execution. This project implements a stack-based VM interpreter to execute compiled GameMaker bytecode, enabling portable game execution across desktop and consoles.

Yes, Generative AI helped build the engine. Yes, it was all checked over by an actual programmer (ME). And yes, it made tons of mistakes—they're all fixed though.

## Architecture

### GML Virtual Machine ✨ NEW
- **Stack-based bytecode interpreter** with 40+ opcodes
- **Dynamically-typed value system** (real, string, bool, undefined)
- **Complete instruction support**: arithmetic, logical, bitwise, comparisons, control flow
- **Function calls** with execution frames and local/global variables
- **Built-in functions**: math (sin, cos, sqrt, etc.), string operations, type conversions

### Rendering & Platform
- **SDL3** for cross-platform window management and rendering
- **SDL_Renderer** for 2D graphics (60 FPS target)
- Abstracted platform layer for future console/mobile ports

## Build Instructions

### Prerequisites
- CMake >= 3.16
- SDL3 (included in vendored/)
- C++17 compiler (MSVC 2026, GCC, Clang)

### Build (Windows)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 18 2026"
cmake --build . --config Release
.\Release\runtime.exe
```

### Build (Linux/Mac)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
./Release/runtime
```

## Project Structure
```
native/                           # Core engine library
  ├── include/
  │   ├── VM_Value.h             # Value type system
  │   ├── VM_Instruction.h       # Instruction set & opcodes
  │   └── VM_Executor.h          # Bytecode executor
  └── src/
      ├── VM_Value.cpp           # Value arithmetic/operations
      ├── VM_Executor.cpp        # Execution engine
      ├── Platform_SDL.cpp       # SDL3 rendering backend
      └── AssetLoader.cpp        # JSON asset loading

runtime/                          # Game runtime entry point
  └── main.cpp                    # Engine initialization

tools/                            # Extraction utilities
  └── extractor.py              # Extract GML from game dumps

scripts/                          # JavaScript runtime (for reference)
vendored/                         # SDL3 source
```

## VM Instruction Set (Partial List)

| Category | Instructions |
|----------|--------------|
| **Stack** | PUSH, POP, PUSHI, PUSHF, PUSHS, PUSHB, DUP, DROP |
| **Arithmetic** | ADD, SUB, MUL, DIV, MOD, NEG |
| **Bitwise** | AND, OR, XOR, COM, SHL, SHR |
| **Comparison** | TEQ, TNE, TLT, TLE, TGT, TGE |
| **Logical** | LAND, LOR, NOT |
| **Control** | JMP, BT, BF, RET, CALL, EXIT |
| **Variables** | LDGLB, STGLB, LDLOC, STLOC, LDINST, STINST |

## Testing

Run the VM test suite:
```bash
cd build/Release
vm_test.exe    # Verifies basic arithmetic (5+3=8)
```

## Current Status

✅ **Completed:**
- VM value type system with all operators
- Complete instruction set (40+ opcodes)
- Stack-based execution engine with call frames
- Built-in math functions
- Test harness (arithmetic operations verified)
- SDL3 rendering backend (60 FPS, cross-platform)
- Python asset extraction (6,272+ GML code objects)

🔄 **In Progress:**
- Load extracted code objects into VM
- Execute real GML bytecode from Undertale

⏳ **Future:**
- Enhanced built-in function library
- Event system (Create, Step, Draw, Collision, etc.)
- Instance and dynamic variable management
- Room/object instantiation from extracted data
- Console/mobile platform ports

## Next Steps

1. **Load game code** from extracted JSON into VM
2. **Execute basic GML** (test with simple extracted functions)
3. **Implement event loop** (Create→Step→Draw cycle)
4. **Add instance system** (spawn/manage game objects)
5. **Improve extraction** for room/sprite/sound data
